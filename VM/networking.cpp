#include "networking.h"
#include <sys/time.h>
#include "robotFunctions.h"
#include "vm.h"
#include "instructions/storeLoad.h"

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
        std::cout << "Pinging router for ID" << std::endl;
        sendto(socketfd, "", 0, 0, routerinfo->ai_addr, routerinfo->ai_addrlen);
        sockaddr_in fromAddr;
        socklen_t fromLen = sizeof(fromAddr);
        ssize_t numBytes = recvfrom(socketfd, &robotID, sizeof(robotID), 0,
                                    (sockaddr *)&fromAddr,
                                    &fromLen);
        if (numBytes == -1)
        {   
            perror("client: recvfrom");
            exit(-1);
        }
        std::cout << "My ID is " << robotID << std::endl;


        std::cout << "Waiting for start signal." << std::endl;
        numBytes = recvfrom(socketfd, &robotID, sizeof(robotID), 0,
                                        (sockaddr *)&fromAddr,
                                        &fromLen);
        if (numBytes == -1)
        {   
            perror("client: recvfrom");
            exit(-1);
        }
        std::cout << "Going!" << std::endl;

        // Pause for 10 microseconds when checking for messages
        // Thanks to https://stackoverflow.com/questions/15941005/making-recvfrom-function-non-blocking
        timeval read_timeout;
        read_timeout.tv_sec = 0;
        read_timeout.tv_usec = 10;
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout)) != 0) {
            perror("setsockopt receive timeout");
            return 1;
        }
        return 0;
    }

    void cleanupNetworking(void) {
        freeaddrinfo(routerinfo);
        close(socketfd);
    }

    // Broadcast a variable data
    timestamp_t broadcastVariable(std::string name, Variable v) {
        timestamp_t currentTime;

        VariableData data = v.getVariableData();

        // Get the cached value if it's a closure that updated
        if (v.getType() == Type::PIELO_CLOSURE) {
            data = closureList[v.getClosureIndex()].cachedValue;
        }

        Message msg;
        gettimeofday(&msg.variableLastUpdated, NULL);
        strncpy(msg.variableName, name.c_str(), 100);
        msg.robotID = robotID;
        msg.senderX = robot.getRobotPos().x;
        msg.senderY = robot.getRobotPos().y;
        msg.senderZ = robot.getRobotPos().z;
        msg.data = data;
        msg.isStigmergy = v.isStigmergy;

        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(routerinfo->ai_family, &routerinfo->ai_addr, ipStr, sizeof(ipStr));
        in_port_t port;
        if (routerinfo->ai_family == AF_INET) port = ((sockaddr_in*) routerinfo->ai_addr)->sin_port;
        else port = ((sockaddr_in6*) routerinfo->ai_addr)->sin6_port;

        ssize_t sentBytes = sendto(socketfd, (void*) &msg, sizeof(msg), 0, routerinfo->ai_addr, routerinfo->ai_addrlen);
        std::cout << "Sent " << sentBytes << " bytes update for variable " << name << " to router at " << ipStr <<":"<< port << " family" << routerinfo->ai_family << std::endl;
        return currentTime;
    }

    // Check for a message and update a variable or rebroadcast own value if necessary
    void checkForMessage(void) {
        // wait to receive something from the router (blocking!!)
        Message msg;
        sockaddr_in fromAddr;
        socklen_t fromLen = sizeof(fromAddr);

        ssize_t numBytes = recvfrom(socketfd, &msg, sizeof(Message), 0,
                                    (sockaddr *)&fromAddr,
                                    &fromLen);
        if (numBytes == -1)
        {   
            // errno set to EAGAIN means no data was available.
            if (errno == EAGAIN) return;
            perror("client: recvfrom");
            exit(-1);
        } else if (numBytes != sizeof(Message)) {
            printf("Only received %ld bytes.\n", numBytes);
        }

        // print out message
        char senderIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &fromAddr.sin_addr, senderIp, sizeof(senderIp));
        std::cout << "Received update to variable " << msg.variableName << " from " << senderIp << ":" << ntohs(fromAddr.sin_port);
        // if (std::string(msg.variableName) == "wait") {
            // std::cout << " value " << msg.data.asClosureIndex;
        // }
        try {
            Variable *var = &taggedTable.at(msg.variableName);
            if (!var->isStigmergy) {
                // If the message has a newer timestamp than the variable
                if (var->lastUpdated.tv_sec < msg.variableLastUpdated.tv_sec ||
                    (var->lastUpdated.tv_sec == msg.variableLastUpdated.tv_sec && var->lastUpdated.tv_usec < msg.variableLastUpdated.tv_usec)) {
                    if (var->getType() == PIELO_CLOSURE) {
                        closureList[var->getClosureIndex()].cachedValue = msg.data;
                    } else {
                        var->mutateValue(msg.data);
                    }
                    var->lastUpdated = msg.variableLastUpdated;
                    std::cout << ". Local version was out of date. Updated." << std::endl;
                } else {
                    std::cout << ". Local version is newer. Brooadcasting update." << std::endl;
                    broadcastVariable(msg.variableName, var->getVariableData());
                }
            } else {
                var->updateStigValue(msg.robotID, msg.data);
                std::cout << ". Updated stigmergy for ID " << msg.robotID << std::endl;
            }
            handleDependants(*var);
        } catch(...) {
            std::cout << ". Local version did not exist." << std::endl;
            exit(-1);
        }
    }
}