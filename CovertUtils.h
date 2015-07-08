//
// Created by Elliott on 7/8/2015.
//

#ifndef CATCHER_COVERTUTILS_H
#define CATCHER_COVERTUTILS_H
#include <map>
#include <string>
#include <fstream>
using namespace std;
class CovertUtils {
    public:
    static long getTimeStamp();
    static map<string,string> parseConfig(string &path);
};


#endif //CATCHER_COVERTUTILS_H
