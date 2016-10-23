/* 
 * File:   Server.h
 * Author: link
 *
 * Created on October 1, 2016, 2:20 PM
 */

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
#include "ClientDummy.h"

class Server {
public:

    Server(int port) {
        this->port = port;
    };

    virtual ~Server() {
    };

    int start() {
        clientListenerSocket = socket(AF_INET, SOCK_STREAM, 0);
        memset(&address, 0, sizeof (address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(clientListenerSocket, (struct sockaddr *) &address, sizeof (address)) != 0) {
            std::cerr << "bind error \n";
            return EXIT_FAILURE;
        }

        listen(clientListenerSocket, 5);
        addressLength = sizeof (struct sockaddr_in);

        running = true;
        waitForClient();
        return EXIT_SUCCESS;
    };

    void stop() {
        running = false;
        close(clientListenerSocket);
    };

private:
    bool running = false;
    int clientListenerSocket;
    socklen_t addressLength;
    int port;
    struct sockaddr_in address;
    std::vector<ClientDummy> clientList;

    void waitForClient() {
        while (running) {
            printf("Waiting for connections...\n");
            
            struct sockaddr_in clientAddress;
            int clientSocket = accept(clientListenerSocket, (struct sockaddr *) &clientAddress, &addressLength);
            
            ClientDummy dummy(clientAddress, clientSocket);
            clientList.push_back(dummy);
            dummy.start();
        }
    }
};

#endif /* SERVER_H */

