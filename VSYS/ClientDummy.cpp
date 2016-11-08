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
        std::cout << clientSocket << ": " <<  "Client connected from "
                << inet_ntoa(clientAddress.sin_addr)
                << ":"
                << ntohs(clientAddress.sin_port)
                << "..."
                << std::endl;
        sendMessage("Connected!\n");
    }

    while (running) {
        server->clearPacket(packet);
        size = recv(clientSocket, packet, PACKET_SIZE, 0);

        if (size == 0) {
            std::cout << clientSocket <<": closed connection." << std::endl;
            running = false;
            break;
        } else if (size < 0) {
            std::cerr << clientSocket <<": crashed, connection closed.";
            running = false;
            break;
        }
        switch ((PACKET_TYPE) * header) {
            case REQ_LIST:
                std::cout << clientSocket << ": " << "requested list." << std::endl;
                onList();
                break;
            case REQ_GET:
                std::cout << clientSocket << ": " << "requested get." << std::endl;
                onGet();
                break;
            case REQ_PUT:
                std::cout << clientSocket << ": " << "requested put." << std::endl;
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
        cerr << clientSocket << ": " <<  "Error(" << errno << ") opening " << server->downloadFolder << endl;
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
    long filesize = server->getFileSize(file);
    sendFileSize(filesize);

    std::ifstream desiredFile(file, std::ifstream::binary);

    while (desiredFile.good()) {
        desiredFile.read(data, BUFFER_SIZE);
        sendFilePart(desiredFile.gcount());
        server->clearPacket(packet);
    }

    desiredFile.close();
    sendEnd();
}

void ClientDummy::onPut() {
    std::string name(server->downloadFolder);
    name += data;
    server->clearPacket(packet);

    long remainingFileSize = 0;
    if (recv(clientSocket, packet, PACKET_SIZE, 0) == -1 || (*header) != RES_FILE_SIZE) {
        std::cout << clientSocket << ": " <<  "Couldn't receive file \n";
        return;
    }
    remainingFileSize = std::atol(data);
    server->clearPacket(packet);

    if (server->exists(name.c_str())) {
        std::remove(name.c_str());
    }
    std::ofstream outfile(name, std::ofstream::out | std::ofstream::binary | std::ofstream::app); //creates file
    int receivedPacketSize = 0;

    while (remainingFileSize > 0) {
        if ((receivedPacketSize = recv(clientSocket, packet, PACKET_SIZE, 0)) == -1 || (*header) == FAILURE) {
            std::cout << clientSocket << ": " <<  "A transmission error occured\n";
            break;
        }
        if ((*header) == END) {
            break;
        }
        if (receivedPacketSize < PACKET_SIZE)// removes last byte of the last packet
            receivedPacketSize--;
        outfile.write(data, receivedPacketSize - 1);
        remainingFileSize -= receivedPacketSize - 1;
    }
    if (remainingFileSize != 0) {
        std::cout << clientSocket << ": " <<  "File not sucessfully received\n";
        std::remove(name.c_str());
        sendFail();
    } else {
        std::cout << clientSocket << ": " <<  "Transmission successful\n";
        sendEnd();
    }
}

void ClientDummy::sendListPacket(std::string& filename, long filesize) {
    server->clearPacket(packet);
    (*header) = RES_LIST_ANSWER;
    std::string str = std::to_string(filesize) + '\0' + filename;
    std::copy(str.begin(), str.end(), data);
    send(clientSocket, packet, PACKET_SIZE, 0);
}

void ClientDummy::sendFileSize(long& filesize) {
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