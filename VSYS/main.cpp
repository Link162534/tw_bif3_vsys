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
     //signal(SIGINT, sighandler);
    client = new Client();
    char* ip = argv[1];
        client->printMenu();

    int port = atoi(argv[2]);
    client->openConnection(ip, port);
    signal(SIGINT, sigHandle);
    return 0;
}

