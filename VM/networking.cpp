#include "networking.h"
#include <sys/time.h>
#include "robotFunctions.h"

#define ROUTER_HOST "localhost"
#define ROUTER_PORT "5005"
namespace PiELo {
    static const int MAXBUFLEN = 512;
    int socketfd;
    addrinfo *routerinfo;

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

    int initNetworking(void) {

        // 1) Resolve router address
        addrinfo hints;
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;

        int rv = getaddrinfo(ROUTER_HOST, ROUTER_PORT, &hints, &routerinfo);
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

        socketfd = sockfd;
        return 0;
    }

    void cleanupNetworking(void) {
        freeaddrinfo(routerinfo);
        close(socketfd);
    }

    // Broadcast a variable data
    timestamp_t broadcastVariable(std::string name, VariableData d) {
        timestamp_t currentTime;

        Message msg;
        gettimeofday(&msg.variableLastUpdated, NULL);
        strncpy(msg.variableName, name.c_str(), 100);
        msg.robotID = 0;
        msg.senderX = robot.getRobotPos().x;
        msg.senderY = robot.getRobotPos().y;
        msg.senderZ = robot.getRobotPos().z;
        msg.data = d;

        ssize_t sentBytes = sendto(socketfd, (void*) &msg, sizeof(msg), 0, routerinfo->ai_addr, routerinfo->ai_addrlen);
        std::cout << "Sent " << sentBytes << " bytes update for variable " << name << " to router." << std::endl;
        return currentTime;
    }
}