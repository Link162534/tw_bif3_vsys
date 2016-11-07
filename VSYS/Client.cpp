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
    std::string input, command, param;
    while (1) {
        std::cin >> input;
        if (input.find(" ") != -1) {
            command = input.substr(0, input.find(" "));
            param = input.substr(input.find(" "), input.length());
        } else {
            command = input;
        }
        for (int i = 0; i < command.size(); ++i) {
            command[i] = tolower(command[i]);
        }//zu ifs umbauen
        if (command == "list") {
            list();
        } else if (command == "get") {
            std::cout << "TEst GET" << std::endl;
        } else if (command == "put") {

        } else if (command == "quit") {
            return;
        } else {
            std::cout << "Command not found, please enter again" << std::endl;
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
//char name[5] = "1234"; 
//    std::string newNAme(name);
//    int lol = std::stoi(newNAme);
//    std::cout << lol;

void Client::list() {
    char reply[BUFFER_SIZE + 1];
    buffer[0] = 1;
    send(socketID, buffer, BUFFER_SIZE + 1, 0);
    while (reply[0] != 4) {
        if (recv(socketID, &reply, BUFFER_SIZE + 1, 0) == -1) {
            std::cout << "An Error occured " << std::endl;
        };
        message = "123\0blablaname";
        listObject temp;
        temp.size = std::stoi(message);
        temp.filename = getSecondStringFromArray(message);
        fileList.push_back(temp);
    }
    printFileList();
}

char* Client::getSecondStringFromArray(char* array) {
    char* secondString = array;
    for (int i = 0; array[i] != '\0'; i++) {
        secondString++;
    }
    secondString++;
    return secondString;
}

void Client::printFileList() {
    std::list<listObject>::iterator i;
    std::cout << "Ordner enthaelt:" << std::endl;
    for (i = fileList.begin(); i != fileList.end(); ++i) {
        std::cout << (*i).filename << " " << (*i).size << " byte" << std::endl;
    }
}

void Client::put(char * filename) {
    std::ifstream toBeSentFile;
    toBeSentFile.open(filename);

    int bytesRead; // how many we have left to send
    int bytesSent;
    char fullbuffer[BUFFER_SIZE + sizeof (int32_t)];
    char* buffer = fullbuffer + sizeof (int32_t) / sizeof (char*);
    fullbuffer[0] = (int32_t) 4;
    int32_t bytesReceived;

    while ((bytesRead = toBeSentFile.readsome(buffer, BUFFER_SIZE)) > 0) {

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
    fullbuffer[0] = (int32_t) 5;
    send(socketID, fullbuffer, sizeof (int32_t), 0);
    toBeSentFile.close();
}





