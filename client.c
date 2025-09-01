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
        exit(errno);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    inet_pton(AF_INET, IP_ADDRESS, &server_addr.sin_addr);
    memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    if(connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        close(client_socket);
        exit(errno);
    }

    printf("Connected to server. Input messages:\n");

    char buffer[1024];
    ssize_t bytes_sent;

    while(fgets(buffer, sizeof(buffer), stdin) != NULL) {
        size_t newline_index = strcspn(buffer, "\n");
        buffer[newline_index] = 0;

        printf("Message length in bytes (excluding newline): %d\n", strlen(buffer));
        bytes_sent = send(client_socket, buffer, strlen(buffer), 0);

        if(bytes_sent == -1) {
            perror("Send error.");
            close(client_socket);
            exit(errno);
        }

        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);  

        if(bytes_received == -1) {
            perror("Receive failed");
            close(client_socket);
            exit(errno);
        } 

        printf("%.*s\n", bytes_received, buffer);
    }

    close(client_socket);

    return 0;
}   
