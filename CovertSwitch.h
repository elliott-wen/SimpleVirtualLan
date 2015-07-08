//
// Created by Elliott on 7/7/2015.
//

#ifndef CATCHER_COVERTSWITCH_H
#define CATCHER_COVERTSWITCH_H
#include <winsock2.h>
#include <cstdio>
#include <string>
#include "CovertSecret.h"
#include <map>
#include "CovertUtils.h"
#include <vector>
using namespace std;

#define ARPAGE 300
#include "CovertSecret.h"
class CovertSwitch {
public:
    bool init_network();
    void run();
    CovertSwitch();
private:
    bool init_winsock();
    map<string, pair<SOCKADDR_IN, long> > arp;
    string stripe_dst(char buf[]);
    string stripe_src(char buf[]);
    void updateARP(string &srcmac, SOCKADDR_IN &addr);
    void handleBoardcast(char buf[],int len);
    void handleUnicast(char buf[],int len, string &dstmac);
    bool lookupARP(string &dstmac);
    void loadSetting();
    SOCKADDR_IN localServ;
    SOCKET sock;
    CovertSecret secret;
    int localport;
    WSAData wsad;
};


#endif //CATCHER_COVERTSWITCH_H
