//
// Created by Moon Ha on 5/13/24.
//

#ifndef OSC2VMIDI_UDPSERVER_CPP
#define OSC2VMIDI_UDPSERVER_CPP

#include <iostream>
#include "UDPServer.h"

UDPServer::UDPServer(char * arg1):serverport(arg1)
{
    printf ("UDPServer start2\n");
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(nullptr, serverport, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    //Getting IP Address
    getIPAddress(ipAddresses);

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != nullptr; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }
        break;
    }
    if (p == nullptr) {
        fprintf(stderr, "listener: failed to bind socket\n");
        exit(2);
    }
    freeaddrinfo(servinfo);
}

UDPServer::~UDPServer(){
    close(sockfd);
}

// Start receiving data
void UDPServer::start() {
    running = true;
    receiveThread = std::thread(&UDPServer::receive, this);
}

// Stop receiving data
void UDPServer::stop() {
    running = false;
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}

// Receive data function
void UDPServer::receive() {
    while (running) {
        addr_len = sizeof their_addr;
        if ((numbytes = (int) recvfrom(sockfd, buf, MAXBUFLEN - 1, 0,
                                       (struct sockaddr *) &their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        buf[numbytes] = '\0';
        if (callback) {
            callback(buf, numbytes);
        }
    }

    close(sockfd);
}

// get sockaddr, IPv4 or IPv6:
void * UDPServer::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return & ((reinterpret_cast<struct sockaddr_in *> (sa))->sin_addr);
    }
    return & ((reinterpret_cast<struct sockaddr_in6 *> (sa))->sin6_addr);
}

void UDPServer::getIPAddress(std::vector<std::string> & localIPAddresses) {
    struct ifaddrs *ifaddr, *ifa;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        int family = ifa->ifa_addr->sa_family;

        // Check if the address is IPv4
        if (family == AF_INET) {
            if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                            host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) {
                //std::cout << "Interface: " << ifa->ifa_name << " Address: " << host << std::endl;
                std::string iface_name(ifa->ifa_name);
                std::string ip_address(host);
                localIPAddresses.push_back(iface_name + " : " + ip_address);
            }
        }
        // Uncomment the following lines if you want to get IPv6 addresses as well.
        /*
        else if (family == AF_INET6) {
            if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6),
                            host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) {
                std::cout << "Interface: " << ifa->ifa_name << " Address: " << host << std::endl;
            }
        }
        */
    }
    freeifaddrs(ifaddr);
}

// Set callback function
void UDPServer::setCallback(std::function<void(char *, int)> callback) {
    this->callback = callback;
}

#endif //OSC2VMIDI_UDPSERVER_CPP