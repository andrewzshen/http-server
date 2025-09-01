#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define PORT_NUMBER 6969
#define IP_ADDRESS "127.0.0.1"

int main(int argc, char **argv) {
    int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(server_socket == -1) {
        perror("Socket creation failed");
        exit(errno);
    }

    int option = 1;
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option)) == -1) {
        perror("Set SO_REUSEADDR failed");
        close(server_socket);
        exit(errno);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    inet_pton(AF_INET, IP_ADDRESS, &server_addr.sin_addr);
    memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    if(bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(errno);
    }

    int connection_backlog = 10;

    if(listen(server_socket, connection_backlog) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(errno);
    }

    printf("Waiting for a client to connect...\n");

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_len);

    if(client_socket == -1) {
        perror("Accept failed");
        close(client_socket);
        close(server_socket);
        exit(errno);
    }

    printf("Client connected. Ready to receive messages.\n");

    const char *response_prefix = "Received message from client! Message: ";
    const size_t prefix_len = strlen(response_prefix);

    // Because the initial response prefix is always known at compile time, it is memcpy'ed into a buffer
    // and then the actual message you want to echo is recv into the same buffer just starting after the prefix
    size_t buffer_size = 1024;
    char buffer[buffer_size];
    memcpy(buffer, response_prefix, prefix_len);

    ssize_t bytes_received;

    // Only receive buffer_size - prefix_len bytes because the buffer is of size buffer_size, so the maximum number
    // number of bytes you can receive is their difference 
    while((bytes_received = recv(client_socket, buffer + prefix_len, buffer_size - prefix_len, 0)) > 0) {
        ssize_t bytes_sent = send(client_socket, buffer, prefix_len + bytes_received, 0); 

        if(bytes_sent == -1) {
            perror("Send failed");
            close(server_socket);
            close(client_socket);
            exit(errno);
        }

        printf("Message received. Response: '%.*s'\n", bytes_sent, buffer);
    }

    if(bytes_received == -1) {
        perror("Receive failed");
        close(client_socket);
        close(server_socket);
        exit(errno);
    }

    printf("Connection closed.\n");

    close(client_socket);
    close(server_socket);

    return 0;
}   