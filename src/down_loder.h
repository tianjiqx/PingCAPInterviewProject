#ifndef DOWN_LODER_H_
#define DOWN_LODER_H_

#include "task.h"
#include "province_server_node.h"
#include "http_client.h"

typedef map<int,ProvinceServerNode> PSMap;
typedef map<string,HttpClient*> HCMap;
/**
 * @brief The DownLoder class
 * 主要的下载类，完成下载调度，选择请求的服务器位置
 */
class DownLoder{
private:

    priority_queue<ProvinceServerNode> freeServerPriorityQueue;   //空闲访问省市服务节点优先队列
    vector<ProvinceServerState> serverStates;   //服务节点的状态
    PSMap busyServerMap;    //忙碌、或者不在线的服务节点，用于处理节点恢复后，快速查找加入正常空闲队列，扩展功能

    HCMap httpClientMap;    //HttpClient的hash map

    //任务内容为个服务节点请求的范围，直接用一个待请求范围的记录器，为新任务分配任务
    //没有必要生成一个任务队列，消耗内存空间
    TaskReqestRange remainTask;   //剩余分片范围，新生成的任务从这里生成任务的分配
    //失败的任务，用一个保存进重做队列，在分配任务时，申请
    queue<TaskReqestRange> redoTaskQueue;  //失败任务，重做任务队列

    //为了使工作线程无状态，减少耦合，建立已经完成任务队列，处理后续工作
    queue<DoneTask> doneTaskQueue;  //已完成任务队列

public:

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
    int popMachineNode(MachineNode &m);

    int pushMachineNode();


    /**
     * @brief addTask
     * 增加一个任务到线程池
     * @return
     */
    int addTask();

    /**
     * @brief initFreeServerPriorityQueue
     * 初始化空闲队列
     * @param psv [in] 省市服务节点数组
     * @return 错误码
     */
    int initFreeServerPriorityQueue(vector<ProvinceServerNode> & psv);

    /**
     * @brief getResourcesInfo
     * 获取资源信息，主要是初始化remainTask
     * @param url [in] 资源地址
     * @return 错误码
     */
    int getResourcesInfo(string url);

    /**
     * @brief DownLoad
     * 下载资源，入口
     * @param url [in] 资源地址
     * @return 错误码
     */
    int DownLoad(string url);



};

#endif // DOWN_LODER_H
