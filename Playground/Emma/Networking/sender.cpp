#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXBUFLEN 100
#define PORT "5005"

int main() {
    // With reference to https://beej.us/guide/bgnet/
    addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    int status;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    
    char s[INET6_ADDRSTRLEN];

    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    int sockfd;
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    // Turn on reuse port
    int one = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(int)) < 0) {
        perror("setsockopt");
    }

    char istr[MAXBUFLEN];
    sockaddr_in* ipv4 = (sockaddr_in*) res->ai_addr;
    inet_ntop(res->ai_family, &ipv4->sin_addr, istr, MAXBUFLEN);
    printf("Sending on %s:%s\n", istr, PORT);

    char* string = "Hello!";
    if ((numbytes = sendto(sockfd, string, strlen(string), 0, res->ai_addr, res->ai_addrlen)) == -1) {
        perror("sender: sendto");
        exit(1);
    }
    printf("Sent %d bytes\n", numbytes);
    freeaddrinfo(res);
    close(sockfd);

}