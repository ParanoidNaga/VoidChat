#include <iostream>
#include <string>
#include <thread>
#include "../network/udp_node.h"

void send_loop(UdpNode& node, const std::string& peer_ip, int peer_port) {
    std::string message;
    std::cout << "Type messages (or # to exit):\n";
    while (true) {
        std::getline(std::cin, message);
        if (message == "#") {
            std::cout << "Exiting send loop\n";
            break;
        }
        if (!message.empty()) {
            node.send(peer_ip, peer_port, message);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: ./chat <listen_port> <peer_ip> <peer_port>\n";
        return 1;
    }

    int listen_port = std::stoi(argv[1]);
    std::string peer_ip = argv[2];
    int peer_port = std::stoi(argv[3]);

    std::cout << "Starting UDP chat...\n";
    std::cout << "Listening on: " << listen_port << "\n";
    std::cout << "Sending to: " << peer_ip << ":" << peer_port << "\n";
    std::cout << "Type # to exit\n";

    UdpNode node(listen_port); //i guess here are the objects created, thats kind of cool, becuase there is only one socket created

  
    std::thread recv_thread(&UdpNode::listen, &node);
    std::thread send_thread(send_loop, std::ref(node), peer_ip, peer_port);

    recv_thread.join();
    send_thread.join();

    return 0;
}
