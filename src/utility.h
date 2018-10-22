#ifndef UTILITY_H_
#define UTILITY_H_

#include "province_server_node.h"
#include "task.h"
#include "thread_safe_queue.h"

/**
 * @brief init_provinces_neighbor_ralation
 * 初始化省市邻居关系
 * @return
 */
vector<vector<int>> init_provinces_neighbor_ralation();
/**
 * @brief bfs_province_access_order
 * 广度遍历得出省市访问顺序
 * @param pnr [in] 省市邻居关系
 * @param q [in] 广度遍历维护队列
 * @param orders [out] 访问顺序数组
 * @param size 访问顺序数组大小
 * @return 错误码
 */
int bfs_province_access_order(vector<vector<int>> &pnr,queue<int> &q,vector<ProvinceServerNode> &orders);
/**
 * @brief get_province_access_order
 * 取得省市访问顺序
 * @param pnr [in] 省市邻居关系
 * @param start [in] 开始省市，即用户所在省市
 * @param orders [out] 访问顺序数组
 * @return 错误码
 */
int get_province_access_order(vector<vector<int>> &pnr, int start,vector<ProvinceServerNode> &orders);


/**
 * @brief down_run
 *  下载线程执行函数
 * @param worker [in] 下载工作器
 * @param doneQueue [inout] 完成队列
 * @param doneTaskNum [inout] 完成成功的任务数
 * @return
 */
int down_run(TaskWorker worker, threadSafeQueue<DoneTask> & doneQueue, atomic<uint64_t> &doneTaskNum);


#endif // UTILITY_H_
