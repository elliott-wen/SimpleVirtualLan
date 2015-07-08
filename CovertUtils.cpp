//
// Created by Elliott on 7/8/2015.
//

#include "CovertUtils.h"
#include <time.h>

long CovertUtils::getTimeStamp() {
    time_t nowtime;
    nowtime = time(NULL);
    return nowtime;
}

map<string,string> CovertUtils::parseConfig(string &path) {

    map<string,string> options;
    ifstream parseFile(path,ios::in);
    if(!parseFile.is_open())
    {
        printf("Unable to load setting %s\n",path.c_str());
    }
    string id, eq, val;
    while(parseFile >> id >> eq >> val)
    {

        if (id[0] == '#') continue;  // skip comments
        if (eq != "=") throw std::runtime_error("Parse error");
        options[id] = val;
    }
    return options;
}
