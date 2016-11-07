
/* 
 * File:   Client.cpp
 * Author: voki
 * 
 */

#include "Client.h"

Client::Client() {
    clearArray(buffer, BUFFER_SIZE);
    usingMessage = "List \nGet <filname> \nPut <filename>\nQuit\n";
}

Client::Client(const Client& orig) {
}

Client::~Client() {
}

int Client::openConnection(char* ip, int port) {
    if ((socketID = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return EXIT_FAILURE;
    }
    memset(&address, 0, sizeof (address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_aton(ip, &address.sin_addr);
    if (connect(socketID, (struct sockaddr *) &address, sizeof (address)) != 0) {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }
    char temp[BUFFER_SIZE];
    recv(socketID, temp, BUFFER_SIZE + 1, 0);
    printf("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));
    clearArray(buffer, BUFFER_SIZE);
    return EXIT_SUCCESS;

}

int Client::requestLogin() {

    while (1) {
        std::string name, password;
        std::cout << "Please enter your data\n";
        std::cout << "Name: ";
        std::cin >> name;
        std::cout << "Passwort: ";

        termios oldt;
        tcgetattr(STDIN_FILENO, &oldt);
        termios newt = oldt;
        newt.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        std::cin >> password;
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        char reply[BUFFER_SIZE + 1];
        buffer[0] = LOGIN;
        send(socketID, buffer, BUFFER_SIZE + 1, 0);
        if (recv(socketID, &reply, BUFFER_SIZE + 1, 0) == -1) {
            std::cout << "An Error occured \n";
            return -1;
        }
        if (reply[0] == RES_FAILURE) {
            std::cout << "Wrong Name or Password\n";
        }
        if (reply[0] == RES_3_TIMES_WRONG) {
            std::cout << "You failed three times to log in. You're banned now.\n";
            return -1;
        }
        if (reply[0] == RES_END) {
            return 0;
        }
    }
    return 0;
}

void Client::printMenu() {
    std::cout << "Welcome. You have the following options available: \n";
    std::cout << usingMessage << "\n";
}

void Client::listenToInput() {
    std::string command, param;
    while (1) {
        std::cout << "Enter command: \n";
        std::cin.clear();
        std::cin >> command;
        //        if (input.find(" ") > -1) {
        //             std::cout << "lol: \n";
        //            command = input.substr(0, input.find(" "));
        //            std::cout << "command. "<<command;
        //            param = input.substr(input.find(" "), input.length());
        //            std::cout << "param. "<<param;
        //        } else {
        //            command = input;
        //        }
        for (int i = 0; i < command.size(); ++i) {
            command[i] = tolower(command[i]);
        }
        if (command == "list") {
            list();
        } else if (command == "get") {
            std::cin >> param;
            get(param);
        } else if (command == "put") {
            std::cin >> param;
            get(param);
        } else if (command == "quit") {
            return;
        } else {
            std::cout << "Command not found, please enter again\n";
        }

    }
}

void Client::closeConnection() {
    close(socketID);
    std::cout << "Connection gets closed...";
    //return EXIT_SUCCESS;
}

bool Client::fileExists(const char* name) {
    if (FILE * file = fopen(name, "r")) {
        fclose(file);

        return true;
    }
    return false;
}

void Client::get(std::string name) {
    char reply[BUFFER_SIZE + 1]; //answere from server
    clearArray(reply, BUFFER_SIZE); //set array all \0
    int remainingFileSize = 0;

    buffer[0] = REQ_GET; //set flag to get 
    std::copy(name.begin(), name.end(), message); //write filename into messagepart of buffer

    send(socketID, buffer, BUFFER_SIZE + 1, 0); //sends get request
    clearArray(buffer, BUFFER_SIZE);
    //Receive Filesize
    if (recv(socketID, &reply, BUFFER_SIZE + 1, 0) == -1 || reply[0] != RES_FILE_SIZE) {//sets remainingFilesize to Filesize and checks error
        std::cout << "Couldn't receive file \n";
        return;
    }
    remainingFileSize = std::atoi((reply + 1)); //

    clearArray(reply, BUFFER_SIZE); //clear reply for next recv
    
    if (fileExists(name.c_str())) {//removes file if exists
        std::remove(name.c_str());
    }
    std::ofstream outfile(name, std::ofstream::out | std::ofstream::binary | std::ofstream::app); //creates file
    int receivedPacketSize = 0;

    while (remainingFileSize > 0) {//while received files aren'T equal to filesize, subtracts received filesize from remaining filesize at end of while

        if ((receivedPacketSize = recv(socketID, &reply, BUFFER_SIZE + 1, 0)) == -1 || reply[0] == RES_FAILURE) {//receive filepart and checks for error
            std::cout << "A transmission error occured\n";
            break;
        }
        //        for (int i = 0; i < BUFFER_SIZE + 1; i++) {
        //            std::cout << reply[i];
        //        }
        if (reply[0] == RES_END) {//breaks if server sends that transmission is finished
            break;
        }
        outfile.write(reply + 1, receivedPacketSize - 1);
        remainingFileSize -= receivedPacketSize - 1;

    }
    if (remainingFileSize != 0) {//if file transfere failed, delete file
        std::remove(name.c_str());
    } else {
        std::cout << "Transmission successful :D \n";
    }

}

void Client::list() {
    char reply[BUFFER_SIZE + 1];
    buffer[0] = REQ_LIST; //set flag to list

    send(socketID, buffer, BUFFER_SIZE + 1, 0);
    clearArray(buffer, BUFFER_SIZE); //clear array for next use

    while (1) {
        if (recv(socketID, &reply, BUFFER_SIZE + 1, 0) == -1) {
            std::cout << "An Error occured \n";
            return;
        }
        //testing purposes
        //        else {
        //            ////////////////////////////////////////
        //            for (int i = 0; i < BUFFER_SIZE; i++) {
        //                std::cout << reply[i];
        //            }
        //            ////////////////////////////////////////
        //        }
        if (reply[0] == RES_END) {//check if transfer finished
            break;
        }
        if (reply[0] == RES_FAILURE) {//check for problems at server
            std::cout << "There was a Problem reading from directory at server. \n";
            return;
        }

        listObject temp; //creating temporary struct 
        temp.size = std::atoi(reply + 1); //add properties
        temp.filename = getSecondStringFromArray(reply);
        fileList.push_back(temp); //add to fileList

        clearArray(reply, BUFFER_SIZE);
    }
    printFileList();
}

void Client::clearArray(char array[], int size) {
    for (int i = 0; i < size; i++) {
        array[i] = '\0';
    }
}

/**
 * puts pointer at arrayposition after the first \0
 * @param array
 * @return pointer at arrayposition after the first \0
 */
char* Client::getSecondStringFromArray(char* array) {
    char* secondString = array;
    for (int i = 0; array[i] != '\0'; i++) {
        secondString++;
    }
    secondString++;
    return secondString;
}

void Client::printFileList() {
    std::list<listObject>::iterator i;
    std::cout << "Ordner enthaelt: \n";
    for (i = fileList.begin(); i != fileList.end(); ++i) {
        std::cout << (*i).filename << " " << (*i).size << " byte\n";
    }
}

void Client::put(char * filename) {
    //    std::ifstream toBeSentFile;
    //    toBeSentFile.open(filename);
    //
    //    int bytesRead; // how many we have left to send
    //    int bytesSent;
    //    char fullbuffer[BUFFER_SIZE + sizeof (int32_t)];
    //    char* buffer = fullbuffer + sizeof (int32_t) / sizeof (char*);
    //    fullbuffer[0] = (int32_t) 4;
    //    int32_t bytesReceived;
    //
    //    while ((bytesRead = toBeSentFile.readsome(buffer, BUFFER_SIZE)) > 0) {
    //
    //        bytesSent = send(socketID, fullbuffer, bytesRead, 0);
    //        if (bytesSent == -1) {
    //            break;
    //        }
    //        recv(socketID, &bytesReceived, sizeof (int32_t), 0);
    //        if (bytesSent - bytesReceived != 0) {
    //            toBeSentFile.close();
    //            return;
    //        }
    //    }
    //    fullbuffer[0] = (int32_t) 5;
    //    send(socketID, fullbuffer, sizeof (int32_t), 0);
    //    toBeSentFile.close();
}





