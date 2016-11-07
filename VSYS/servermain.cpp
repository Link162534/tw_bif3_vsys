#include <signal.h>
#include "Server.h"
#include <iostream>

Server* server;
using namespace std;

bool test = true;

void onExit(int s) {
    cout << "Stopping server..." << endl;
    server->stop();
    cout << "Server stopped!" << endl;
    delete server;
    exit(0);
}

void addOnExit() {
    signal(SIGINT, onExit);
    signal(SIGABRT, onExit);
    signal(SIGFPE, onExit);
    signal(SIGILL, onExit);
    signal(SIGSEGV, onExit);
    signal(SIGTERM, onExit);
}

int main(int argc, char *argv[]) {
    addOnExit();
    if (!test) {
        if (argc != 3) {
            cout << "Not enough parameter! Needs port and downloadfolder";
            return EXIT_FAILURE;
        }
        server = new Server(atoi(argv[1]), argv[2]);
    } else
        server = new Server(DEFAULT_PORT, DEFAULT_FOLDER);

    server->start();
    server->stop();
}

