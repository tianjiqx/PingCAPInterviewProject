
#include "common.h"
#include <string>
#include <mutex>

int log_level = INFO;
//为了使多线程打印时日志能够完整打印
mutex lgmutx;

void LOG(int level,int errCode,string errMsg){
    lock_guard<mutex> lk(lgmutx);
    if (log_level>=level){
        string str=":\b"+errMsg;
        cout<<LOG_LEVEL[level]<<str<<",ret="<<errCode<<endl;
    }
}


void str2int(int &int_temp,const string &string_temp)
{
    stringstream stream(string_temp);
    stream>>int_temp;
}

string id2ip(int pid,int mid){
    return ips[pid*2+mid];
}

void ip2id(string ip,int &pid,int &mid){
    int tmp;
    str2int(tmp,ip.substr(8,ip.length()));
    pid=(tmp-1)/2;
    mid=(tmp-1)%2;
}
