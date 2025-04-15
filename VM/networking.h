#ifndef __NETWORKING__H
#define __NETWORKING__H
#include <time.h>
#include <string>
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

namespace PiELo {
    class VM;
    class Variable;
    typedef timeval timestamp_t;
    class Networking {
        int socketfd;
        addrinfo *routerinfo;
        VM* vm;
        public:


        int bindEphemeralPort(int sockfd);

        // Setup sockets and things. Returns 0 on success
        int initNetworking(VM* vmPtr);

        // Clean up stored networking info
        void cleanupNetworking(void);

        // Broadcast a variable data
        timestamp_t broadcastVariable(std::string name, Variable v);

        // Check for a message and update a variable or rebroadcast own value if necessary
        bool checkForMessage(void);
    };
}
#endif