//
// Created by Elliott on 7/6/2015.
//

#ifndef CATCHER_COVERTNETWORK_H
#define CATCHER_COVERTNETWORK_H

#include <windows.h>
#include <winsock2.h>
#include <string>
using namespace std;
class CovertNetwork {
public:
    CovertNetwork();
    bool init_network();
    bool write_to_network(BYTE[], int num);
    SOCKET get_sock();
    void set_localport(int port);
    void set_removeAddr(string remoteaddr,int port);
private:
    WSADATA wsad;
    SOCKADDR_IN localServ;
    SOCKADDR_IN remoteServ;
    SOCKET sock;
    bool init_winsock();
    int localport;
    int remoteport;
    string remoteaddr;
};


#endif //CATCHER_COVERTNETWORK_H
