#include "ClientDummy.h"
#include "Server.h"

Server::Server(int port, char* downloadFolder) {
    this->port = port;
    this->downloadFolder = downloadFolder;
};

Server::~Server() {
};

int Server::start() {
    clientListenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    memset(&address, 0, sizeof (address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(clientListenerSocket, (struct sockaddr *) &address, sizeof (address)) != 0) {
        std::cerr << "bind error \n";
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

void Server::clearBuffer(char *buffer) {
    for (int i = 0; i < BUF; i++) {
        buffer[i] = '\0';
    }
}

int Server::getFileSize(std::string filename) {
    FILE *p_file = NULL;
    p_file = fopen(filename.c_str(), "rb");
    fseek(p_file, 0, SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);
    return size;
}

void Server::waitForClient() {
    while (running) {
        printf("Waiting for connections...\n");

        struct sockaddr_in clientAddress;
        int clientSocket = accept(clientListenerSocket, (struct sockaddr *) &clientAddress, &addressLength);
        ClientDummy dummy(this, clientAddress, clientSocket);
        clientList.push_back(&dummy);
        //std::thread t(&ClientDummy::start, dummy);
        dummy.start();
    }
}

void Server::getFileListString(char* buffer) {
    using namespace std;
    
    string str = getDir(downloadFolder);
    memcpy(buffer, str.c_str(), str.length());
}

std::string Server::getDir(std::string dir) {
    using namespace std;
    DIR *dp;
    struct dirent *dirp;
    string s;
    
    if ((dp = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return "ERROR";
    }

    while ((dirp = readdir(dp)) != NULL) {
        s.append(dirp->d_name);
        s.append(", ");
        s.append(to_string(getFileSize(dir + dirp->d_name)));
    }
    closedir(dp);
    return 0;
}
