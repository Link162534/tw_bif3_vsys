#include "ClientDummy.h"
#include "Server.h"

Server* gs;

Server::Server(int port, char* downloadFolder) {
    this->port = port;
    this->downloadFolder = downloadFolder;
    gs = this;
};

Server::~Server() {
    stop();
};

int Server::start() {
    clientListenerSocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&address, 0, sizeof (address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(clientListenerSocket, (struct sockaddr *) &address, sizeof (address)) != 0) {
        std::cerr << "Bind error" << std::endl;
        return EXIT_FAILURE;
    }

    listen(clientListenerSocket, 5);
    addressLength = sizeof (struct sockaddr_in);

    running = true;
    waitForClient();
    return EXIT_SUCCESS;
};

void Server::stop() {
    running = false;
    close(clientListenerSocket);
    for (ClientDummy * c : clientList) {
        c->stop();
    }
};

void* testThread(void *arg) {
    //pthread_detach(pthread_self());
    gs->clientList.back()->start();
}

void Server::waitForClient() {
    std::cout << "Waiting for connections..." << std::endl;
    while (running) {
        struct sockaddr_in clientAddress;
        int clientSocket = accept(clientListenerSocket, (struct sockaddr *) &clientAddress, &addressLength);
        clientList.push_back(new ClientDummy(this, clientAddress, clientSocket));
        //std::thread t(&ClientDummy::start, dummy);
        pthread_t* thread = new pthread_t;
        threadList.push_back(thread);
        pthread_create(thread, NULL, testThread, NULL);
    }
}

void Server::clearPacket(char *packet) {
    for (int i = 0; i < PACKET_SIZE; i++) {

        packet[i] = '\0';
    }
}

bool Server::exists(const char *name) {
    if (FILE * file = fopen(name, "r")) {
        fclose(file);

        return true;
    }
    return false;
}

int Server::getFileSize(std::string filename) {
    FILE *p_file = NULL;
    p_file = fopen(filename.c_str(), "rb");
    fseek(p_file, 0, SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);
    return size;
}