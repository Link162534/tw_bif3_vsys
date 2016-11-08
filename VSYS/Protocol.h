#ifndef PROTOCOL_H
#define PROTOCOL_H

#define DEFAULT_PORT 7777
#define LOCALHOST "127.0.0.1"
#define BUFFER_SIZE 1024
#define PACKET_SIZE BUFFER_SIZE + 1
#define DEFAULT_FOLDER "./downloads/"

enum PACKET_TYPE {
    MESSAGE = 0, 
    REQ_LIST = 1, REQ_GET = 2, REQ_PUT = 3, 
    END = 4, FAILURE = 5, 
    RES_FILE_PART = 6, RES_FILE_SIZE = 7, RES_LIST_ANSWER = 8,
    LOGIN = 9, BANNED = 10         
};

#endif /* PROTOCOL_H */