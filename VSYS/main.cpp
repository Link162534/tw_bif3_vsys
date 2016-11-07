#include <cstdlib>
#include <iostream>
#include <string>
#include <signal.h>

#include "Client.h"

using namespace std;

/*
 * 
 */
Client* client;

void sigHandle(int signum){
    client->closeConnection();
}


int main(int argc, char** argv) {
//    char name[10] = "1234\0lal"; 
//    std::string newNAme(name);
//    int lol = std::stoi(newNAme);
//    std::cout << lol;
    
     //signal(SIGINT, sighandler);
    client = new Client();
    char* ip = argv[1];
    int port = atoi(argv[2]);
    if( client->openConnection(ip, port) == EXIT_FAILURE){
        return -1;
    }
    client->printMenu();
    client->listenToInput();
    signal(SIGINT, sigHandle);
    client->closeConnection();
    return 0;
}

