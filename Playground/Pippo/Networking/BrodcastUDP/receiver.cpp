#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <string>

#define MULTICAST_ADDR "239.255.0.1"
#define MULTICAST_PORT 4950
#define MAX_MSG_LEN 1024

struct Device {
    std::string id;
    std::string last_seen;
};

int main() {
    int sockfd;
    struct sockaddr_in addr;
    struct ip_mreq mreq;
    char buffer[MAX_MSG_LEN];
    Device paired_device;
    bool paired = false; // Indicates if this receiver is already paired with a sender

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

    // Receive messages and register devices
    while (true) {
        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
        if (n < 0) {
            perror("recvfrom");
            return 1;
        }
        buffer[n] = '\0';

        std::string message(buffer);
        std::cout << "Received message: " << message << std::endl;

        // Parse the sender ID from the registration message
        size_t pos = message.find("from ");
        if (pos != std::string::npos) {
            std::string sender_id = message.substr(pos + 5);

            // Pair with the first sender and ignore subsequent ones
            if (!paired) {
                paired = true;
                paired_device.id = sender_id;
                paired_device.last_seen = message;
                std::cout << "Paired with sender: " << sender_id << std::endl;

                // Send acknowledgment to the sender
                struct sockaddr_in sender_addr;
                sender_addr.sin_family = AF_INET;
                sender_addr.sin_port = htons(MULTICAST_PORT);
                sender_addr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);

                std::string reply = "I'm the receiver for Node: " + sender_id;
                sendto(sockfd, reply.c_str(), reply.length(), 0, (struct sockaddr *)&sender_addr, sizeof(sender_addr));
            } else {
                std::cout << "Already paired with sender: " << paired_device.id << ", ignoring other registrations." << std::endl;
            }
        }
    }

    close(sockfd);
    return 0;
}


// #include <iostream>
// #include <cstring>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <unistd.h>
// #include <vector>
// #include <string>

// #define MULTICAST_ADDR "239.255.0.1"
// #define MULTICAST_PORT 4950
// #define MAX_MSG_LEN 1024

// struct Device {
//     std::string id;
//     std::string last_seen;
//     struct sockaddr_in sender_addr;  // Store sender's address
// };

// int main() {
//     int sockfd;
//     struct sockaddr_in addr;
//     struct ip_mreq mreq;
//     char buffer[MAX_MSG_LEN];
//     std::vector<Device> devices; // List to store registered devices

//     // Create the socket
//     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sockfd < 0) {
//         perror("socket");
//         return 1;
//     }

//     // Allow multiple sockets to use the same PORT number
//     int reuse = 1;
//     if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
//         perror("setsockopt (SO_REUSEADDR)");
//         return 1;
//     }

//     // Bind to the multicast port
//     memset(&addr, 0, sizeof(addr));
//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all interfaces
//     addr.sin_port = htons(MULTICAST_PORT);

//     if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
//         perror("bind");
//         return 1;
//     }

//     // Join the multicast group
//     mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
//     mreq.imr_interface.s_addr = htonl(INADDR_ANY);
//     if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
//         perror("setsockopt (IP_ADD_MEMBERSHIP)");
//         return 1;
//     }

//     std::cout << "Listening for multicast messages on " << MULTICAST_ADDR << ":" << MULTICAST_PORT << std::endl;

//     // Receive messages and register devices
//     while (true) {
//         int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
//         if (n < 0) {
//             perror("recvfrom");
//             return 1;
//         }
//         buffer[n] = '\0';

//         std::string message(buffer);
//         std::cout << "Received message: " << message << std::endl;

//         // Parse the sender ID from the message
//         size_t pos = message.find("from ");
//         if (pos != std::string::npos) {
//             std::string sender_id = message.substr(pos + 5);

//             // Register the sender in the device list if not already registered
//             bool found = false;
//             for (auto& device : devices) {
//                 if (device.id == sender_id) {
//                     found = true;
//                     device.last_seen = message;  // Update the last seen time
//                     break;
//                 }
//             }

//             if (!found) {
//                 Device new_device;
//                 new_device.id = sender_id;
//                 new_device.last_seen = message;
//                 devices.push_back(new_device);
//                 std::cout << "Registered new node: " << sender_id << std::endl;
//             }

//             // Send a unicast reply back to the specific sender
//             struct sockaddr_in sender_addr;
//             sender_addr.sin_family = AF_INET;
//             sender_addr.sin_port = htons(MULTICAST_PORT);
//             sender_addr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);

//             // Find the correct device and reply
//             for (auto& device : devices) {
//                 if (device.id == sender_id) {
//                     std::string reply = "I'm the receiver for Node: " + sender_id;
//                     sendto(sockfd, reply.c_str(), reply.length(), 0, (struct sockaddr*)&sender_addr, sizeof(sender_addr));
//                     break;
//                 }
//             }
//         }
//     }

//     close(sockfd);
//     return 0;
// }




// #include <iostream>
// #include <cstring>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <unistd.h>
// #include <vector>
// #include <string>

// #define MULTICAST_ADDR "239.255.0.1"
// #define MULTICAST_PORT 4950
// #define MAX_MSG_LEN 1024

// struct Node {
//     std::string id;
//     std::string last_seen;
// };

// int main() {
//     int sockfd;
//     struct sockaddr_in addr;
//     struct ip_mreq mreq;
//     char buffer[MAX_MSG_LEN];
//     std::vector<Node> nodes; // List to store registered devices

//     // Create the socket
//     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sockfd < 0) {
//         perror("socket");
//         return 1;
//     }

//     // Allow multiple sockets to use the same PORT number
//     int reuse = 1;
//     if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
//         perror("setsockopt (SO_REUSEADDR)");
//         return 1;
//     }

//     // Bind to the multicast port
//     memset(&addr, 0, sizeof(addr));
//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all interfaces
//     addr.sin_port = htons(MULTICAST_PORT);

//     if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
//         perror("bind");
//         return 1;
//     }

//     // Join the multicast group
//     mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
//     mreq.imr_interface.s_addr = htonl(INADDR_ANY);
//     if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
//         perror("setsockopt (IP_ADD_MEMBERSHIP)");
//         return 1;
//     }

//     std::cout << "Listening for multicast messages on " << MULTICAST_ADDR << ":" << MULTICAST_PORT << std::endl;

//     // Receive messages and register devices
//     while (true) {
//         int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
//         if (n < 0) {
//             perror("recvfrom");
//             return 1;
//         }
//         buffer[n] = '\0';

//         std::string message(buffer);
//         std::cout << "Received message: " << message << std::endl;

//         // Parse the sender ID from the message
//         size_t pos = message.find("from ");
//         if (pos != std::string::npos) {
//             std::string sender_id = message.substr(pos + 5);

//             // Register the sender in the device list if not already registered
//             bool found = false;
//             for (auto& node : nodes) {
//                 if (node.id == sender_id) {
//                     found = true;
//                     node.last_seen = message;  // Update the last seen time
//                     break;
//                 }
//             }

//             if (!found) {
//                 nodes.push_back(Node{sender_id, message});
//                 std::cout << "Registered new device: " << sender_id << std::endl;
//             }
//         }

//         // Optionally, print the list of registered devices
//         std::cout << "Registered devices:\n";
//         for (const auto& node : nodes) {
//             std::cout << "ID: " << node.id << ", Last seen: " << node.last_seen << std::endl;
//         }
//     }

//     close(sockfd);
//     return 0;
// }