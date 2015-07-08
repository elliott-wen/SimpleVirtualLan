//
// Created by Elliott on 7/7/2015.
//

#include "CovertSwitch.h"
#include <string>
#include <iostream>

bool CovertSwitch::init_winsock()
{
    if (WSAStartup(MAKEWORD(2,2),&wsad) != 0)
    {
        return false;
    }
    return true;
}

CovertSwitch::CovertSwitch() {
    localport = 25004;
}
void CovertSwitch::loadSetting()
{
    string configFile = "switch.ini";
    map<string,string> opts = CovertUtils::parseConfig(configFile);

    if(opts.count("server_port") == 1)
    {
        int port = atoi(opts["server_port"].c_str());
        if(port!=0)
            localport = port;
    }
}

bool CovertSwitch::init_network() {
    loadSetting();
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
    if(0 != bind(sock,(sockaddr*)&localServ,sizeof(localServ)))
    {
        printf("Bind failed!\n");
        return false;
    }
    return true;
}

void CovertSwitch::updateARP(string &srcmac, SOCKADDR_IN &addr) {
    arp[srcmac] = pair<SOCKADDR_IN,long>(addr,CovertUtils::getTimeStamp());
}



void CovertSwitch::run() {
    char recvBuf[2048];
    while(true)
    {
        SOCKADDR_IN addrClient;
        int len = sizeof(addrClient);

        int readNum = recvfrom(this->sock,recvBuf,2048,0,(sockaddr*)&addrClient,&len);
        if(readNum<=0)
        {
            printf("Bad packet dropped\n");
            continue;
        }
        char *ip = inet_ntoa(addrClient.sin_addr);
        int port = addrClient.sin_port;
        printf("recv from %s:%d %d bytes\n",ip,port,readNum);
        char tmp[4096];
        int tmp_size=4096;
        if(secret.decrypt((char*)recvBuf,(int)readNum,(char*)tmp,&tmp_size))
        {
            string dst = stripe_dst(tmp);
            string src = stripe_src(tmp);
            updateARP(src,addrClient);
            if(lookupARP(dst))
            {
                handleUnicast(tmp,tmp_size,dst);
            }
            else {
                handleBoardcast(tmp, tmp_size);
            }
        }
        else
        {
            printf("Bad packet dropped\n");
        }

    }
}

void CovertSwitch::handleUnicast(char buf[], int len, string &dstmac)
{
    printf("Handle Unicast to %s\n",dstmac.data());
    pair<SOCKADDR_IN,long> record = this->arp[dstmac];
    char tmp[4096];
    int tmp_size=4096;
    this->secret.encrypt((char*)buf,len,(char*)tmp,&tmp_size);
    sendto(this->sock,tmp,tmp_size,0,(sockaddr*)(&(record.first)),sizeof(record.first));
    if(CovertUtils::getTimeStamp()-record.second>ARPAGE)
    {
        this->arp.erase(dstmac);
    }
}
bool CovertSwitch::lookupARP(string &dstmac) {
    return this->arp.count(dstmac)==1;
}
void CovertSwitch::handleBoardcast(char buf[], int len)
{
    printf("Handle Boardcast \n");
    map<string,pair<SOCKADDR_IN,long> >::iterator it;
    vector<string> to_del_vector;
    for(it=arp.begin();it!=arp.end();++it)
    {
        char tmp[4096];
        int tmp_size=4096;
        this->secret.encrypt((char*)buf,len,(char*)tmp,&tmp_size);
        sendto(this->sock,tmp,tmp_size,0,(sockaddr*)(&(it->second.first)),sizeof(it->second.first));
        if(CovertUtils::getTimeStamp()-it->second.second>ARPAGE)
        {
            to_del_vector.push_back(it->first);
        }
    }
    vector<string>::iterator vit;
    for(vit=to_del_vector.begin();vit!=to_del_vector.end();vit++)
    {
        this->arp.erase(*vit);
    }
}

string CovertSwitch::stripe_dst(char buf[])
{
    unsigned char *inbuf = (unsigned char*)buf;
    char tmp[16];
    sprintf(tmp,"%02x%02x%02x%02x%02x%02x",inbuf[0],inbuf[1],inbuf[2],inbuf[3],inbuf[4],inbuf[5],inbuf[6]);
    return string(tmp);
}

string CovertSwitch::stripe_src(char buf[])
{
    unsigned char *inbuf = (unsigned char*)buf+6;
    char tmp[16];
    sprintf(tmp,"%02x%02x%02x%02x%02x%02x",inbuf[0],inbuf[1],inbuf[2],inbuf[3],inbuf[4],inbuf[5],inbuf[6]);
    return string(tmp);
}

int main()
{
    CovertSwitch switcher;
    if(!switcher.init_network()) {
        printf("Unable to init network!");
        return -1;
    }
    switcher.run();
}
