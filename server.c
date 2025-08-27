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

int main(int argc, char **argv) {
    int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(server_fd == -1) {
        perror("Socket creation failed.");
        return 1;
    }

    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("SO_REUSEPORT failed.");
        close(server_fd);
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
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

    char *message = "Hello client\n";
    send(client_fd, message, strlen(message), 0);

    close(client_fd);
    close(server_fd);

    return 0;
}   
