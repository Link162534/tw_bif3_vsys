#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <thread>
#include <dirent.h>
#include <errno.h>

#include "Protocol.h"

class ClientDummy;

class Server {
public:

    Server(int port, char* downloadFolder);

    virtual ~Server();

    int start();
    void stop();

    void clearPacket(char* packet);
    int getFileSize(std::string filename);
    bool exists(const char *name);
    char * downloadFolder;
    std::vector<pthread_t *> threadList;
    std::vector<ClientDummy *> clientList;


private:
    bool running = false;
    int clientListenerSocket;
    socklen_t addressLength;
    int port;
    struct sockaddr_in address;

    void waitForClient();
    std::string getDir(std::string dir);

};

#endif /* SERVER_H */

