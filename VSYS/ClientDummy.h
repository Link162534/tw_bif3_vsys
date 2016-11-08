#ifndef CLIENTDUMMY_H
#define CLIENTDUMMY_H

#include <cstring>
#include <string>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <arpa/inet.h>
#include "Protocol.h"

class Server;
class ClientDummy {
public:

    ClientDummy(Server* server, struct sockaddr_in clientAddress, int clientSocket);

    virtual ~ClientDummy();
    
    int start();

    int stop();

private:
    bool running = false;
    int clientSocket;
    int size;
    char* packet;
    char* header;
    char* data;
    char* downloadFolder;
    struct sockaddr_in clientAddress;
    Server* server;
    
    void onList();
    void onGet();
    void onPut();
    void sendListPacket(std::string &filename, long filesize);
    void sendFileSize(long &filesize);
    void sendFilePart(int size);
    void sendEnd();
    void sendFail();
    void sendMessage(char *message);
    
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

};

#endif /* CLIENTDUMMY_H */