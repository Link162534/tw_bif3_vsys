/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Client.h
 * Author: voki
 *
 * Created on 4. Oktober 2016, 15:53
 */

#ifndef CLIENT_H
#define CLIENT_H

class Client {
public:
    Client();
    Client(const Client& orig);
    virtual ~Client();
    
    void openConnection();
private:

};

#endif /* CLIENT_H */

