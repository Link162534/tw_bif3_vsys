/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Client.cpp
 * Author: voki
 * 
 * Created on 4. Oktober 2016, 15:53
 */

#include "Client.h"

Client::Client() {
}

Client::Client(const Client& orig) {
}

Client::~Client() {
}

int Client::openConnection(int ip, int port) {
    if((socketID = socket(AF_INET, SOCK_STREAM, 0)) == -1){
       return EXIT_FAILURE;
    }
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
  address.sin_port = htons (port);
  inet_aton (ip, &address.sin_addr);
    
}

