#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "tcp_client.h"


#define ERROR_S "CLIENT ERROR: "
#define SERVER_CLOSE_CONNECTION_SYMBOL '#'
#define BUFFER_SIZE 1024

bool is_client_connection_close_client(const char* msg);

int tcp_client_start(const char* ip, int port) {
    int client;
    struct sockaddr_in server_address;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        std::cout << ERROR_S << "establishing socket error.\n";
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_address.sin_addr);

    std::cout << "=> Client socket created.\n";

    int ret = connect(
        client,
        (struct sockaddr*)&server_address,
        sizeof(server_address)
    );

    if (ret < 0) {
        std::cout << ERROR_S << "connection failed.\n";
        return 1;
    }

    std::cout << "=> Connected to server "
              << ip
              << " on port "
              << port << "\n";

    char buffer[BUFFER_SIZE];

    int bytes = recv(client, buffer, BUFFER_SIZE - 1, 0);
    buffer[bytes] = '\0';
    std::cout << buffer << std::endl;

    while (true) {
        std::cout << "Client: ";
        std::cin.getline(buffer, BUFFER_SIZE);

        send(client, buffer, strlen(buffer), 0);

        if (is_client_connection_close_client(buffer))
            break;

        bytes = recv(client, buffer, BUFFER_SIZE - 1, 0);
        buffer[bytes] = '\0';

        std::cout << "Server: " << buffer << std::endl;

        if (is_client_connection_close_client(buffer))
            break;
    }

    close(client);
    std::cout << "Goodbye...\n";
    return 0;
}

bool is_client_connection_close_client(const char* msg) {
    for (int i = 0; i < strlen(msg); ++i) {
        if (msg[i] == SERVER_CLOSE_CONNECTION_SYMBOL)
            return true;
    }
    return false;
}

