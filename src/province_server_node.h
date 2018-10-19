
#ifndef PROVINCE_SERVER_NODE_H_
#define PROVINCE_SERVER_NODE_H_

#include "common.h"


struct MachineNode{
    int id;     //省市id
    string ip;  //主机IP
    int stat;   //主机服务状态，HTTP状态码，正常，繁忙，无法连接,无请求资源等
    int doing;   //工作状态
};

/**
 * @brief The ProvinceServerState struct
 * 省市服务节点的状态，将状态分离是为了避免遍历优先队列，修改空闲服务节点的机器的状态
 */
struct ProvinceServerState{

    int aliveNum;   //可用主机数,服务器状态正常
    int freeNum;    //空闲的机器数

    vector<MachineNode>  machines;  //机器

    /**
     * @brief print
     * 打印状态信息
     */
    void print(){
         string info ="aliveNum="+aliveNum+" freeNum="+freeNum;
        for(int i=0;i<machines.size();i++){
            info += ",ip="+machines[i].ip+" stat="+machines[i].stat+" doing="+machines[i].doing;
            LOG(INFO,SUCCESS,info);
        }
    }

};


/**
 * @brief The ProvinceServerNode struct
 * 省市服务节点
 */
//节约写代码时间，定义为结构体，不定义为类
struct ProvinceServerNode{
    int id;     //省市的id
    int order;  //省市的顺序

    ProvinceServerNode(){}
    ProvinceServerNode(int i,int o){
        id=i;
        order=o;
    }

    friend bool operator < (ProvinceServerNode a, ProvinceServerNode b)
    {
        return a.order > b.order;   //order值小的，优先级更高。
    }
    /**
     * @brief print
     * 打印节点信息
     */
    void print(){
        string info = "id="+id+" order="+order;
        LOG(INFO,SUCCESS,info);
    }
};


#endif
