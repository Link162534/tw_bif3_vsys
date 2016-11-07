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
    client = new Client();
    char* ip = argv[1];
    int port = atoi(argv[2]);
    if( client->openConnection(ip, port) == EXIT_FAILURE){
        return -1;
    }
//    if(client->requestLogin() == -1)
    client->printMenu();
    client->listenToInput();
    signal(SIGINT, sigHandle);
    client->closeConnection();
    return 0;
    

}

