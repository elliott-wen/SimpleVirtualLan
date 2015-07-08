//
// Created by Elliott on 7/6/2015.
//

#include "CovertNetwork.h"
#include <cstdio>
using namespace std;
CovertNetwork::CovertNetwork() {
    localport = 25003;
    remoteport = 25004;
    remoteaddr = "127.0.0.1";

}

void CovertNetwork::set_removeAddr(string remoteaddr, int port) {
    this->remoteaddr=remoteaddr;
    this->remoteport = port;
    printf("Setting server %s:%d\n",remoteaddr.c_str(),port);
}
bool CovertNetwork::init_network() {
    if(!init_winsock())
    {
        printf("Unable to init winsock!\n");
        return false;
    }
    this->sock = socket(AF_INET,SOCK_DGRAM,0);
    if(INVALID_SOCKET == sock)
    {
        printf("UDP socket failed\n");
        return  false;
    }
    localServ.sin_family = AF_INET;
    localServ.sin_port = htons(localport);
    localServ.sin_addr.s_addr = INADDR_ANY;
    remoteServ.sin_family = AF_INET;
    remoteServ.sin_port = htons(remoteport);
    remoteServ.sin_addr.s_addr=inet_addr(remoteaddr.data());
    if(0 != bind(sock,(sockaddr*)&localServ,sizeof(localServ)))
    {
        printf("Bind failed!\n");
        return false;
    }
    unsigned long ul=1;

    if(ioctlsocket(sock,FIONBIO,(unsigned long *)&ul)==SOCKET_ERROR)//?????
    {
        printf("Async Sockets failed!\n");
        return false;
    }
    return true;
}

bool CovertNetwork::write_to_network(BYTE data[], int num){
    bool r =  sendto(this->sock,(char *)data,num,0,(sockaddr*)&remoteServ,sizeof(remoteServ))!=SOCKET_ERROR;
    if(!r)
    {
        printf("Unknown Bug network\n");
    }
    printf("Send out to network %d\n",num);
    return r;
}
void CovertNetwork::set_localport(int port) {
    this->localport = port;
}

SOCKET CovertNetwork::get_sock() {
    return this->sock;
}
bool CovertNetwork::init_winsock()
{
    if (WSAStartup(MAKEWORD(2,2),&wsad) != 0)
    {
        return false;
    }
    return true;
}