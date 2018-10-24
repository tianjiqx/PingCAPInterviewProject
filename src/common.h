
#ifndef COMMON_H_
#define COMMON_H_

#include<iostream>
#include<vector>
#include<queue>
#include<map>
#include<string>

#include<atomic>
//#include<windows.h>  //for windows
#include <unistd.h> //for linux
#include<assert.h>
#include<sstream>

using namespace std;

const uint64_t PROVINCE_NUM = 33;  //省市数
const uint64_t MACHINE_NUM = 2;  //每省的主机数

//const uint64_t HTTP_UNIT = 1024;    //分片基本单位为1KB，使下载分片地址范围描述的更大
const uint64_t MAX_HTTP_BODY_SIZE = 64*1024L;   //每次请求的最大资源大小，默认64KB


const uint64_t MAX_QUEUE_SIZE = 1000; //最大任务队列长度,超过后不再添加新任务进入队列

//错误码定义
#define SUCCESS 0   //成功
#define COERROR -1    //通用错误
#define ERR_NULL_PTR -2     //空指针错误
#define ERR_ILLEGAL_ID -3   //非法的ID
#define ERR_WRONG_STASTE -4 //错误的状态
#define ERR_FAIL_SERVER -5 //服务器机器工作失败
#define ERR_ASSERT -6   //断言错误



#define ERR_TASK_EMPTY -100
#define ERR_QUEUE_EMPTY -101
//...

//机器状态码
#define MACHINE_NORMAL 0
#define MACHINE_FAIL 1
//...

//任务状态
#define TASK_DOING 0    //正在做
#define TASK_DONE 1     //已经完成
#define TASK_REDO 2     //需要重做
#define TASK_SERVER_FAIL 3     //服务端错误需要重做
//....


extern int log_level;

//日志级别定义
#define ERR 0
#define WARN 1
#define INFO 2
#define DEBUG 3
const static string LOG_LEVEL[4]={"\b错误:","\b警告:","\b信息:","\b调试:"};


/**
 * @brief LOG
 * 通用的日志信息打印函数
 * @param level 日志级别
 * @param errCode 错误码
 * @param errMsg 信息
 */
void LOG(int level, int errCode, string errMsg);

#ifndef UNUSED
#define UNUSED(v) ((void)(v))
#endif

void str2int(int &int_temp,const string &string_temp);



//gcc原子操作别名
/// @brief Compare And Swap
///        If the current value of *a_ptr is a_oldVal, then write a_newVal into *a_ptr
/// @return true if the comparison is successful and a_newVal was written
#define CAS(a_ptr, a_oldVal, a_newVal) __sync_bool_compare_and_swap(a_ptr, a_oldVal, a_newVal)

//断言
#define ASSERT(x) do{ if(!(x)) LOG(ERR,ERR_ASSERT, "assert fail"); assert(x);} while(false)


const static string ips[]={"10.11.1.1",
                           "10.11.1.2",
                           "10.11.1.3",
                           "10.11.1.4",
                           "10.11.1.5",
                           "10.11.1.6",
                           "10.11.1.7",
                           "10.11.1.8",
                           "10.11.1.9",
                           "10.11.1.10",
                           "10.11.1.11",
                           "10.11.1.12",
                           "10.11.1.13",
                           "10.11.1.14",
                           "10.11.1.15",
                           "10.11.1.16",
                           "10.11.1.17",
                           "10.11.1.18",
                           "10.11.1.19",
                           "10.11.1.20",
                           "10.11.1.21",
                           "10.11.1.22",
                           "10.11.1.23",
                           "10.11.1.24",
                           "10.11.1.25",
                           "10.11.1.26",
                           "10.11.1.27",
                           "10.11.1.28",
                           "10.11.1.29",
                           "10.11.1.30",
                           "10.11.1.31",
                           "10.11.1.32",
                           "10.11.1.33",
                           "10.11.1.34",
                           "10.11.1.35",
                           "10.11.1.36",
                           "10.11.1.37",
                           "10.11.1.38",
                           "10.11.1.39",
                           "10.11.1.40",
                           "10.11.1.41",
                           "10.11.1.42",
                           "10.11.1.43",
                           "10.11.1.44",
                           "10.11.1.45",
                           "10.11.1.46",
                           "10.11.1.47",
                           "10.11.1.48",
                           "10.11.1.49",
                           "10.11.1.50",
                           "10.11.1.51",
                           "10.11.1.52",
                           "10.11.1.53",
                           "10.11.1.54",
                           "10.11.1.55",
                           "10.11.1.56",
                           "10.11.1.57",
                           "10.11.1.58",
                           "10.11.1.59",
                           "10.11.1.60",
                           "10.11.1.61",
                           "10.11.1.62",
                           "10.11.1.63",
                           "10.11.1.64",
                           "10.11.1.65",
                           "10.11.1.66",
                          };

/**
 * @brief id2ip
 * id转ip
 * @param pid [in] 省id
 * @param mid [in] 机器id
 * @return
 */
string id2ip(int pid,int mid);
/**
 * @brief ip2id
 * ip转id
 * @param pid [out] 省id
 * @param mid [out] 机器id
 */
void ip2id(string ip,int &pid,int &mid);
#endif
