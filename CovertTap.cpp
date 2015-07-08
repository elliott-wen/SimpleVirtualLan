//
// Created by Elliott on 7/6/2015.
//
#include "CovertTap.h"
#include <cstdio>
#include <windows.h>
#include <winioctl.h>
#include "tap-windows.h"
#define MAX_KEY_LENGTH 256
bool CovertTap::get_guid(string &guid) {
    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name
    DWORD    cchClassName = MAX_PATH;  // size of class string
    DWORD    cSubKeys = 0;             // number of subkeys
    DWORD    cbMaxSubKey;              // longest subkey size
    DWORD    cchMaxClass;              // longest class string
    DWORD    cValues;              // number of values for key
    DWORD    cchMaxValue;          // longest value name
    DWORD    cbMaxValueData;       // longest value data
    DWORD    cbSecurityDescriptor; // size of security descriptor
    FILETIME ftLastWriteTime;      // last write time
    HKEY hk;
    DWORD i, retCode;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, ADAPTER_KEY,0,KEY_ALL_ACCESS,&hk)!=ERROR_SUCCESS)
    {
        //printf("Failed on reading the name of the tap adapter\n");
        return false;
    }



    retCode = RegQueryInfoKey(
            hk,                    // key handle
            achClass,                // buffer for class name
            &cchClassName,           // size of class string
            NULL,                    // reserved
            &cSubKeys,               // number of subkeys
            &cbMaxSubKey,            // longest subkey size
            &cchMaxClass,            // longest class string
            &cValues,                // number of values for this key
            &cchMaxValue,            // longest value name
            &cbMaxValueData,         // longest value data
            &cbSecurityDescriptor,   // security descriptor
            &ftLastWriteTime);

    if (cSubKeys)
    {
        //printf( "\nNumber of subkeys: %d\n", cSubKeys);

        for (i = 0; i < cSubKeys; i++)
        {
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(hk, i,
                                   achKey,
                                   &cbName,
                                   NULL,
                                   NULL,
                                   NULL,
                                   &ftLastWriteTime);
            if (retCode == ERROR_SUCCESS)
            {
                //printf(TEXT("(%d) %s\n"), i + 1, achKey);
                HKEY adapthk;
                if(RegOpenKeyEx(hk, achKey,0,KEY_ALL_ACCESS,&adapthk)==ERROR_SUCCESS)
                {
                    DWORD dwType=REG_NONE;
                    DWORD dwCount=0;
                    retCode = RegQueryValueEx(adapthk, "ComponentId" ,NULL, &dwType, NULL, &dwCount);
                    if (retCode == ERROR_SUCCESS)
                    {
                        string result;
                        result.resize(dwCount);
                        RegQueryValueEx(adapthk, "ComponentId", NULL, &dwType, (LPBYTE)result.data(), &dwCount);
                        //cout<<result<<endl;
                        if(result[0]=='t'&&result[1]=='a'&&result[2]=='p')
                        {

                            retCode = RegQueryValueEx(adapthk, "NetCfgInstanceId" ,NULL, &dwType, NULL, &dwCount);
                            if (retCode == ERROR_SUCCESS)
                            {
                                result.resize(dwCount);
                                RegQueryValueEx(adapthk, "NetCfgInstanceId", NULL, &dwType, (LPBYTE)result.data(), &dwCount);
                                result.pop_back();
                                guid = result;
                                return true;
                            }

                        }


                    }


                }

            }
        }
    }
    //printf("Unable to find any device\n");
    return false;
}

string CovertTap::get_filename_from_guid(string &guid)
{
    return USERMODEDEVICEDIR+guid+".tap";
}
HANDLE CovertTap::create_tap_handle(string &dir)
{
    HANDLE handle = CreateFile(dir.data(), GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL, OPEN_EXISTING,
                               FILE_FLAG_OVERLAPPED, NULL);
    return handle;
}

bool CovertTap::enable_tap(HANDLE handle)
{
    ULONG status = TRUE;
    ULONG len;
    if (!DeviceIoControl(handle,TAP_WIN_IOCTL_SET_MEDIA_STATUS,
                         &status, sizeof (status),&status, sizeof (status), &len, NULL))
        return false;
    return true;
}

bool CovertTap::write_data_to_tap(BYTE data[], int num){
    printf("Writing  Tap %d \n",num);
    DWORD numWriten;
    OVERLAPPED tap_ol;
    int tap_ol_ret;
    memset(&tap_ol,0,sizeof(tap_ol));
    tap_ol_ret = WriteFile(tap_handle,data,num,&numWriten,&tap_ol);
    if(tap_ol_ret)
    {
        return true;
    }
    else if(GetLastError()==ERROR_IO_PENDING)
    {
        WaitForSingleObject(tap_handle,INFINITE);
        return true;
    }
    printf("Unknown Bug\n");
    return false;

}
CovertTap::CovertTap()
{
    tap_handle = INVALID_HANDLE_VALUE;
}

bool CovertTap::init_tap()
{
    string guid;
    if(!CovertTap::get_guid(guid))
    {
        printf("Unable to get guid");
        return false;
    }
    string dir = CovertTap::get_filename_from_guid(guid);
    tap_handle = CovertTap::create_tap_handle(dir);
    if(tap_handle == INVALID_HANDLE_VALUE)
    {
        printf("Unable to open handle\n");
        return false;
    }
    if(!CovertTap::enable_tap(tap_handle))
    {
        printf("Unable to enable tap");
        return false;
    }
    return true;
}

HANDLE CovertTap::get_tap_handle() {
    return this->tap_handle;
}

