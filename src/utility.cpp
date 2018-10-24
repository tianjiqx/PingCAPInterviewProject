#include "utility.h"

//省市相邻关系
/*
id,简称，相邻省id
0, 京, 1, 2,
1, 津, 0, 2, 14,
2, 冀, 0, 1, 4, 5, 14, 9, 15, 3,
3, 晋, 2, 26, 15, 4,
4, 蒙, 7, 6, 5, 2, 3, 26, 29, 27,
5, 辽, 6, 4, 2,
6, 吉, 5, 4, 7,
7, 黑, 4, 6,
8, 沪, 9, 10,
9, 苏, 8, 10, 11, 15, 14,
10, 浙, 13, 9, 11, 8, 12,
11, 皖, 14, 9, 10, 13, 16, 15,
12, 闽, 13, 18, 10,
13, 赣, 16, 17, 11, 12, 10, 18,
14, 鲁, 2, 15, 11, 9,
15, 豫, 3, 26, 16, 14, 11,
16, 鄂, 15, 11, 13, 17, 21, 26,
17, 湘, 16, 18, 19, 21, 23, 13,
18, 粤, 12, 13, 16, 19, 31, 32, 20,
19, 桂, 18, 17, 23, 24,
20, 琼, 18,
21, 渝, 16, 17, 23, 22, 26,
22, 川, 21, 27, 23, 24, 26, 25, 28,
23, 黔, 21, 22, 24, 19,
24, 滇, 23, 22, 25, 19,
25, 藏, 30, 28, 22, 24,
26, 陕, 21, 22, 16, 15, 3, 27, 4, 29,
27, 甘, 4, 29, 26, 22, 28, 30,
28, 青, 30, 27, 22, 25,
29, 宁, 26, 4, 27,
30, 新, 27, 28, 25,
31, 港, 32, 18,
32, 澳, 31, 18,
排除台湾
 */

vector<vector<int>> init_provinces_neighbor_ralation(){

    vector<vector<int>> v ={
        {1, 2},
        {0, 2, 14},
        {0, 1, 4, 5, 14, 9, 15, 3},
        {2, 26, 15, 4},
        {7, 6, 5, 2, 3, 26, 29, 27},
        {6, 4, 2},
        {5, 4, 7},
        {4, 6},
        {9, 10},
        {8, 10, 11, 15, 14},
        {13, 9, 11, 8, 12},
        {14, 9, 10, 13, 16, 15},
        {13, 18, 10},
        {16, 17, 11, 12, 10, 18},
        {2, 15, 11, 9},
        {3, 26, 16, 14, 11},
        {15, 11, 13, 17, 21, 26},
        {16, 18, 19, 21, 23, 13},
        {12, 13, 16, 19, 31, 32, 20},
        {18, 17, 23, 24},
        {18},
        {16, 17, 23, 22, 26},
        {21, 27, 23, 24, 26, 25, 28},
        {21, 22, 24, 19},
        {23, 22, 25, 19},
        {30, 28, 22, 24},
        {21, 22, 16, 15, 3, 27, 4, 29},
        {4, 29, 26, 22, 28, 30},
        {30, 27, 22, 25},
        {26, 4, 27},
        {27, 28, 25},
        {32, 18},
        {31, 18},
    };

    return v;
}

/*
广度遍历的方法按省市的邻近关系，确定访问省市的优先顺序
根据请求数据的用户所在省市，通过遍历邻近省市作为下一个请求地址，
当所有直接相邻的省市已经请求，则异常请求间接相邻的省市，直到所有的省市的服务节点的优先顺序确定。
TODO：需要注意的是，省市大小面积不一，实际优先顺序或许应该通过测试每个省市到各个省市机器的时间作为优先顺序，
可以根据各省市的平均请求数据的响应时间，重置访问优先顺序
*/

int bfs_province_access_order(vector<vector<int>> &pnr,queue<int> &q,vector<ProvinceServerNode> &orders){
    int ret = SUCCESS;
    int order=0;    //省市访问顺序、优先级
    int size = orders.size();
    bool visited[size]={0};  //标记节点是否被访问
    visited[q.front()]=1;   //标记起始节点已经被访问
    while(!q.empty()){
        int idx=q.front();
        q.pop();
        orders[idx].order=order++;    //设置优先级
        int i=0,n=pnr[idx].size();
        while(i<n){ //遍历邻居省市，未被访问到的加入队列
            if (!visited[pnr[idx][i]]){
                q.push(pnr[idx][i]);
                visited[pnr[idx][i]]=1; //标记已被访问
            }
            i++;
        }
    }

    //检查visited所有项都应该置为1,若存在非1则说明邻居关系可能存在单边，输入错误
    for(int i=0;i<size;i++){
        if (!(1&visited[i])){
            ret = COERROR;
        }
    }

    return ret;
}

int get_province_access_order(vector<vector<int>> &pnr, int start,vector<ProvinceServerNode> &orders){
    int ret = SUCCESS;
    queue<int> q;
    q.push(start);
    //广度遍历设置优先顺序
    if (SUCCESS!=(ret = bfs_province_access_order(pnr,q,orders))){
        LOG(ERR,ret,"广度遍历设置优先顺序失败");
    }
    return ret;
}

int down_run(TaskWorker worker,threadSafeQueue<DoneTask> & doneQueue,atomic<uint64_t> & doneTaskNum){

    int ret = SUCCESS;
    //完成任务信息
    DoneTask task;
    task.pid=worker.pid;
    task.mid=worker.mid;
    task.trr=worker.trr;
    task.stat=TASK_DOING;
    //下载
    if (SUCCESS!=(ret = worker.handleDownRequests())){
        LOG(ERR,ret,"下载失败");
        //TODO根据错误类型，设置状态
        task.stat=TASK_SERVER_FAIL;//超时 TASK_REDO
    }else{
        //正常完成
        task.stat=TASK_DONE;
        //设置任务完成时间
        task.times=worker.httpClient->downTime;
        LOG(INFO,ret,"下载成功");
    }
    //向完成队列添加一个done/fail/redo DoneTask,提前生成下一个任务
    doneQueue.push(task);

    if (ret!=SUCCESS){
    }
    else if(SUCCESS!= (ret = worker.store())){
        LOG(WARN,ret,"本地写数据失败");
        task.stat=TASK_REDO;
        //向完成队列再添加一个redo DoneTask,重新下载
        doneQueue.push(task);
    }else{
        LOG(INFO,ret,"写数据到磁盘成功");
        //修改完成成功的任务数
        doneTaskNum++;
    }

    return ret;
}

int64_t get_current_time()
{
    timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
