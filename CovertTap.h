//
// Created by Elliott on 7/6/2015.
//

#ifndef CATCHER_UTILS_H
#define CATCHER_UTILS_H
#include <string>
#include <windows.h>

using namespace std;
class CovertTap
{
public:
    CovertTap();
    bool init_tap();
    HANDLE get_tap_handle();
    bool write_data_to_tap(BYTE[],int num);
private:
    HANDLE tap_handle;
    bool get_guid(string &guid);
    string get_filename_from_guid(string& guid);
    HANDLE create_tap_handle(string &dir);
    bool enable_tap(HANDLE handle);
};

#endif //CATCHER_UTILS_H
