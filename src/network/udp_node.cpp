#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include "udp_node.h"

#define MAXLINE 1024

UdpNode::UdpNode(int bind_port) : listen_port(bind_port){
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0){
        perror("socket failed");
        return;
    }
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(listen_port);
  addr.sin_addr.s_addr = INADDR_ANY;
  
  if(bind(sockfd, (sockaddr* )&addr, sizeof(addr)) < 0){
    perror("bind failed");
    close(sockfd);
    sockfd = -1;
    return;
  }

  std::cout << "udp node listening on port " << listen_port << "..." << std::endl;

  UdpNode::~UdpNode(){
    if(sockfd>=0){
      close(sockfd);
      std::cout << "udp socket closed" << std::endl;
    }
  }

  void UdpNode::listen(){
    if(sockfd < 0) return;

    char buffer[MAXLINE];
    sockaddr_in cliaddr{};
    socklen_t len = sizeof(cliaddr);
    
    std::cout << "udp listening on port " << port << "..." << std::endl;

    while(true){
      int n = recvfrom(sockfd, buffer, MAXLINE, 0, (sockaddr*)& cliaddr, &len);
      if(n < 0){
        perror("recvfrom");
        continue;
      }
      buffer[n] = '\0';

      char ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &cliaddr.sin_addr, ip, sizeof(ip));
      std::cout << "[" << ip << ":" << ntohs(cliaddr.sin_port) << "] " << buffer << "\n";
    }
  }

  void UdpNode::send(const std::string& ip_str, int port, const std::string& message){
    if(sockfd < 0){
      std::cout << "no socket available" << std::endl;
      return;
    }
    sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, ip_str.c_str(), &dest.sin_addr);

    ssize_t sent = sendto(sockfd, message.c_str(), message.size(), 0, (sockaddr*)&dest, sizeof(dest));
    if(sent <2)
      perror("sendto");
    else {
      std::cout << "Sent to " << ip_str << ":" << port << " -> " << message << "\n";
    }
  }
};
 
