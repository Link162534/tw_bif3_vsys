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
#include "Protocol.h"
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
#include <list>

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
    char* flag = buffer;
    char* message = buffer + 1;
    char* ip;
    int port;
    int socketID;
    char buffer[BUFFER_SIZE+1];
    struct sockaddr_in address;
    int size;
    void list();
    void get(std::string name);
    void put(char * filename);//param?
    struct listObject{std::string filename; int size; };
    std::list<listObject> fileList;
    char* getSecondStringFromArray(char * array);
    void printFileList();
};

#endif /* CLIENT_H */

