#include "down_loder.h"
#include<thread>
#include<math.h>
#include<utility.h>

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

int DownLoder::popMachineNode(MachineNode *& m){
    int ret = SUCCESS;
    if (freeServerPriorityQueue.empty()){
        ret = ERR_QUEUE_EMPTY;
        LOG(INFO,ret,"无空闲服务节点");
    }else{
        //是否找到可用的空闲节点
        bool succ = false;
        while(!succ&&!freeServerPriorityQueue.empty())
        {
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
                s.print();
                //加入到busyMap
                busyServerMap.insert(PSMap::value_type(psn.id,psn));
                //移除空闲队列
                freeServerPriorityQueue.pop();
            }
        };
        if(!succ){
            ret =  ERR_QUEUE_EMPTY;
            LOG(INFO,ret,"无空闲服务节点");
        }
    }
    return ret;
}

int DownLoder::initDownLoder(vector<ProvinceServerState> &pss){
    int ret = SUCCESS;
    LOG(INFO,ret,"初始化下载器");
    //初始化空闲队列
    for(int i=0;i<pss.size();i++){
        freeServerPriorityQueue.push(pss[i].ps);
    }
    //初始化服务节点的状态
    for(int i=0;i<pss.size();i++){
        serverStates.push_back(pss[i]);
    }

    return ret;
}

int DownLoder::getResourcesInfo(string url){
    UNUSED(url);
    int ret = SUCCESS;
    //TODO 根据url选择一个服务器地址，获取资源的基本信息，整体长度

    remainTask.start=0;
    //假设资源2GB
    //remainTask.len=2*1024*1024*1024L;   //2GB
    //remainTask.len=2*1024*1024L;   //2MB
    remainTask.len=2*1024*1024*64L;   //128MB

    //初始化任务数
    taskNum=ceil(remainTask.len*1.0/MAX_HTTP_BODY_SIZE);
    cout<<"\b任务数:"<<taskNum<<endl;
    char buf[100];
    sprintf(buf,"任务数%d",taskNum);
    LOG(INFO,ret,buf);

    return ret;
}

int DownLoder::merge(){
    int ret = SUCCESS;
    //TODO
    //合并临时文件
    return ret;
}

int DownLoder::getHttpclient(string ip,HttpClient *& http){
    int ret=SUCCESS;
    HCMap::iterator it;
    if ((it=httpClientMap.find(ip))!=httpClientMap.end()){
        http=it->second;
    }
    else{
        http = new HttpClient();
        http->ip=ip;
        //根据order，设置一个伪下载时间
        int pid,mid,order;
        ip2id(ip,pid,mid);
        order = serverStates[pid].ps.order;
        http->downTime=100+order/5*100;//+order%5*10
        //http->downTime=100;
        httpClientMap.insert(HCMap::value_type(ip,http));
    }
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
        LOG(ERR,ret,"获取可服务机器出错");
    }else{
        TaskReqestRange trr;
        ret = produceTaskReqestRange(trr);
        if (ERR_TASK_EMPTY==ret){
            LOG(INFO,ret,"任务生成结束,等待当前任务完成");
            ret = SUCCESS;
        }
        else if (SUCCESS!=ret){
            LOG(INFO,ret,"获取请求范围失败");
        }else{
            TaskWorker worker(url,m->pid,m->mid,trr);
            //根据ip获取一个HttpClient，复用未关闭的http连接客户端
            HttpClient * http=NULL;
            if (SUCCESS!=(ret=getHttpclient(ips[m->pid],http))){

            }
            else{
                worker.httpClient=http;

                //生成工作线程
                thread t(down_run,worker,ref(doneTaskQueue),ref(doneTaskNum));
                //后台运行
                t.detach();
            }
        }
    }

     return ret;
}

int DownLoder::handleDoneTask(){
    int ret = SUCCESS;
    queue<DoneTask> q;
    //集中释放，减少状态锁的抢占次数
    doneTaskQueue.popALL(q);
    while(ret==SUCCESS&&!q.empty()){
        DoneTask task = q.front();
        PSMap::iterator it = busyServerMap.end();
        //检查id
        if (task.pid >=serverStates.size() || task.pid <0||
                task.mid>=serverStates[task.pid].machines.size()||
                task.mid <0)
        {
            ret = ERR_ILLEGAL_ID;
            LOG(COERROR,ret,"错误的完成任务信息");
            break;
        }
        else
        {
            switch (task.stat) {
            case TASK_DONE://正常完成
            {
                if(SUCCESS !=(ret = serverStates[task.pid].setFreeMachine(task.mid))){
                    LOG(COERROR,ret,"设置机器空闲状态失败");
                }
                //将服务节点从忙碌map中移除,并加入空闲优先队列
                else if ((it=busyServerMap.find(task.pid))!=busyServerMap.end()){
                    freeServerPriorityQueue.push(it->second);
                    busyServerMap.erase(it);
                }
                char buf[100];
                uint64_t doneNum=doneTaskNum;
                sprintf(buf,"完成任务数%u",doneNum);
                LOG(INFO,ret,buf);
                break;
            }
            case TASK_DOING:
            case TASK_REDO://重做
            {
                redoTaskQueue.push(task.trr);
                break;
            }
            case TASK_SERVER_FAIL://机器不可服务
            {
                //添加重做任务
                redoTaskQueue.push(task.trr);
                //将机器移除
                ret = serverStates[task.pid].removeMachine(task.mid);
                if (ret!=SUCCESS){

                }else if (serverStates[task.pid].aliveNum==0){
                    //设置可服务的节点数减1
                    totoalAlive--;
                }
                break;
            }
            default:
                //不应该走到此处！
                ASSERT(0);
                break;
            }
        }
        q.pop();
    }

    return ret;
}

int DownLoder::DownLoad(string url){
    this->url=url;
    int64_t start=get_current_time();
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
        LOG(INFO,ret,"开始下载资源 begin download...");
        int retryCount=3;   //重试次数
        while(retryCount--){
            //完成任务数
            doneTaskNum = 0;

            //初始化任务
            for(int i=0;i<PROVINCE_NUM*MACHINE_NUM&&hasFreeServer()&&hasRemainTask();i++){
                addTask();
            }
            //主线程循环检查，轮询
            while(true){

                //TODO:当下载任务完成一定数据量后，计算平均完成时间，重调优先级？

                //异常检查，无可用服务器
                if (ret != SUCCESS ||SUCCESS!=(ret=checkDownException())){
                    ret = COERROR;
                    LOG(ERR,ret,"下载异常，停止下载");
                    break;
                }
                //退出检查,没有剩余任务，并且所有线程结束
                else if (!hasRemainTask()&&allThreadDone()){
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
                //失败处理，可能重试，不可修复错误，重新从头开始下载
            }
            else{
                //合并分片组合为完整的资源
                if (SUCCESS != (ret=merge())){
                    LOG(ERR,ret,"合并临时文件失败");
                }else{
                    LOG(INFO,ret,"合并临时文件成功");
                    break;
                }
            }
        }
    }
    //清理环境
    clear();
    LOG(INFO,ret,"下载资源结束 end download...");
    int64_t end=get_current_time();
    char buf[100];
    sprintf(buf,"耗时%d ms",(end-start));
    LOG(INFO,ret,buf);
    return ret;
}













