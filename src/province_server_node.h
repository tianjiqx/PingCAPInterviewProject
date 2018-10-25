
#ifndef PROVINCE_SERVER_NODE_H_
#define PROVINCE_SERVER_NODE_H_

#include "common.h"


struct MachineNode{
    int pid;     //省市id
    int mid;     //机器id
    string ip;  //主机IP
    int stat;   //主机服务状态，HTTP状态码，正常，繁忙，无法连接,无请求资源等
    int doing;   //工作状态

    MachineNode(){
        this->pid=0;
        this->mid=0;
        this->ip="10.11.1.1";
        this->stat=MACHINE_NORMAL;
        this->doing=0;
    }
    MachineNode(int pid,int mid,string ip){
        this->pid=pid;
        this->mid=mid;
        this->ip=ip;
        this->stat=MACHINE_NORMAL;
        this->doing=0;
    }

    void print() const{
        char buf[128];
        sprintf(buf,"机器信息:pid=%d mid=%d ip=%s stat=%d doing=%d",pid,mid,ip.c_str(),stat,doing);
        LOG(INFO,SUCCESS,buf);
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

    friend bool operator < (ProvinceServerNode a, ProvinceServerNode b){
        return a.order > b.order;   //order值小的，优先级更高。
    }

    friend bool operator == (ProvinceServerNode a, ProvinceServerNode b){
      return a.id==b.id&&a.order==b.order;
    }


    /**
     * @brief print
     * 打印节点信息
     */
    void print() const {
        char buf[256];
        sprintf(buf,"id=%d order=%d",id,order);
        LOG(INFO,SUCCESS,buf);
    }
};



/**
 * @brief The ProvinceServerState struct
 * 省市服务节点的状态，将状态分离是为了避免遍历优先队列，修改空闲服务节点的机器的状态
 */
struct ProvinceServerState{

    ProvinceServerNode ps;

    int aliveNum;   //可用主机数,服务器状态正常
    int freeNum;    //空闲的机器数
    queue<int> freeMachinesIndex;  //空闲机器的索引队列，
                                   //当一个省市的机器数过多时，能够快速获取空闲的机器，避免遍历整个机器列表
    vector<MachineNode>  machines;  //机器

    /**
     * @brief print
     * 打印状态信息
     */
    void print() const{
        ps.print();
        char buf[256];
        sprintf(buf,"aliveNum=%d freeNum=%d",aliveNum,freeNum);
        LOG(INFO,SUCCESS,buf);
        for(int i=0;i<machines.size();i++){
            machines[i].print();
        }
    }

    /**
     * @brief getFreeMachine
     * 取得一个空闲的机器
     * @param m [out] 空闲机器
     * @return
     */
    int getFreeMachine(MachineNode *& m){
        int ret = SUCCESS;
        //检查
        if(aliveNum==0||freeNum==0||
                freeMachinesIndex.empty()){
            ret = ERR_QUEUE_EMPTY;
        }else{
            m = &(machines[freeMachinesIndex.front()]);
            if (m->doing){
                ret = ERR_WRONG_STASTE;
                LOG(WARN,ret,"机器工作状态错误，并未在空闲状态");
            }
            //不管是否工作状态正常，都从空闲队列中移除
            //移除索引
            freeMachinesIndex.pop();
            freeNum--;
            //设置机器状态被使用
            m->doing=1;
        }
        return ret;
    }
    /**
     * @brief pushMachine
     * 将一台机器忙碌设置为空闲
     * @param mid
     * @return
     */
    int setFreeMachine(int mid){
        int ret = SUCCESS;
        //检查mid合法性
        if (mid>=machines.size()||mid <0){
            ret = ERR_ILLEGAL_ID;
            LOG(ERR,ret,"机器id非法！");
        }else if (!machines[mid].doing){
            ret = ERR_WRONG_STASTE;
            LOG(WARN,ret,"机器工作状态错误，并未在工作状态");
        }else{
            freeMachinesIndex.push(mid);
            freeNum++;
            //设置机器状态为空闲
            machines[mid].doing=0;
        }

        return ret;
    }
    /**
     * @brief remove
     * 将一台问题机器移除
     * @param mid
     * @return
     */
    int removeMachine(int mid){
        int ret = SUCCESS;
        //检查mid合法性
        if (mid>=machines.size()||mid <0){
            ret = ERR_ILLEGAL_ID;
            LOG(ERR,ret,"机器id非法！");
        }else if (machines[mid].stat == MACHINE_NORMAL){
            aliveNum--;
            machines[mid].doing=0;
            machines[mid].stat=MACHINE_FAIL;
        }else{
            ret = ERR_WRONG_STASTE;
            LOG(WARN,ret,"机器状态错误，并未在正常状态");
        }

        return ret;
    }
};




#endif
