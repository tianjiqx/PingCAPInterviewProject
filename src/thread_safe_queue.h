#ifndef LOCK_FREE_QUEUE_H_
#define LOCK_FREE_QUEUE_H_


#include "common.h"
#include<mutex>
//#include<condition_variable>

/**
 * @brief The LockFreeQueue class
 * 本想构建一个简单的无锁队列
 * 但是根据查阅的资料，论文显示多生产者和单消费的无锁队列比加锁的还要慢，
 * https://www.cnblogs.com/sniperHW/p/4172248.html
 * 暂停使用无锁队列，更好的替代似乎是Disruptor，但是我好像已经没有时间来了解和实现了sadness
 * https://coolshell.cn/articles/9169.html?spm=a2c4e.11153940.blogcont598122.10.18cc2d95yl2dzY
 */
//class LockFreeQueue{

//};



/**
 * 一个性能不高的线程安全队列
 */
template<class T>
class  threadSafeQueue{
private:

    queue<T> q;
    mutex mut;

public:
    /**
     * @brief push
     * 单个元素入队
     * @param item
     */
    void push(T item){
        lock_guard<mutex> lk(mut);
        q.push(item);
    }
    /**
     * @brief popALL
     * 所有元素出队
     * @param oq
     */
    void popALL(queue<T> & oq){
        lock_guard<mutex> lk(mut);
        int len=q.size();
        for(int i=0;i<len;i++){
            oq.push(q.front());
            q.pop();
        }
    }
};





#endif // LOCK_FREE_QUEUE_H_
