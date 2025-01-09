#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define MULTICAST_ADDR "239.255.0.1"
#define MULTICAST_PORT 4950

int main() {
    int sockfd;
    struct sockaddr_in addr;
    struct ip_mreq mreq;
    char buffer[1024];

    // Create the socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    // Allow multiple sockets to use the same PORT number
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt (SO_REUSEADDR)");
        return 1;
    }

    // Bind to the multicast port
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all interfaces
    addr.sin_port = htons(MULTICAST_PORT);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    // Join the multicast group
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt (IP_ADD_MEMBERSHIP)");
        return 1;
    }

    std::cout << "Listening for multicast messages on " << MULTICAST_ADDR << ":" << MULTICAST_PORT << std::endl;

    // Receive messages
    while (true) {
        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
        if (n < 0) {
            perror("recvfrom");
            return 1;
        }
        buffer[n] = '\0';
        std::cout << "Received message: " << buffer << std::endl;
    }

    // Cleanup
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
// #include <sys/types.h> 

// #define MULTICAST_ADDR "239.255.0.1"
// #define MULTICAST_PORT "4950"
// #define MAXBUFLEN 100

// // get sockaddr, IPv4 or IPv6:
// void* get_in_addr(struct sockaddr *sa)
// {
//     if (sa->sa_family == AF_INET) {
//         return &(((struct sockaddr_in*)sa)->sin_addr);
//     }
//     return &(((struct sockaddr_in6*)sa)->sin6_addr);
// }

// int main()
// {
//     int sockfd;
//     struct addrinfo hints, *servinfo, *p;
//     int rv;
//     int numbytes;
//     struct sockaddr_storage their_addr;
//     char buf[MAXBUFLEN];
//     socklen_t addr_len;
//     char s[INET6_ADDRSTRLEN];
//     int optval = 1;
//     struct ip_mreq mreq; 

//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_INET; // set to AF_INET to force IPv4
//     hints.ai_socktype = SOCK_DGRAM;
//     hints.ai_flags = AI_PASSIVE; // use my IP

//     if ((rv = getaddrinfo(NULL, MULTICAST_PORT, &hints, &servinfo)) != 0) {
//         std::cerr << "getaddrinfo: " << gai_strerror(rv) << std::endl;
//         return 1;
//     }

//     // loop through all the results and bind to the first we can
//     for (p = servinfo; p != nullptr; p = p->ai_next) {
//         if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
//             perror("listener: socket");
//             continue;
//         }

//         if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0) {
//             perror("listener: setsockopt");
//             continue;
//         }

//         if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
//             close(sockfd);
//             perror("listener: bind");
//             continue;
//         }

//         break;
//     }

//     if (p == nullptr) {
//         std::cerr << "listener: failed to bind socket" << std::endl;
//         return 2;
//     }

//     freeaddrinfo(servinfo);

//     // Set up multicast group membership
//     mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
//     //mreq.imr_interface.s_addr = htonl(INADDR_ANY); // Use the default interface
//     mreq.imr_interface.s_addr = inet_addr("192.168.1.141"); // Replace with your en0 IP address

//     if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
//         perror("setsockopt (IP_ADD_MEMBERSHIP)");
//         return 1;
//     }

//     std::cout << "listener: waiting to recvfrom..." << std::endl;

//     addr_len = sizeof(their_addr);
//     if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
//         perror("recvfrom");
//         return 1;
//     }

//     std::cout << "listener: got packet from " << inet_ntop(their_addr.ss_family,
//             get_in_addr((struct sockaddr *)&their_addr),
//             s, sizeof(s)) << std::endl;
//     std::cout << "listener: packet is " << numbytes << " bytes long" << std::endl;
//     buf[numbytes] = '\0';
//     std::cout << "listener: packet contains \"" << buf << "\"" << std::endl;

//     close(sockfd);

//     return 0;
// }

// #include <iostream>
// #include <cstdlib>
// #include <cstring>
// #include <unistd.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <netdb.h>

// #define MYPORT "4950"    // the port users will be connecting to
// #define MAXBUFLEN 100

// // get sockaddr, IPv4 or IPv6:
// void* get_in_addr(struct sockaddr *sa)
// {
//     if (sa->sa_family == AF_INET) {
//         return &(((struct sockaddr_in*)sa)->sin_addr);
//     }
//     return &(((struct sockaddr_in6*)sa)->sin6_addr);
// }

// int main()
// {
//     int sockfd;
//     struct addrinfo hints, *servinfo, *p;
//     int rv;
//     int numbytes;
//     struct sockaddr_storage their_addr;
//     char buf[MAXBUFLEN];
//     socklen_t addr_len;
//     char s[INET6_ADDRSTRLEN];
//     int optval = 1;

//     std::memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
//     hints.ai_socktype = SOCK_DGRAM;
//     hints.ai_flags = AI_PASSIVE; // use my IP

//     if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
//         std::cerr << "getaddrinfo: " << gai_strerror(rv) << std::endl;
//         return 1;
//     }

//     // loop through all the results and bind to the first we can
//     for (p = servinfo; p != nullptr; p = p->ai_next) {
//         if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
//             perror("listener: socket");
//             continue;
//         }

//         if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0) {
//             perror("listener: setsockopt");
//             continue;
//         }

//         if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
//             close(sockfd);
//             perror("listener: bind");
//             continue;
//         }

//         break;
//     }

//     if (p == nullptr) {
//         std::cerr << "listener: failed to bind socket" << std::endl;
//         return 2;
//     }

//     freeaddrinfo(servinfo);

//     std::cout << "listener: waiting to recvfrom..." << std::endl;

//     addr_len = sizeof(their_addr);
//     if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
//         perror("recvfrom");
//         return 1;
//     }

//     std::cout << "listener: got packet from " << inet_ntop(their_addr.ss_family,
//             get_in_addr((struct sockaddr *)&their_addr),
//             s, sizeof(s)) << std::endl;
//     std::cout << "listener: packet is " << numbytes << " bytes long" << std::endl;
//     buf[numbytes] = '\0';
//     std::cout << "listener: packet contains \"" << buf << "\"" << std::endl;

//     close(sockfd);

//     return 0;
// }
