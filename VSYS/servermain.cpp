#include <signal.h>
#include "Server.h"
#include <iostream>

Server* server;
using namespace std;

void onExit(int s) {
    cout << "Stopping server..." << endl;
    server->stop();
    cout << "Server stopped!" << endl;
    server->stop();
    delete server;
    exit(0);
}

void addOnExit() {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = onExit;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Not enough parameter! Needs port and downloadfolder";
        return EXIT_FAILURE;
    }
    addOnExit();
    server = new Server(atoi(argv[1]), argv[2]);
    return server->start();
}

