#ifndef TASK_H
#define TASK_H

#include "http_client.h"


struct TaskReqestRange{
    uint64_t start;
    uint64_t len;
};

struct DoneTask{
    int pid;    //省市id
    int mid;    //机器id

    int stat;   //完成状态
    int times;  //下载时间

    TaskReqestRange trr;    //完成的任务范围
};


/**
 * @brief The TaskWorker struct
 * 任务工作器
 */
struct TaskWorker{
    string ip;  //ip以ip为key找到HttpClient，没有则创建一个新的HttpClient

    string url; //资源地址
    int pid;     //省市id
    int mid;     //机器id
    TaskReqestRange trr;    //任务范围

    char *buf;  //分片数据内容
    HttpClient *httpClient;   //尝试复用已建立的的HttpClient,避免重复建立tcp连接的开销
    TaskWorker(string url,int pid,int mid,TaskReqestRange trr){
        this->url=url;
        this->pid=pid;
        this->mid=mid;
        this->trr=trr;
        this->httpClient=NULL;
    }

    /**
     * @brief handleDownRequests
     * 处理下载请求,调用HttpClient请求数据
     * @return
     */
    int handleDownRequests(){
        int ret = SUCCESS;

        if (httpClient==NULL){
            ret=ERR_NULL_PTR;
            LOG(ERR,ret,"HttpClient 为空");
        }else if (SUCCESS!=(ret = httpClient->getData(url,trr.start,trr.len,buf))){
            LOG(ERR,ret,"获取数据失败");
        }else{
            LOG(INFO,ret,"下载完一个任务");
        }
        return ret;
    }

    /**
     * @brief store
     * 缓冲区内容保存到本地，等待所有分片下载完成合并
     * @param buf
     * @return
     */
    int store(){
        int ret  = SUCCESS;
        if (false){ //buf==NULL
        //if (buf==NULL){
            ret=ERR_NULL_PTR;
            LOG(ERR,ret,"无数据内容");
        }else {
            //TODO 写临时文件
            //捕获异常，设置对应错误码
        }
        return ret;
    }

};


#endif // TASK_H
