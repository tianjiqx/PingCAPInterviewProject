#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include "common.h"

/**
 * @brief The HttpClient struct
 * 伪http客户端，当前并未实现真正的HttpClient
 * 可调用libcurl等库
 */
struct HttpClient{
    string ip;         //连接ip

    int downTime;       //伪下载时间

    int timeout;    //超时时间
    int tryCount;   //重试次数
    int threadNum;  //多线程数，此处为与一个主机多线程下载的线程数，
                    //因为考虑到本地机器可以与某省市的一台主机多线程通讯，加快请求数据，
                    //但题目提到33省市2台主机，应该是意图考察如何利用选择这66台主机的请求
                    //所以关于单台主机，我们假设单服务器主机带宽有限制，不能无限提高与单台主机的下载并发度
                    //否则，始终与最近的服务器主机通信是最快的，失去考察意义，具体连接线程数也非重点，不进行实现。

    HttpClient()
    {
        timeout=3000;   //3s超时
        tryCount=3;     //重试次数
        threadNum=1;    //单线程数
    }

    /**
     * @brief getData
     * 下载远程资源的数据
     * @param url [in] 地址
     * @param start [in] 起始地址
     * @param len [in] 长度
     * @param buf [out] 数据内容
     * @return
     */
    int getData(string url,int start,int len,char * buf){
         int ret = SUCCESS;
         UNUSED(url);
         UNUSED(start);
         UNUSED(len);
         UNUSED(buf);
         //TODO 发送http请求
         //捕获异常，设置对应错误码

         Sleep(downTime);

         return ret;
    }
};



#endif // HTTP_CLIENT_H
