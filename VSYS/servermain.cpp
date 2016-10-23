#include <signal.h>
#include "Server.h"
#include <iostream>

Server* server;
using namespace std;

void onExit(int s) {
    cout << "Stopping server..." << endl;
    server->stop();
    cout << "Server stopped!" << endl;
    delete server;
    exit(0);
}
void addOnExit(){
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = onExit;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}

int main(void) {
    addOnExit();
    server = new Server(7777);
    return server->start();
}

