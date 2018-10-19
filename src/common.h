
#ifndef COMMON_H_
#define COMMON_H_

#include<iostream>
#include<vector>
#include<queue>
#include<map>

using namespace std;

const int PROVINCE_NUM = 33;  //省市数
const int MACHINE_NUM = 2;  //每省的主机数

const int MAX_HTTP_BODY_SIZE = 64*1024;   //每次请求的最大资源大小，默认64KB

const int MAX_QUEUE_SIZE = 1000; //最大任务队列长度,超过后不再添加新任务进入队列

//错误码定义
#define SUCCESS 0
#define ERROR -1
#define ERR_NULL_PTR -2
#define ERR_ILLEGAL_ID -3

#define ERR_TASK_EMPTY -100
#define ERR_QUEUE_EMPTY -101

//日志级别定义
#define ERR 0
#define WARN 1
#define INFO 2
#define DEBUG 3
//const string ERROR_TYPE []={"ERROR","WARN","INFO","DEBUG"};
const string LOG_LEVEL []={"错误","警告","信息","调试"};

/**
 * @brief LOG
 * 通用的日志信息打印函数
 * @param level 日志级别
 * @param errCode 错误码
 * @param errMsg 信息
 */
void LOG(int level,int errCode,string errMsg);


#ifndef UNUSED
#define UNUSED(v) ((void)(v))
#endif


#endif
