
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
            std::cout << "Connection failed \n";
            return -1;
        }
        if (reply[0] == FAILURE) {
            std::cout << "Wrong Name or Password\n";
        }
        if (reply[0] == BANNED) {
            std::cout << "You failed three times to log in. You're banned now.\n";
            return -1;
        }
        if (reply[0] == END) {
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
        std::cout << "\nEnter command: \n";
        std::cin.clear();
        std::cin >> command;

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
            put(param);
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
    long remainingFileSize = 0;

    buffer[0] = REQ_GET; //set flag to get 
    std::copy(name.begin(), name.end(), message); //write filename into messagepart of buffer

    send(socketID, buffer, BUFFER_SIZE + 1, 0); //sends get request
    clearArray(buffer, BUFFER_SIZE);
    //Receive Filesize
    if (recv(socketID, &reply, BUFFER_SIZE + 1, 0) == -1 || reply[0] != RES_FILE_SIZE) {//sets remainingFilesize to Filesize and checks error
        std::cout << "Couldn't receive file \n";
        return;
    }
    remainingFileSize = std::atol((reply + 1)); //

    clearArray(reply, BUFFER_SIZE); //clear reply for next recv

    if (fileExists(name.c_str())) {//removes file if exists
        std::remove(name.c_str());
    }
    std::ofstream outfile(name, std::ofstream::out | std::ofstream::binary | std::ofstream::app); //creates file
    int receivedPacketSize = 0;

    while (remainingFileSize > 0) {//while received files aren'T equal to filesize, subtracts received filesize from remaining filesize at end of while
        if ((receivedPacketSize = recv(socketID, reply, BUFFER_SIZE + 1, 0)) == -1 || reply[0] == FAILURE) {//receive filepart and checks for error
            std::cout << "A transmission error occured\n";
            break;
        }
        //        for (int i = 0; i < BUFFER_SIZE + 1; i++) {
        //            std::cout << reply[i];
        //        }
        if (reply[0] == END) {//breaks if server sends that transmission is finished
            break;
        }

        if (receivedPacketSize < BUFFER_SIZE + 1)// removes last byte of the last packet
            receivedPacketSize--;

        outfile.write(reply + 1, receivedPacketSize - 1);
        remainingFileSize -= receivedPacketSize - 1;
    }
    if (remainingFileSize != 0) {//if file transfere failed, delete file
        std::remove(name.c_str());
        std::cout << "A transmission error occured\n";
    } else {
        std::cout << "Transmission successful\n";
    }

}

void Client::list() {
    char reply[BUFFER_SIZE + 1];
    buffer[0] = REQ_LIST; //set flag to list

    send(socketID, buffer, BUFFER_SIZE + 1, 0);
    clearArray(buffer, BUFFER_SIZE); //clear array for next use

    while (1) {
        if (recv(socketID, &reply, BUFFER_SIZE + 1, 0) == -1) {
            std::cout << "Connection failed \n";
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
        if (reply[0] == END) {//check if transfer finished
            break;
        }
        if (reply[0] == FAILURE) {//check for problems at server
            std::cout << "There was a Problem reading from directory at server. \n";
            return;
        }

        listObject temp; //creating temporary struct 
        temp.size = std::atol(reply + 1); //add properties
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
    std::cout << "Directory contains: \n";
    for (i = fileList.begin(); i != fileList.end(); ++i) {
        std::cout << (*i).filename << " " << (*i).size << " byte\n";
    }
}

void Client::put(std::string name) {
    if (fileExists(name.c_str()) == false) {
        std::cout << "The file does not exist\n";
        return;
    }
    buffer[0] = REQ_PUT; //set flag to put
    std::copy(name.begin(), name.end(), message); //write filename into messagepart of buffer
    send(socketID, buffer, BUFFER_SIZE + 1, 0); //sends put request
    clearArray(buffer, BUFFER_SIZE);
    long filesize = getFileSize(name);
    sendFileSize(filesize);
    clearArray(message, BUFFER_SIZE);
    std::ifstream desiredFile(name, std::ifstream::binary);

    while (desiredFile.good()) {
        desiredFile.read(message, sizeof (char) * BUFFER_SIZE);
        sendFilePart(desiredFile.gcount());
        //        for (int i = 0; i < BUFFER_SIZE; i++) {
        //            std::cout << message[i];
        //        }
        clearArray(buffer, BUFFER_SIZE);
    }
    desiredFile.close();
    clearArray(buffer, BUFFER_SIZE);
    buffer[0] = END; //end transmission
    send(socketID, buffer, 1, 0);
    char reply[BUFFER_SIZE + 1];
    if (recv(socketID, &reply, BUFFER_SIZE + 1, 0) == -1) {
        std::cout << "There could have been problems with the upload \n";
        return;
    }
    if (reply[0] == END) {
        std::cout << "Upload successful \n";
    } else {
        std::cout << "Upload failed \n";
    }
}

void Client::sendFileSize(long& filesize) {
    clearArray(buffer, BUFFER_SIZE);
    (*flag) = RES_FILE_SIZE;

    std::string str = std::to_string(filesize) + '\0';
    std::copy(str.begin(), str.end(), message);
    send(socketID, buffer, BUFFER_SIZE + 1, 0);
}

void Client::sendFilePart(int size) {
    (*flag) = RES_FILE_PART;
    send(socketID, buffer, size + 1, 0);
}

long Client::getFileSize(std::string filename) {
    FILE *p_file = NULL;
    p_file = fopen(filename.c_str(), "rb");
    fseek(p_file, 0, SEEK_END);
    long size = ftell(p_file);
    fclose(p_file);
    return size;
}




