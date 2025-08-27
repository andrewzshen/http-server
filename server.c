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
    int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(server_fd == -1) {
        perror("Socket creation failed.");
        return 1;
    }

    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        perror("Set SO_REUSEADDR failed.");
        close(server_fd);
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    inet_pton(AF_INET, IP_ADDRESS, &server_addr.sin_addr);
    memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    if(bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed.");
        close(server_fd);
        return 1;
    }

    int connection_backlog = 10;

    if(listen(server_fd, connection_backlog) == -1) {
        perror("Listen failed.");
        close(server_fd);
        return 1;
    }

    printf("Waiting for a client to connect...\n");

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);

    if(client_fd == -1) {
        perror("Accept failed.");
        close(server_fd);
        return 1;
    }

    printf("Client connected.\n");

    char *message = "Hello client!\n";
    ssize_t bytes_sent = send(client_fd, message, strlen(message), 0);

    if(bytes_sent == -1) {
        perror("Send failed.");
        close(client_fd);
        close(server_fd);
        return 1;
    }

    close(client_fd);
    close(server_fd);

    return 0;
}   
