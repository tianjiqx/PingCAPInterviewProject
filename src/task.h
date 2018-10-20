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

    TaskReqestRange trr;    //完成的任务范围
};


/**
 * @brief The TaskWorker struct
 * 任务工作器线程
 */
struct TaskWorker{
    string ip;  //ip以ip为key找到HttpClient，没有则创建一个新的HttpClient
    string url; //资源地址
    int start;   //请求分片起始位置
    int len;    //请求分片的大小
    char *buf;  //分片数据内容
    HttpClient *httpClient;   //尝试复用已建立的的HttpClient,避免重复建立tcp连接的开销

    TaskWorker(string url,int start,int len,HttpClient* hc){
        this->url=url;
        this->start=start;
        this->len=len;
        this->httpClient=hc;
    }

    /**
     * @brief doTask
     * 调用HttpClient请求数据并保存到本地
     * @return
     */
    int doTask(){
        int ret  = SUCCESS;
        if (httpClient==NULL){
            ret=ERR_NULL_PTR;
            LOG(ERR,ret,"HttpClient 为空");
        }else if (SUCCESS!=(ret = httpClient->get(url,start,len,buf))){
            LOG(ERR,ret,"获取数据失败");
        }else if (SUCCESS!=(ret = store(buf))){
            LOG(ERR,ret,"本地写数据失败");
        }else{
            LOG(INFO,ret,"完成一个任务");
        }

        return ret;
    }
    /**
     * @brief store
     * 保存到本地，等待所有分片下载完成合并
     * @param buf
     * @return
     */
    int store(char * buf){
        int ret  = SUCCESS;
        if (buf==NULL){
            ret=ERR_NULL_PTR;
            LOG(ERR,ret,"无数据内容");
        }else {
            //TODO
        }
        return ret;
    }

};


#endif // TASK_H
