#include <algorithm>

#include "ClientDummy.h"
#include "Server.h"

ClientDummy::ClientDummy(Server* server, struct sockaddr_in clientAddress, int clientSocket) {
    this->clientAddress = clientAddress;
    this->clientSocket = clientSocket;
    this->server = server;
    packet = new char[PACKET_SIZE];
    header = packet;
    data = packet + 1;
    server->clearPacket(packet);
}

ClientDummy::~ClientDummy() {
    stop();
}

int ClientDummy::start() {
    running = true;
    if (clientSocket > 0) {
        std::cout << "Client connected from "
                << inet_ntoa(clientAddress.sin_addr)
                << ":"
                << ntohs(clientAddress.sin_port)
                << "..."
                << std::endl;
        sendMessage("Verbunden!\n");
    }

    while (running) {
        server->clearPacket(packet);
        size = recv(clientSocket, packet, PACKET_SIZE, 0);
        if (size == 0) {
            std::cout << "Client closed remote socket" << std::endl;
            running = false;
            break;
        } else if (size < 0) {
            std::cerr << "Receive error, disconnected client.";
            running = false;
            break;
        }
        server->clearPacket(packet);
        data[0] = 'a';
        onGet();
        switch ((PACKET_TYPE) * header) {
            case REQ_LIST:
                std::cout << clientSocket << ": " << "list" << std::endl;
                onList();
                break;
            case REQ_GET:
                std::cout << clientSocket << ": " << "get" << std::endl;
                onGet();
                break;
            case REQ_PUT:
                std::cout << clientSocket << ": " << "put" << std::endl;
                onPut();
                break;
        }
    }
    close(clientSocket);
}

int ClientDummy::stop() {
    running = false;
    close(clientSocket);
}

void ClientDummy::onList() {
    using namespace std;
    DIR *dp;
    struct dirent *dirp;

    if ((dp = opendir(server->downloadFolder)) == NULL) {
        cerr << "Error(" << errno << ") opening " << server->downloadFolder << endl;
        sendFail();
        return;
    }
    string folder(server->downloadFolder);
    while ((dirp = readdir(dp)) != NULL) {
        string name(dirp->d_name);
        if (name == "." || name == "..")
            continue;
        sendListPacket(name, server->getFileSize(folder + name));
    }
    closedir(dp);
    sendEnd();
}

void ClientDummy::onGet() {
    std::string file(server->downloadFolder);
    file += data;
    if (!server->exists((file).c_str())) {
        sendFail();
    }
    int filesize = server->getFileSize(file);
    sendFileSize(filesize);

    std::ifstream desiredFile(file, std::ifstream::binary);

    while (desiredFile.good()) {
        desiredFile.read(data, sizeof (char) * BUFFER_SIZE);
        sendFilePart(desiredFile.gcount());
        for (int i = 0; i < BUFFER_SIZE; i++) {
            std::cout << data[i];
        }
        server->clearPacket(packet);
    }

    desiredFile.close();
    sendEnd();
}

void ClientDummy::onPut() {

}

void ClientDummy::sendListPacket(std::string& filename, int filesize) {
    server->clearPacket(packet);
    (*header) = RES_LIST_ANSWER;
    std::string str = std::to_string(filesize) + '\0' + filename;
    std::copy(str.begin(), str.end(), data);
    send(clientSocket, packet, PACKET_SIZE, 0);
}

void ClientDummy::sendFileSize(int& filesize) {
    server->clearPacket(packet);
    (*header) = RES_FILE_SIZE;

    std::string str = std::to_string(filesize) + '\0';
    std::copy(str.begin(), str.end(), data);
    send(clientSocket, packet, PACKET_SIZE, 0);
}

void ClientDummy::sendFilePart(int size) {
    (*header) = RES_FILE_PART;
    send(clientSocket, packet, size + 1, 0);
}

void ClientDummy::sendEnd() {
    server->clearPacket(packet);
    (*header) = END;
    send(clientSocket, packet, 1, 0);
}

void ClientDummy::sendFail() {
    server->clearPacket(packet);
    (*header) = FAILURE;
    send(clientSocket, packet, 1, 0);
}

void ClientDummy::sendMessage(char* message) {
    strcpy(data, message);
    (*header) = MESSAGE;
    send(clientSocket, packet, PACKET_SIZE, 0);
}