#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdio>
#include <sys/types.h> 

#define MULTICAST_ADDR "239.255.0.1"
#define MULTICAST_PORT 4950
#define MAX_MSG_LEN 1024

int main()
{
    int sockfd;
    struct sockaddr_in their_addr;
    struct ip_mreq mreq;
    int numbytes;
    int ttl = 1; // Example TTL value
    char message[MAX_MSG_LEN];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return 1;
    }

    // Set TTL for multicast packets
    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("setsockopt (IP_MULTICAST_TTL)");
        return 1;
    }

    // Set up multicast group membership
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); // Use default interface

    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt (IP_ADD_MEMBERSHIP)");
        return 1;
    }

    their_addr.sin_family = AF_INET; // host byte order
    their_addr.sin_port = htons(MULTICAST_PORT); // short, network byte order
    their_addr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR); 
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

    std::cout << "Enter message: ";
    std::cin.getline(message, MAX_MSG_LEN);

    if ((numbytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) {
        perror("sendto");
        return 1;
    }

    std::cout << "sent " << numbytes << " bytes to " << MULTICAST_ADDR << std::endl;

    close(sockfd);

    return 0;
}




// #include <iostream>
// #include <cstdlib>
// #include <cstring>
// #include <unistd.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <netdb.h>
// #include <cstdio>

// #define SERVERPORT 4950    // the port users will be connecting to

// int main(int argc, char *argv[])
// {
//     int sockfd;
//     struct sockaddr_in their_addr; // connector's address information
//     struct hostent *he;
//     int numbytes;
//     int broadcast = 1;

//     if (argc != 3) {
//         std::cerr << "usage: broadcaster hostname message" << std::endl;
//         return 1;
//     }

//     if ((he = gethostbyname(argv[1])) == nullptr) {  // get the host info
//         perror("gethostbyname");
//         return 1;
//     }

//     if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {  // Using AF_INET for Internet address family
//         perror("socket");
//         return 1;
//     }

//     // this call is what allows broadcast packets to be sent:
//     if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
//         perror("setsockopt (SO_BROADCAST)");
//         return 1;
//     }

//     their_addr.sin_family = AF_INET; // host byte order
//     their_addr.sin_port = htons(SERVERPORT); // short, network byte order
//     their_addr.sin_addr = *((struct in_addr *)he->h_addr);
//     std::memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

//     if ((numbytes = sendto(sockfd, argv[2], std::strlen(argv[2]), 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) {
//         perror("sendto");
//         return 1;
//     }

//     std::cout << "sent " << numbytes << " bytes to " << inet_ntoa(their_addr.sin_addr) << std::endl;

//     close(sockfd);

//     return 0;
// }

