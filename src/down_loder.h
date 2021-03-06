#ifndef DOWN_LODER_H_
#define DOWN_LODER_H_

#include<atomic>
#include "task.h"
#include "province_server_node.h"
#include "http_client.h"
#include "thread_safe_queue.h"


typedef map<int,ProvinceServerNode> PSMap;
typedef map<string,HttpClient*> HCMap;
/**
 * @brief The DownLoder class
 * 主要的下载类，完成下载调度，选择请求的服务器位置
 */
class DownLoder{
private:

    string url; //资源地址

    priority_queue<ProvinceServerNode> freeServerPriorityQueue;   //空闲访问省市服务节点优先队列
    vector<ProvinceServerState> serverStates;   //服务节点的状态
    PSMap busyServerMap;    //忙碌、或者不在线的服务节点，用于处理节点恢复后，快速查找加入正常空闲队列，扩展功能
    uint64_t totoalAlive;   //总共在线的服务机器数

    HCMap httpClientMap;    //HttpClient的hash map

    //任务内容为个服务节点请求的范围，直接用一个待请求范围的记录器，为新任务分配任务
    //没有必要生成一个任务队列，消耗内存空间
    TaskReqestRange remainTask;   //剩余分片范围，新生成的任务从这里生成任务的分配
    //失败的任务，用一个保存进重做队列，在分配任务时，申请
    queue<TaskReqestRange> redoTaskQueue;  //失败任务，重做任务队列

    //为了使工作线程无状态，减少耦合，建立已经完成任务队列，处理后续工作
    threadSafeQueue<DoneTask> doneTaskQueue;  //已完成任务队列

    uint64_t taskNum;
    atomic<uint64_t> doneTaskNum;   //已经完成的任务数，用于检查是否所有的任务已经完成，结束

    uint64_t startTime; //下载开始时间
    uint64_t timeOut;   //下载超时时间，超时后停止自动退出下载

public:
    DownLoder(){
        taskNum=0;
        doneTaskNum=0;
        timeOut=1000000; //1000s
        totoalAlive=0;
    }
    /**
     * @brief produceTaskReqestRange
     * 生成新的任务的请求范围
     * @param trr [out] 任务范围
     * @return 错误码
     */
    int produceTaskReqestRange(TaskReqestRange & trr);

    /**
     * @brief popMachineNode
     * 获取一个空闲机器
     * @param m [out]
     * @return 错误码
     */
    int popMachineNode(MachineNode *&m);

    /**
     * @brief addTask
     * 增加一个任务到线程池
     * @return
     */
    int addTask();
    /**
     * @brief handleDoneTask
     * 处理完成的任务，修改机器状态
     * @return
     */
    int handleDoneTask();

    /**
     * @brief hasRemainTask
     * 检查是否还有剩余任务
     * @return
     */
    bool hasRemainTask(){
        return remainTask.len!=0||!redoTaskQueue.empty();
    }

    bool allThreadDone(){
        return doneTaskNum==taskNum;
    }

    /**
     * @brief hasFreeServer
     * 检查是否有空闲服务节点
     * @return
     */
    bool hasFreeServer(){
        return !freeServerPriorityQueue.empty();
    }
    /**
     * @brief checkDownException
     * 下载异常检查
     * @return
     */
    int checkDownException();

    /**
     * @brief initDownLoder
     * 初始化下载器
     * @param pss [in] 省市服务节点状态数组
     * @return 错误码
     */
    int initDownLoder(vector<ProvinceServerState> &pss);

    /**
     * @brief getResourcesInfo
     * 获取资源信息，主要是初始化remainTask
     * @param url [in] 资源地址
     * @return 错误码
     */
    int getResourcesInfo(string url);

    /**
     * @brief getHttpclient
     * 获取一个Httpclient
     * @param ip [in]
     * @param http [out] 维持的客户端
     * @return
     */
    int getHttpclient(string ip, HttpClient *&http);

    /**
     * @brief DownLoad
     * 下载资源，入口
     * @param url [in] 资源地址
     * @return 错误码
     */
    int DownLoad(string url);

    /**
     * @brief merge
     * 合并临时文件
     * @return
     */
    int merge();

    /**
     * @brief clear
     * 清理环境
     */
    void clear();

    /**
    * @brief DownLoder::getRedoTaskQueue
    * 获取重做任务队列，用于单元测试
    * @return
    */
    queue<TaskReqestRange> &getRedoTaskQueue();
    /**
     * @brief getRemainTask
     * 获取剩余任务状态，用于单元测试
     * @return
     */
    TaskReqestRange &getRemainTask();
    /**
     * @brief getFreeServerPriorityQueue
     * 获取空闲队里，用于单元测试
     * @return
     */
    priority_queue<ProvinceServerNode> &getFreeServerPriorityQueue();
    /**
     * @brief getDoneTaskQueue
     * 获取完成队列
     * @return
     */
    threadSafeQueue<DoneTask>& getDoneTaskQueue();
    /**
     * @brief getDoneTaskNum
     * 获取成功完成任务数
     * @return
     */
    atomic<uint64_t> &getDoneTaskNum();

    uint64_t getTaskNum() const;
    void setTaskNum(const uint64_t &value);
};

#endif // DOWN_LODER_H
