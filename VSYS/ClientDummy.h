/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ClientDummy.h
 * Author: link
 *
 * Created on October 4, 2016, 4:56 PM
 */

#ifndef CLIENTDUMMY_H
#define CLIENTDUMMY_H

#include <cstring>
#include <string>
#include <cstdio>
#define BUF 1024 

class ClientDummy {
public:

    ClientDummy(struct sockaddr_in clientAddress, int clientSocket) {
        this->clientAddress = clientAddress;
        this->clientSocket = clientSocket;
    }

    virtual ~ClientDummy() {
    }

    int start() {
        running = true;
        if (clientSocket > 0) {
            printf("Client connected from %s:%d...\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
            strcpy(messageBuffer, "Welcome to myserver, Please enter your command:\n");
            send(clientSocket, messageBuffer, strlen(messageBuffer), 0);
        }
        while (running) {
            size = recv(clientSocket, messageBuffer, BUF - 1, 0);
            if (size > 0) {
                messageBuffer[size] = '\0';
                printf("Message received: %s\n", messageBuffer);
            } else if (size == 0) {
                printf("Client closed remote socket\n");
                break;
            } else {
                perror("recv error");
                return EXIT_FAILURE;
            }
            if (strncmp(messageBuffer, "quit", 4) != 0)
                running = false;
        }
    }

    int stop() {
        running = false;
        close(clientSocket);
    }

private:
    bool running = false;
    int clientSocket;
    int size;
    char messageBuffer[BUF];
    struct sockaddr_in clientAddress;
};

#endif /* CLIENTDUMMY_H */

