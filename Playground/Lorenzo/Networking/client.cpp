#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <iostream>

static const int MAXBUFLEN = 512;

// bind a UDP socket to an ephemeral port (0) so it can receive data
int bindEphemeralPort(int sockfd)
{
    sockaddr_in myAddr;
    std::memset(&myAddr, 0, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = 0; // 0 => let OS choose ephemeral port

    if (bind(sockfd, (sockaddr *)&myAddr, sizeof(myAddr)) == -1)
    {
        perror("client: bind ephemeral");
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    // expect: client <router-host> <router-port>
    if (argc != 3)
    {
        std::fprintf(stderr, "Usage: %s <router-host> <router-port>\n", argv[0]);
        return 1;
    }
    const char *routerHost = argv[1];
    const char *routerPort = argv[2];

    // 1) Resolve router address
    addrinfo hints, *routerinfo;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rv = getaddrinfo(routerHost, routerPort, &hints, &routerinfo);
    if (rv != 0)
    {
        std::fprintf(stderr, "client: getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // create our UDP socket
    int sockfd = socket(routerinfo->ai_family, routerinfo->ai_socktype, routerinfo->ai_protocol);
    if (sockfd == -1)
    {
        perror("client: socket");
        freeaddrinfo(routerinfo);
        return 1;
    }

    // bind to an ephemeral port so we can receive data
    if (bindEphemeralPort(sockfd) == -1)
    {
        close(sockfd);
        freeaddrinfo(routerinfo);
        return 1;
    }

    // print out port
    sockaddr_in localAddr;
    socklen_t len = sizeof(localAddr);
    if (getsockname(sockfd, (sockaddr *)&localAddr, &len) == -1)
    {
        perror("client: getsockname");
    }
    else
    {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &localAddr.sin_addr, ipStr, sizeof(ipStr));
        std::printf("Client bound to %s:%d\n", ipStr, ntohs(localAddr.sin_port));
    }

    // prompt user for a message, then send to router
    std::string message;
    std::cout << "Enter a message to send to router: ";
    if (!std::getline(std::cin, message))
    {
        std::cerr << "Error reading from stdin\n";
    }

    ssize_t sentBytes = sendto(sockfd,
                               message.c_str(),
                               message.size(),
                               0,
                               routerinfo->ai_addr,
                               routerinfo->ai_addrlen);
    if (sentBytes == -1)
    {
        perror("client: sendto");
    }
    else
    {
        std::printf("Sent %zd bytes to router.\n", sentBytes);
    }

    freeaddrinfo(routerinfo);

    // wait to receive something from the router (blocking!!)
    char buf[MAXBUFLEN];
    sockaddr_in fromAddr;
    socklen_t fromLen = sizeof(fromAddr);

    ssize_t numBytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0,
                                (sockaddr *)&fromAddr,
                                &fromLen);
    if (numBytes == -1)
    {
        perror("client: recvfrom");
        close(sockfd);
        return 1;
    }
    buf[numBytes] = '\0';

    // print the message
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &fromAddr.sin_addr, ipStr, sizeof(ipStr));
    std::printf("Received from %s:%d => \"%s\"\n",
                ipStr, ntohs(fromAddr.sin_port), buf);

    // cleanup
    close(sockfd);
    return 0;
}
