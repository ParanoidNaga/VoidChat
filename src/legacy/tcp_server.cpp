#include <iostream>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <unistd.h>     
#include <cstring>      
#include "tcp_server.h"


#define ERROR_S "SERVER ERROR: "
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION_SYMBOL '#'

bool is_client_connection_close_server(const char* msg) {
    for (int i = 0; i < strlen(msg); ++i) {
        if (msg[i] == CLIENT_CLOSE_CONNECTION_SYMBOL)
            return true;
    }
    return false;
}
int tcp_server_start(int port) {
    int server_socket, client_socket;
    struct sockaddr_in server_address;

    // create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << ERROR_S << "Failed to create socket\n";
        return 1;
    }
    std::cout << "SERVER: Socket successfully created\n";

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << ERROR_S << "Binding failed\n";
        return 1;
    }

    //listen
    if (listen(server_socket, 1) < 0) {
        std::cerr << ERROR_S << "Listen failed\n";
        return 1;
    }
    std::cout << "SERVER: Listening for clients on port " << port << "...\n";

    socklen_t addr_len = sizeof(server_address);
    client_socket = accept(server_socket, (struct sockaddr*)&server_address, &addr_len);
    if (client_socket < 0) {
        std::cerr << ERROR_S << "Failed to accept client\n";
        return 1;
    }
    std::cout << "SERVER: Client connected\n";

    char buffer[BUFFER_SIZE];
    bool exit_flag = false;

    //message to client
    strcpy(buffer, "=> Server connected!\n");
    send(client_socket, buffer, strlen(buffer), 0);

    while (!exit_flag) {
        //receive message from client
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            std::cout << "SERVER: Client disconnected\n";
            break;
        }
        buffer[bytes_received] = '\0';
        std::cout << "Client: " << buffer << std::endl;

        if (is_client_connection_close_server(buffer)) {
            std::cout << "SERVER: Client requested to close connection\n";
            break;
        }

        //send message
        std::cout << "Server: ";
        std::cin.getline(buffer, BUFFER_SIZE);
        send(client_socket, buffer, strlen(buffer), 0);

        if (is_client_connection_close_server(buffer)) {
            std::cout << "SERVER: Closing connection as requested\n";
            exit_flag = true;
        }
    }

    close(client_socket);
    close(server_socket);
    std::cout << "SERVER: Goodbye!\n";
    return 0;
}

