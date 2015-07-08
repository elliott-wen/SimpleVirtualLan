//
// Created by Elliott on 7/7/2015.
//

#ifndef CATCHER_COVERTSECRET_H
#define CATCHER_COVERTSECRET_H

#include <zlib.h>
#include <cstdio>
#define DEBUG_VERSION
using namespace std;
class CovertSecret {
public:
    CovertSecret();
    bool encrypt(char in[],int in_num, char out[], int *out_num);
    bool decrypt(char in[],int in_num, char out[], int *out_num);

private:
    void addKey(char in[],int in_num);
    void removeKey(char in[], int in_num);
    int key;
};


#endif //CATCHER_COVERTSECRET_H
