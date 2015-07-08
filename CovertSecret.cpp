//
// Created by Elliott on 7/7/2015.
//

#include "CovertSecret.h"
bool CovertSecret::encrypt(char in[], int in_num, char out[], int *out_num)
{
    uLongf output_size = *out_num;
    if(Z_OK == compress((Bytef*)(out),&output_size,(Bytef*)in,in_num))
    {
        *out_num = output_size;
        addKey(out,*out_num);
        return true;
    }
    else
    {
        printf("Compress Error\n");
        return false;
    }
}

bool CovertSecret::decrypt(char in[], int in_num, char out[], int *out_num)
{
    uLongf output_size = *out_num;
    removeKey(in,in_num);
    if(Z_OK == uncompress((Bytef*)(out),&output_size,(Bytef*)in,in_num))
    {
        *out_num = output_size;
        return true;
    }
    else
    {
        printf("Decompress Error\n");
        return false;
    }
}

void CovertSecret::addKey(char in[] , int in_num)
{
    for(int i=0;i<in_num;i++)
    {
        in[i]+=key;
    }
}


void CovertSecret::removeKey(char in[] , int in_num)
{
    for(int i=0;i<in_num;i++)
    {
        in[i]-=key;
    }
}
CovertSecret::CovertSecret() {
key = 7;
}