#include "ClientDummy.h"
#include "Server.h"

Server::Server(int port, char* downloadFolder) {
    this->port = port;
    this->downloadFolder = downloadFolder;
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

void Server::waitForClient() {
    while (running) {

        std::cout << "Waiting for connections..." << std::endl;

                struct sockaddr_in clientAddress;
                int clientSocket = accept(clientListenerSocket, (struct sockaddr *) &clientAddress, &addressLength);
                ClientDummy dummy(this, clientAddress, clientSocket);
                clientList.push_back(&dummy);
                //std::thread t(&ClientDummy::start, dummy);
                dummy.start();
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