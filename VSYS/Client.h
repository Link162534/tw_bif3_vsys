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
#include <cstdio>
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <algorithm>
#include <fstream>
#define BUF 1024

class Client {
public:
    Client();
    Client(const Client& orig);
    virtual ~Client();    
    static void sighandler(int signum);
    int openConnection(char* ip, int port);
    void closeConnection();
    void printMenu();
    void onExit();
    void listenToInput();
private:
    char* ip;
    int port;
    int socketID;
    char buffer[BUF];
    struct sockaddr_in address;
    int size;
    void list();
    void get(std::string name);
    void put(char * filename);//param?
};

#endif /* CLIENT_H */

