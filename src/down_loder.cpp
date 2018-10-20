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

int DownLoder::popMachineNode(MachineNode * m){
    int ret = SUCCESS;
    if (freeServerPriorityQueue.empty()){
        ret = ERR_QUEUE_EMPTY;
        LOG(INFO,ret,"无空闲服务节点");
    }else{
        //是否找到可用的空闲节点
        bool succ = true;
        do{
            succ= true;
            const ProvinceServerNode & psn  = freeServerPriorityQueue.top();
            if (psn.id >= serverStates.size() || psn.id <0){
                ret = ERR_ILLEGAL_ID;
                LOG(ERR,ret,"服务节点的id非法");
                break;
            }
            ProvinceServerState & s = serverStates[psn.id];

            if (s.freeNum>1 && (SUCCESS==(ret=s.getFreeMachine(m)))){
                LOG(INFO,ret,"获取一台空闲机器");
                m->print();
            }else{
                if(s.aliveNum<=0||s.freeNum<=0){
                    LOG(WARN,ret,"当前服务节点不可服务,不应该在空闲队列！");
                    succ=false;
                }
                else if ((s.freeNum-1)==0 &&(SUCCESS==(ret=s.getFreeMachine(m)))){
                    LOG(INFO,ret,"获取一台空闲机器");
                    m->print();
                }else{
                    succ=false;
                }
                //打印服务节点基本信息
                psn.print();
                //打印服务节点状态
                s.print();
                //加入到busyMap
                busyServerMap.insert(PSMap::value_type(psn.id,psn));
                //移除空闲队列
                freeServerPriorityQueue.pop();
            }
        }while(!succ||freeServerPriorityQueue.empty());
        if(!succ){
            ret =  ERR_QUEUE_EMPTY;
            LOG(INFO,ret,"无空闲服务节点");
        }
    }
    return ret;
}

int DownLoder::initDownLoder(vector<ProvinceServerNode> & psv,vector<ProvinceServerState> &pss){
    int ret = SUCCESS;

    if (psv.size()!=pss.size()){
        ret = ERROR;
        LOG(ERR,ret,"初始化信息错误");
    }else{
        //初始化空闲队列
        for(int i=0;i<psv.size();i++){
            freeServerPriorityQueue.push(psv[i]);
        }
        //初始化服务节点的状态
        for(int i=0;i<pss.size();i++){
            serverStates.push_back(pss[i]);
        }
        //初始化线程池
    }

    return ret;
}

int DownLoder::getResourcesInfo(string url){
    UNUSED(url);
    int ret = SUCCESS;
    //TODO 根据url选择一个服务器地址，获取资源的基本信息，整体长度

    remainTask.start=0;
    //假设资源2GB
    remainTask.len=2*1024*1024*1024L;   //2GB

    return ret;
}

int DownLoder::addTask(){
    int ret = SUCCESS;

    //获取空闲机器
    MachineNode * m = NULL;
    ret = popMachineNode(m);
    if (ret == ERR_QUEUE_EMPTY){
        LOG(INFO,ret,"无多余可服务机器");
        ret = SUCCESS;
    }else if(ret!=SUCCESS){
        LOG(INFO,ret,"获取可服务机器出错");
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
            //追加任务到线程池
        }
    }

     return ret;
}

int DownLoder::handleDoneTask(){
    int ret = SUCCESS;
    //集中释放，减少状态锁的抢占次数
    while(!doneTaskQueue.empty()){
        DoneTask task = doneTaskQueue.front();
        PSMap::iterator it = busyServerMap.end();
        //检查id
        if (task.pid >=serverStates.size() || task.pid <0||
                task.mid>=serverStates[task.pid].machines.size()||
                task.mid <0)
        {
            ret = ERR_ILLEGAL_ID;
            LOG(ERROR,ret,"错误的完成任务信息");
            break;
        }
        else if(SUCCESS !=(ret = serverStates[task.pid].setFreeMachine(task.mid))){
            LOG(ERROR,ret,"设置机器状态失败");
            break;
        }
        //将服务节点从忙碌map中移除,并加入空闲优先队列
        else if ((it=busyServerMap.find(task.pid))!=busyServerMap.end()){
            freeServerPriorityQueue.push(it->second);
            busyServerMap.erase(it);
        }
        doneTaskQueue.pop();
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
        for(int i=0;i<PROVINCE_NUM&&hasFreeServer()&&hasRemainTask();i++){
            addTask();
        }
        //主线程循环检查
        while(true){
            //异常检查，无可用服务器
            if (ret != SUCCESS ||checkDownException()){
                ret = ERROR;
                LOG(ERR,ret,"下载异常，停止下载");
            }
            //退出检查,没有剩余任务，并且所有线程结束
            else if (!hasRemainTask()){
                LOG(INFO,ret,"所有内容下载完成");
                break;
            }else{
                //处理任务队列的完成内容
                if (ret = handleDoneTask()){
                    LOG(ERR,ret,"处理完成任务失败");
                }
                //添加新任务
                while(hasRemainTask()&&hasFreeServer()&&ret==SUCCESS){
                    ret = addTask();
                }
            }
        }//end while

        if (ret != SUCCESS){
            //TODO
            //失败处理，可能重试，重新从头开始下载
        }
    }
    //清理环境
    clear();

    return ret;
}













