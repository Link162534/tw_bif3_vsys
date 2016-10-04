#include <cstdlib>
#include <iostream>
#include <string>
#include <signal.h>

#include "Client.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
   // signal(SIGINT, intHandler);
    char* ip = argv[1];
    int port = atoi(argv[2]);
    Client* client = new Client(ip, port);
    return 0;
}

