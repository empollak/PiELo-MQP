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
#include <ctime>
#include <sstream>
#include <random>

#define MULTICAST_ADDR "239.255.0.1"
#define MULTICAST_PORT 4950
#define MAX_MSG_LEN 1024

// Function to generate a random port for each sender instance
int generate_unique_port_id() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(49152, 65535);  // Range for dynamic ports
    return dis(gen);
}

int main()
{
    int sockfd;
    struct sockaddr_in their_addr;
    struct ip_mreq mreq;
    int numbytes;
    int ttl = 1;
    char message[MAX_MSG_LEN];

    int unique_port_id = generate_unique_port_id();
    // Generate a unique ID for the sender (combination of address and port)
    std::string sender_id = MULTICAST_ADDR + std::to_string(unique_port_id);
    
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

    // Send "I'm alive" message initially
    snprintf(message, sizeof(message), "I'm alive from %s", sender_id.c_str());

    if ((numbytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&their_addr, sizeof(their_addr))) == -1) {
        perror("sendto");
        return 1;
    }

    std::cout << "Sent 'I'm alive' message: " << message << std::endl;

    // You can loop this periodically if needed to keep the "I'm alive" message going
    while (true) {
        std::cout << "Enter message: ";
        std::cin.getline(message, MAX_MSG_LEN);

        if ((numbytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&their_addr, sizeof(their_addr))) == -1) {
            perror("sendto");
            return 1;
        }

        std::cout << "sent " << numbytes << " bytes to " << MULTICAST_ADDR << std::endl;
    }

    // Listen for a response from the receiver
    struct sockaddr_in response_addr;
    socklen_t addr_len = sizeof(response_addr);
    char buffer[MAX_MSG_LEN];

    int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&response_addr, &addr_len);
    if (n < 0) {
        perror("recvfrom");
        return 1;
    }
    buffer[n] = '\0';
    std::cout << "Received response: " << buffer << std::endl;



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
// #include <sys/types.h> 

// #define MULTICAST_ADDR "239.255.0.1"
// #define MULTICAST_PORT 4950
// #define MAX_MSG_LEN 1024

// int main()
// {
//     int sockfd;
//     struct sockaddr_in their_addr;
//     struct ip_mreq mreq;
//     int numbytes;
//     int ttl = 1;
//     //int ttl = 1; // Example TTL value
//     char message[MAX_MSG_LEN];

//     if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
//         perror("socket");
//         return 1;
//     }

//     // Set TTL for multicast packets
//     if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
//         perror("setsockopt (IP_MULTICAST_TTL)");
//         return 1;
//     }

//     // Set up multicast group membership
//     mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
//     mreq.imr_interface.s_addr = htonl(INADDR_ANY); // Use default interface

//     if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
//         perror("setsockopt (IP_ADD_MEMBERSHIP)");
//         return 1;
//     }

//     their_addr.sin_family = AF_INET; // host byte order
//     their_addr.sin_port = htons(MULTICAST_PORT); // short, network byte order
//     their_addr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR); 
//     memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

//     std::cout << "Enter message: ";
//     std::cin.getline(message, MAX_MSG_LEN);

//     if ((numbytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) {
//         perror("sendto");
//         return 1;
//     }

//     std::cout << "sent " << numbytes << " bytes to " << MULTICAST_ADDR << std::endl;

//     close(sockfd);

//     return 0;
// }

