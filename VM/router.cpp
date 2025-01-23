#include "networking.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>
#include <algorithm>

static const char *PORT = "5005";
static const int MAXBUFLEN = 512;

int currentID = 0;

// client info
struct ClientInfo
{
    sockaddr_in addr;
    socklen_t addrLen;
};

// check if two sockaddr_in are the same (by IP + port)
bool sameAddress(const sockaddr_in &a, const sockaddr_in &b)
{
    return (a.sin_addr.s_addr == b.sin_addr.s_addr) &&
           (a.sin_port == b.sin_port) &&
           (a.sin_family == b.sin_family);
}

int main()
{
    // resolve address info for binding
    struct addrinfo hints, *servinfo;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP
    hints.ai_flags = AI_PASSIVE;    // binding/listening

    int rv = getaddrinfo(nullptr, PORT, &hints, &servinfo);
    if (rv != 0)
    {
        std::fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // create socket
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (sockfd == -1)
    {
        perror("router: socket");
        return 1;
    }

    // allow reuse of the port
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) == -1)
    {
        perror("router: setsockopt SO_REUSEPORT");
        close(sockfd);
        return 1;
    }

    // bind to the port
    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        perror("router: bind");
        close(sockfd);
        freeaddrinfo(servinfo);
        return 1;
    }
    freeaddrinfo(servinfo);

    std::printf("Router listening on port %s...\n", PORT);

    std::vector<ClientInfo> clients; // We'll store addresses here

    // main receive-forward loop
    while (true)
    {
        struct sockaddr_in theirAddr;
        socklen_t addrLen = sizeof(theirAddr);
        char buf[MAXBUFLEN];

        // block until packet received
        ssize_t numBytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0,
                                    (struct sockaddr *)&theirAddr, &addrLen);
        if (numBytes == -1)
        {
            perror("router: recvfrom");
            continue; // keep trying
        } 
        buf[numBytes] = '\0'; // null-terminate for printing

        // check if client is known
        bool found = false;
        for (auto &client : clients)
        {
            if (sameAddress(client.addr, theirAddr))
            {
                found = true;
                break;
            }
        }

        // if unknown add to list
        if (!found)
        {
            ClientInfo ci;
            ci.addr = theirAddr;
            ci.addrLen = addrLen;
            clients.push_back(ci);

            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &theirAddr.sin_addr, ipStr, sizeof(ipStr));
            std::printf("New client registered: %s:%d, id %d\n",
                        ipStr, ntohs(theirAddr.sin_port), currentID);

            ssize_t sentBytes = sendto(sockfd, &currentID, sizeof(currentID), 0,
                                       (sockaddr*) &theirAddr, sizeof(sockaddr_in));
            if (sentBytes == -1)
            {
                perror("router: sendto");
            }
            currentID++;
            if (numBytes == 0) continue; // This is the init ping
        }

        // print out message
        char senderIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &theirAddr.sin_addr, senderIp, sizeof(senderIp));
        if (numBytes != sizeof(PiELo::Message)) {
            printf("Got incorrectly sized packet from %s:%d. Got %ld bytes.\n", senderIp, ntohs(theirAddr.sin_port), numBytes);
            continue;
        }
        PiELo::Message *msg = (PiELo::Message*) buf;
        std::printf("Received update to variable %s from %s:%d.\n",
                    msg->variableName, senderIp, ntohs(theirAddr.sin_port));

        // forward the message to all clients
        for (auto &client : clients)
        {
            // if (sameAddress(client.addr, theirAddr)) continue;

            ssize_t sentBytes = sendto(sockfd, buf, numBytes, 0,
                                       (struct sockaddr *)&client.addr, client.addrLen);
            if (sentBytes == -1)
            {
                perror("router: sendto");
            }
        }
    }

    close(sockfd);
    return 0;
}
