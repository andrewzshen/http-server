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
    int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(client_fd == -1) {
        perror("Socket creation failed.");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    inet_pton(AF_INET, IP_ADDRESS, &server_addr.sin_addr);
    memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    if(connect(client_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed.");
        close(client_fd);
        return 1;
    }

    char buffer[1024];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);  

    if(bytes_received == -1) {
        perror("Receive failed.");
        close(client_fd);
        return 1;
    } 

    printf("Received: %s", buffer);

    close(client_fd);

    return 0;
}   
