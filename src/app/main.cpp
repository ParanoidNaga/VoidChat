#include <iostream>
#include <string>
#include <thread>

#include "../network/udp_node.h"

int main(int argc, char* argv[]){
    if (argc != 4) {
        std::cout << "Usage:\n";
        std::cout << "  ./chat <listen_port> <peer_ip> <peer_port>\n";
        return 1;
    }

    int listen_port = std::stoi(argv[1]);
    std::string peer_ip = argv[2];
    int peer_port = std::stoi(argv[3]);

    std::cout << "Starting udp chat...\n";
    std::cout << "Listening on port " << listen_port << "\n";
    std::cout << "Sending to " << peer_ip << ":" << peer_port << "\n";
    std::cout << "Type # to exit\n";

    std::thread recv_thread(udp_listen, listen_port);
    std::thread send_thread(udp_send_loop, peer_ip, peer_port);

    recv_thread.join();
    send_thread.join();

    return 0;
}

