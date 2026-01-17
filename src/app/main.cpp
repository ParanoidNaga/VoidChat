#include <iostream>
#include <string>

#include "../legacy/tcp_client.h"
#include "../legacy/tcp_server.h"



int main(int argc, const char* argv[]){
  if(argc < 2){
    std::cout << "Usage: ./chat [server <port> | client <ip> <port>]\n";
    return 1;
  }
  std::string mode = argv[1];

  if (mode == "server") {
        if (argc != 3) { std::cerr << "Need port\n"; return 1; }
        int port = std::stoi(argv[2]);
        tcp_server_start(port);
  } else if (mode == "client") {
        if (argc != 4) { std::cerr << "Need ip and port\n"; return 1; }
        std::string ip = argv[2];
        int port = std::stoi(argv[3]);
        tcp_client_start(ip.c_str(), port);
    } else {
        std::cout << "Unknown mode. Later: p2p mode coming...\n";
    }


  return 0;
}
