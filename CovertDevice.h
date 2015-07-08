//
// Created by Elliott on 7/7/2015.
//

#ifndef CATCHER_COVERTDEVICE_H
#define CATCHER_COVERTDEVICE_H
#include "CovertTap.h"
#include "CovertNetwork.h"
#include "CovertSecret.h"
#include "CovertUtils.h"

class CovertDevice {
public:
    CovertDevice();
    bool initDevice();
    void run();
private:
    void loadSetting();
    void handle_network_income();
    void handle_tap_income();

    CovertTap tap;
    CovertSecret secret;
    CovertNetwork network;
    HANDLE tap_handle;
    HANDLE network_handle;
    OVERLAPPED tap_ol,network_ol;
    int tap_ol_ret, network_ol_ret;
    BYTE tap_buff[2048];
    BYTE network_buff[2048];
    long lastHeartBeatTime;
};


#endif //CATCHER_COVERTDEVICE_H
