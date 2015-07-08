//
// Created by Elliott on 7/7/2015.
//

#include "CovertDevice.h"

CovertDevice::CovertDevice() {
    memset(&tap_ol,0,sizeof(tap_ol));
    memset(&network_ol,0,sizeof(network_ol));
    lastHeartBeatTime = CovertUtils::getTimeStamp();
}

bool CovertDevice::initDevice() {
    loadSetting();
    if(!tap.init_tap())
    {
        printf("Unable to init tap");
        return false;
    }
    tap_handle = tap.get_tap_handle();

    if(!network.init_network())
    {
        printf("Unable to init network");
        return false;
    }

    network_handle = (HANDLE)network.get_sock();
}

void CovertDevice::run() {



    while(true) {
        bool waitForTap = false;
        bool waitForNetwork = false;
        tap_ol_ret = ReadFile(tap_handle, tap_buff, 2048, NULL, &tap_ol);

        if (tap_ol_ret || GetLastError() == ERROR_IO_PENDING) {
            waitForTap = true;
        }
        else {
            printf("Unknown Error While Reading Tap:%d\n", GetLastError());
            return;
        }
        network_ol_ret = ReadFile(network_handle, network_buff, 2048, NULL, &network_ol);
        if (network_ol_ret || GetLastError() == ERROR_IO_PENDING) {
            waitForNetwork = true;
        }
        else {
            printf("Unknown Error While Reading UDP:%d\n", GetLastError());
            return;
        }
        HANDLE hdArray[2];
        if (waitForNetwork) {
            hdArray[1] = network_handle;
        }
        if (waitForTap) {
            hdArray[0] = tap_handle;
        }

        DWORD dwRet = WaitForMultipleObjects(2, hdArray, false, INFINITE);
        int nIndex = dwRet - WAIT_OBJECT_0;
        if (nIndex >= 0 && nIndex < 2) {
            //printf("c1: %d\n",nIndex);
            if (WaitForSingleObject(tap_handle,0) == WAIT_OBJECT_0)
            {
                handle_tap_income();
            }

            if(WaitForSingleObject(network_handle,0) == WAIT_OBJECT_0)
            {
                handle_network_income();
            }
        }
        else
        {
            printf("Unknow Error when waiting objects\n");
            return;
        }


    }
}

void CovertDevice::handle_network_income() {
    DWORD readNum = 0;
    GetOverlappedResult(network_handle,&network_ol,&readNum,false);
    if(readNum!=0) {
        BYTE tmp[4096];
        int tmp_size=4096;
        if(secret.decrypt((char*)network_buff,(int)readNum,(char*)tmp,&tmp_size))
        {
            tap.write_data_to_tap(tmp, tmp_size);
        }
        else
        {
            printf("Bad packet drop\n");
        }
    }
    else
    {
        printf("Something wrong with the udp socket\n");
    }
}

void CovertDevice::loadSetting()
{
    string configFile = "device.ini";
    map<string,string> opts = CovertUtils::parseConfig(configFile);

    if(opts.count("server_address")==1 && opts.count("server_port") == 1)
    {
        int port = atoi(opts["server_port"].c_str());
        if(port!=0)
            network.set_removeAddr(opts["server_address"],port);
    }


}

void CovertDevice::handle_tap_income() {
    DWORD readNum = 0;
    GetOverlappedResult(tap_handle,&tap_ol,&readNum,false);
    //network.write_to_network(tap_buff,readNum);
    BYTE tmp[4096];
    int tmp_size=4096;
    secret.encrypt((char*)tap_buff,readNum,(char*)tmp,&tmp_size);
    network.write_to_network(tmp,tmp_size);
}

int main()
{
    CovertDevice device;
    if(device.initDevice())
    {
       device.run();
    }
}