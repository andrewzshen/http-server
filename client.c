#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define PORT_NUMBER 6969
#define IP_ADDRESS "127.0.0.1"

int main(int argc, char **argv) {
    int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    inet_pton(AF_INET, IP_ADDRESS, &server_addr.sin_addr);
    memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    if(connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Ready to receive messages:\n");

    char message_buffer[1024];

    while(fgets(message_buffer, sizeof(message_buffer), stdin) != NULL) {
        size_t newline_index = strcspn(message_buffer, "\n");
        message_buffer[newline_index] = '\0';

        ssize_t bytes_sent = send(client_socket, message_buffer, strlen(message_buffer), 0);

        if(bytes_sent == -1) {
            perror("Send error");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        ssize_t bytes_received = recv(client_socket, message_buffer, sizeof(message_buffer), 0);  

        if(bytes_received == -1) {
            perror("Receive failed");
            close(client_socket);
            exit(EXIT_FAILURE);
        } 

        printf("%.*s\n", bytes_received, message_buffer);
    }

    close(client_socket);

    return 0;
}   
