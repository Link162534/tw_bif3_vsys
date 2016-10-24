#include "ClientDummy.h"
#include "Server.h"

ClientDummy::ClientDummy(Server * server, struct sockaddr_in clientAddress, int clientSocket) {
    this->clientAddress = clientAddress;
    this->clientSocket = clientSocket;
    this->server = server;
}

ClientDummy::~ClientDummy() {
}

int ClientDummy::start() {
    running = true;
    if (clientSocket > 0) {
        printf("Client connected from %s:%ho...\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
        strcpy(buffer, "Welcome to myserver, Please enter your command:\n");
        send(clientSocket, buffer, strlen(buffer), 0);
    }
    server->clearBuffer(buffer);
    while (running) {
        size = recv(clientSocket, buffer, BUF - 1, 0);
        if (size == 0) {
            printf("Client closed remote socket\n");
            running = false;
            break;
        } else if (size < 0) {
            perror("Receive error, disconnected client.");
            running = false;
            break;
        }

        if (buffer[0] == 1 || strncmp(buffer, "list", 4) == 0) {
            printf("Client: list");
            buffer[0] = 5;
            //server->getFileListString(&buffer[1]);
          //  send(clientSocket, buffer, BUF, 0);
        }
        if (buffer[0] == 2) {
            //  receiveFile(std::atoi(&buffer[strlen(&buffer[2]) + 1]),
            //          &buffer[2]);
        }
        if (buffer[0] == 3) {
            //  sendFile(&buffer[2]);
        }
        if (strncmp(buffer, "quit", 4) == 0)
            running = false;
        server->clearBuffer(buffer);
    }
    close(clientSocket);
}

int ClientDummy::stop() {
    running = false;
    close(clientSocket);
}


/*void sendFile(std::string filename) {
    std::ifstream toSendFile;
        
    //FEHLER WENN FILE NICHT EXISITIERT
    toSendFile.open(downloadFolder + filename);

    int bytesRead; 
    int bytesSent;
    char fullbuffer[BUF + 1];
    char* buffer = fullbuffer + 1;
    int filesize = getFileSize(filename);
    fullbuffer[0] = 2;
    fullbuffer[1] = '\0';
    strcpy(std::to_string(filesize).c_str())
    fullbuffer[2] = std::to_string(filesize).c_str();
     send(clientSocket,);
        
    fullbuffer[0] = 4;
    while ((bytesRead = toSendFile.readsome(buffer, BUF)) > 0) {

        bytesSent = send(clientSocket, fullbuffer, bytesRead, 0);
        if (bytesSent == -1) {
            break;
        }
        recv(clientSocket, &bytesReceived, sizeof (int32_t), 0);
        if (bytesSent - bytesReceived != 0) {
            toSendFile.close();
            return;
        }
    }
    fullbuffer[0] = (int32_t) 5;
    send(clientSocket, fullbuffer, sizeof (int32_t), 0);
    toSendFile.close();
}

void receiveFile(int filelength, std::string filename) {
    std::ofstream receivedFile;
    receivedFile.open(filename);

    int bytesRead; // how many we have left to send
    int bytesSent;
    char fullbuffer[BUF + sizeof (int32_t)];
    char* buffer = fullbuffer + sizeof (int32_t) / sizeof (char*);
    fullbuffer[0] = (int32_t) 4;
    int32_t bytesReceived;

    while ((bytesRead = receivedFile.readsome(buffer, BUF)) > 0) {

        bytesSent = send(clientSocket, fullbuffer, bytesRead, 0);
        if (bytesSent == -1) {
            break;
        }
        recv(clientSocket, &bytesReceived, sizeof (int32_t), 0);
        if (bytesSent - bytesReceived != 0) {
            receivedFile.close();
            return;
        }
    }
    fullbuffer[0] = (int32_t) 5;
    send(clientSocket, fullbuffer, sizeof (int32_t), 0);
    receivedFile.close();
}*/
