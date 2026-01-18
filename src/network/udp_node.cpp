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

void udp_listen(int port){
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket failed");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if(bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror("bind failed");
        close(sockfd);
        return;
    }

    std::cout << "udp listening on port " << port << "..." << std::endl;

    socklen_t len = sizeof(cliaddr);
    while(true){
        int n = recvfrom(sockfd, buffer, MAXLINE, 0,
                         (struct sockaddr*) &cliaddr, &len);
        if(n < 0){
            perror("recvfrom");
            continue;
        }

        buffer[n] = '\0';

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cliaddr.sin_addr, ip, sizeof(ip));
        std::cout << "[" << ip << ":" << ntohs(cliaddr.sin_port)
                  << "] " << buffer << "\n";
    }

    close(sockfd);
}

void udp_send(const std::string& ip_str, int port,
              const std::string& message){
    int sockfd;
    struct sockaddr_in servaddr;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket failed");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port); inet_pton(AF_INET, ip_str.c_str(), &servaddr.sin_addr);
    ssize_t sent = sendto(sockfd,
                          message.c_str(),
                          message.size(),
                          0,
                          (const struct sockaddr *) &servaddr,
                          sizeof(servaddr));

    if(sent < 0){
        perror("sendto");
    } else {
        std::cout << "Send to " << ip_str << ":" << port
                  << " -> " << message << "\n";
    }

    close(sockfd);
}


void udp_send_loop(const std::string& ip, int port){
    std::string message;

    while (true) {
        std::getline(std::cin, message);

        if (message == "#") {
            std::cout << "Exiting send loop\n";
            break;
        }

        if (!message.empty()) {
            udp_send(ip, port, message);
        }
    }
}
