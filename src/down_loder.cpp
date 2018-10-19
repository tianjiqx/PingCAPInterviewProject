#include "down_loder.h"



int DownLoder::produceTaskReqestRange(TaskReqestRange & trr){
    int ret = SUCCESS;
    if (!redoTaskQueue.empty()){ //检查重做队列是否存在任务
        trr = redoTaskQueue.front();
        redoTaskQueue.pop();
    }else if (remainTask.len > 0){ //检查剩余任务范围
        trr.start=remainTask.start;
        trr.len=min(remainTask.len,MAX_HTTP_BODY_SIZE);
        remainTask.start+=trr.len;
        remainTask.len-=trr.len;
    }else{
        LOG(INFO,ret,"没有新任务");
        ret = ERR_TASK_EMPTY;
    }
    return ret;
}

int DownLoder::popMachineNode(MachineNode & m){
    int ret = SUCCESS;
    if (freeServerPriorityQueue.empty()){
        ret = ERR_QUEUE_EMPTY;
        LOG(INFO,ret,"无空闲服务节点");
    }else{
        //是否找到可用的空闲节点
        bool succ = true;
        do{
            succ= true;
            ProvinceServerNode & psn  = freeServerPriorityQueue.top();
            if (psn.id >= serverStates.size() || psn.id <0){
                ret = ERR_ILLEGAL_ID;
                LOG(ERR,ret,"服务节点的id非法");
                break;
            }
            m = serverStates[psn.id];
            if(m.aliveNum<=0||m.freeNum<=0){
                LOG(WARN,ret,"当前服务节点不可服务,不应该在空闲队列！");
                //print psn信息
                psn.print();
                m.print();
                succ=false;
                //加入到busyMap
                busyServerMap.insert(PSMap::value_type(psn.id,psn));
                //移除空闲队列
                freeServerPriorityQueue.pop();
            }
            else if ((m.freeNum-1)==0){
                //加入到busyMap
                busyServerMap.insert(PSMap::value_type(psn.id,psn));
                //移除空闲队列
                freeServerPriorityQueue.pop();
            }
        }while(succ||freeServerPriorityQueue.empty());
        if(!succ){
            ret =  ERR_QUEUE_EMPTY;
            LOG(INFO,ret,"无空闲服务节点");
        }
    }
    return ret;
}

int DownLoder::initFreeServerPriorityQueue(vector<ProvinceServerNode> & psv){
    int ret = SUCCESS;
    for(int i=0;i<psv.size();i++){
        freeServerPriorityQueue.push(psv);
    }
    return ret;
}

int DownLoder::getResourcesInfo(string url){
    UNUSED(url);
    int ret = SUCCESS;
    //TODO 根据url选择一个服务器地址，获取资源的基本信息，整体长度

    remainTask.start=0;
    remainTask.len=2*1024*1024*1024;

    return ret;
}

int DownLoder::addTask(){
    int ret = SUCCESS;

    //获取空闲服务节点
    ProvinceServerNode psn;
    ret = popMachineNode(psn);
    if (ret == ERR_QUEUE_EMPTY){
        LOG(INFO,ret,"无多余可服务节点");
        ret = SUCCESS;
        break;
    }else if(ret!=SUCCESS){
        LOG(INFO,ret,"获取可服务节点出错");
        break;
    }else{

        TaskReqestRange trr;
        ret = produceTaskReqestRange(trr);
        if (ERR_TASK_EMPTY==ret){
            LOG(INFO,ret,"任务生成结束,等待当前任务完成");
            ret = SUCCESS;
        }
        else if (SUCCESS!=ret){
            LOG(INFO,ret,"获取请求范围失败");
        }
        else{

        }
    }


     return ret;
}

int DownLoder::DownLoad(string url){
    int ret=SUCCESS;
    if (SUCCESS!=(ret=getResourcesInfo(url))){
        LOG(ERR,ret,"无法获取资源信息，检查url地址是否合法");
    }else if(freeServerPriorityQueue.empty()){
        ret = ERR_QUEUE_EMPTY;
        LOG(ERR,ret,"无空闲服务节点,无法下载资源");
    }else{
        //此处可以增加一个下载大小数据量的判断，当数据量较小不足66个分片时
        //或者指定了同时请求的节点数量时，初始化的任务数减少，
        //因为可能发太远，反而需要等待远端的地区结果
        //初始化任务
        for(int i=0;i<PROVINCE_NUM&&remainTask.len!=0;i++){



        }
    }
}













