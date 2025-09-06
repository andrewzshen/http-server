#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define PORT_NUMBER 6969
#define IP_ADDRESS "127.0.0.1"

int main(int argc, char **argv) {
    int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    socklen_t option = 1;
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option)) == -1 ||
       setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
        perror("setsockopt failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    inet_pton(AF_INET, IP_ADDRESS, &server_addr.sin_addr);
    memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    if(bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    int connection_backlog = 10;

    if(listen(server_socket, connection_backlog) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d. Waiting for a client to connect...\n", PORT_NUMBER);

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_len);

    if(client_socket == -1) {
        perror("Accept failed");
        close(client_socket);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Client connected. Ready to receive messages.\n");

    const char *response_prefix = "Message received! Echo: ";
    const size_t prefix_len = strlen(response_prefix);

#define BUFFER_SIZE 1024

    char message_buffer[BUFFER_SIZE];
    char response_buffer[BUFFER_SIZE];

    ssize_t bytes_received;

    while((bytes_received = recv(client_socket, message_buffer, BUFFER_SIZE, 0)) > 0) {
        int bytes_written = snprintf(response_buffer, BUFFER_SIZE, "Message received. Echo: \"%.*s\"", bytes_received, message_buffer); 
        
        if(bytes_written == -1) {
            perror("Buffer write failed");
            close(server_socket);
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        ssize_t bytes_sent = send(client_socket, response_buffer, bytes_written, 0);

        if(bytes_sent == -1) {
            perror("Send failed");
            close(server_socket);
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        printf("Echoed back %zd bytes to client.\n", bytes_sent);
    }

    if(bytes_received == -1) {
        perror("Receive failed");
        close(client_socket);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connection closed.\n");

    close(client_socket);
    close(server_socket);

    return 0;
}   