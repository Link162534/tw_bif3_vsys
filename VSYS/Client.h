/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Client.h
 * Author: voki
 *
 * Created on 4. Oktober 2016, 15:53
 */
#ifndef CLIENT_H
#define CLIENT_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#define BUF 1024

class Client {
public:
    Client();
    Client(const Client& orig);
    virtual ~Client();
    
    int openConnection(int ip, int port);
    int closeConnection();
    int send();
private:
    int socketID;
     char buffer[BUF];
     struct sockaddr_in address;
     int size;

};

#endif /* CLIENT_H */

