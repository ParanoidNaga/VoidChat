#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include "udp_node.h"

#define MAXLINE 1024

UdpNode::UdpNode(int bind_port) : listen_port(bind_port){
  
  if(sodium_init() == -1){
    std::cerr << "libsodium error" << std::endl;
    exit(1);
  }

  crypto_box_keypair(my_pkey, my_skey);
  std::cout << "your public key: " << get_my_pubkey_hex() << "\n\n";

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
}
  UdpNode::~UdpNode(){
    if(sockfd>=0){
      close(sockfd);
      std::cout << "udp socket closed" << std::endl;
    }
  }
  
  
  std::string UdpNode::get_my_pubkey_hex() const{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < crypto_box_PUBLICKEYBYTES; ++i) {
        oss << std::setw(2) << static_cast<unsigned>(my_pkey[i]);
    }
    return oss.str();
  }
  
  bool UdpNode::set_peer_pubkey_from_hex(const std::string& hex){
    if(hex.length()!= crypto_box_PUBLICKEYBYTES * 2){
      std::cout "wrong public key length";
      return false;
    }

    for (size_t i = 0; i < crypto_box_PUBLICKEYBYTES; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        try {
            peer_publickey[i] = static_cast<unsigned char>(std::stoul(byte_str, nullptr, 16));
        } catch (...) {
            std::cout << "Invalid hex character in public key\n";
            return false;
        }
    }
    have_peer = true;
    std::cout "user's public key accepted" << std::endl;
    return true;
  }

  std::string UdpNode::encrypt(const std::string& plaintext) const{
    if(!have_peer) return plaintext;
    unsigned char nonce[crypto_box_NONCEBYTES];
    randombytes_buf(nonce, sizeof(nonce));
    
    std::vector<unsigned char> ciphertext(
        crypto_box_MACBYTES + plaintext.size()
    );

    if (crypto_box_easy(
            ciphertext.data(),
            reinterpret_cast<const unsigned char*>(plaintext.data()),
            plaintext.size(),
            nonce,
            peer_publickey,
            my_skey) != 0) {
        return "[encryption error]";
    }

    std::string packet(
        reinterpret_cast<const char*>(nonce), sizeof(nonce);
    );
    packet.append(reinterpret_cast<const char*>(ciphertext.data(), ciphertext.size()));
    
    return packet;
  }
  

  std::string UdpNode::decrypt(const std::string& packet) const{
    if(!have_peer || packet.size() < crypto_box_NONCEBYTES + crypto_box_MACBYTES){
      return packet;
    }
    const unsigned char* nonce = reinterpret_cast<const unsigned char*> (packet.data());
    const unsigned char* ct = nonce+ crypto_box_NONCEBYTES;
    size_t ct_len = packet.size() - crypto_box_NONCEBYTES;

    std::vector<unsigned char> plaintext(ct_len - crypto_box_MACBYTES);
  
    if (crypto_box_open_easy(
            plaintext.data(),
            ct,
            ct_len,
            nonce,
            peer_publickey,
            my_skey) != 0) {
        return "[DECRYPT FAILED - wrong key or damaged packet]";
    }
    return std::string(plaintext.begin(), plaintext.end());
  }

  void UdpNode::listen(){
    if(sockfd < 0) return;

    char buffer[MAXLINE];
    sockaddr_in cliaddr{};
    socklen_t len = sizeof(cliaddr);
    
    std::cout << "udp listening on port " << listen_port << "..." << std::endl;

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
};
 
