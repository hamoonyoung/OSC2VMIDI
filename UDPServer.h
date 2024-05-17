//
// Created by Moon Ha on 5/13/24.
//

#ifndef OSC2VMIDI_UDPSERVER_H
#define OSC2VMIDI_UDPSERVER_H

#include <cstdio>       //<stdio.h>
#include <cstdlib>      //<stdlib.h>
#include <unistd.h>
#include <cerrno>       //<errno.h>
#include <cstring>      //<string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <chrono>
#include <vector>
#include <ifaddrs.h>

#define MAXBUFLEN 1024

class UDPServer {
public:
    UDPServer(char * arg1);
    ~UDPServer();
    void start();
    void stop();
    void receive();
    static void getIPAddress(std::vector<std::string> &);
    static void printIPAddress();
    // get sockaddr, IPv4 or IPv6:
    static void * get_in_addr(struct sockaddr *sa);

    std::vector<std::string> ipAddresses;
    std::thread receiveThread;
    std::function<void(char *, int)> callback;
    void setCallback(std::function<void(char *, int)> callback);

private:
    char * serverport;
    bool running;
    int sockfd = 0;
    struct addrinfo hints{}, *servinfo{}, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr{};
    char buf[MAXBUFLEN]{};
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN]{};

};

#endif //OSC2VMIDI_UDPSERVER_H
