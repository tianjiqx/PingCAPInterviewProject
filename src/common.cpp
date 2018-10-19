
#include "common.h"

void LOG(int level,int errCode,string errMsg){
    cout<<LOG_LEVEL[level]<<":"<<errMsg<<",ret="<<errCode<<endl;
}
