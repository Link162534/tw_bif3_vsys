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
using namespace std;

Client::Client() {

}

Client::Client(const Client& orig) {
}

Client::~Client() {
}

int Client::openConnection(char* ip, int port) {
    if ((socketID = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return EXIT_FAILURE;
    }
    memset(&address, 0, sizeof (address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_aton(ip, &address.sin_addr);
    if (connect(socketID, (struct sockaddr *) &address, sizeof (address)) != 0) {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }
    printf("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));


    return EXIT_SUCCESS;

}

void Client::printMenu() {
    std::cout << "Welcome. You have the following options available: \n";
    std::cout << "List \nGet \nPut \nQuit\n";
}

void Client::listenToInput() {
    std::string input, param;
    while (1) {
        std::cin >> input;
        for (int i = 0; i < input.size(); ++i) {
            input[i] = tolower(input[i]);
        }
        switch (input[0]) {
            case 'l':;
            case 'g':;
            case 'p': ;
            case 'q': return;
            case 'h':;
            default:;
        }

    }
}

void Client::closeConnection() {
    close(socketID);
    std::cout << "Connection gets closed...";
    //return EXIT_SUCCESS;
}

void Client::get(std::string name) {

}

void Client::list() {

}

void Client::put(char * filename) {
    std::ifstream toBeSentFile;
    toBeSentFile.open(filename);

    int bytesRead; // how many we have left to send
    int bytesSent;
    char fullbuffer[BUF + sizeof (int32_t)];
    char* buffer = fullbuffer + sizeof (int32_t) / sizeof(char*);
    fullbuffer[0] = (int32_t)4;
    int32_t bytesReceived;

    while ((bytesRead = toBeSentFile.readsome(buffer, BUF)) > 0) {
        
        bytesSent = send(socketID, fullbuffer, bytesRead, 0);
        if (bytesSent == -1) {
            break;
        }
        recv(socketID, &bytesReceived, sizeof (int32_t), 0);
        if (bytesSent - bytesReceived != 0) {
            toBeSentFile.close();
            return;
        }
    }
    fullbuffer[0] = (int32_t)5;
    send(socketID, fullbuffer, sizeof(int32_t), 0);
    toBeSentFile.close();
}





