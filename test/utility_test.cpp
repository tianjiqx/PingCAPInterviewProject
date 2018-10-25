#include "../src/utility.h"
#include "../include/gtest/gtest.h"


TEST(UTILITY,BFS_ORDER_NORMAL_TEST){
  //正常
  vector<vector<int>> v={
    {1,2},
    {0,3},
    {0,3},
    {1,2},
  };
  queue<int> q;
  q.push(0);
  int num=4;
  vector<ProvinceServerNode> orders(num);
  vector<ProvinceServerNode> stdresult(num);

  for (int i=0;i<num;i++){
    orders[i].id=i;
    stdresult[i].id=i;
  }

  stdresult[0].order=0;
  stdresult[1].order=1;
  stdresult[2].order=2;
  stdresult[3].order=3;
  
  int ret = bfs_province_access_order(v,q,orders);
  
  ASSERT_EQ(ret,SUCCESS);
  EXPECT_EQ(q.size(),0);
  EXPECT_EQ(orders,stdresult);

  queue<int> p;
  p.push(1);
  ret = bfs_province_access_order(v,p,orders);
  stdresult[0].order=1;
  stdresult[1].order=0;
  stdresult[2].order=3;
  stdresult[3].order=2;
  
  ASSERT_EQ(ret,SUCCESS);
  EXPECT_EQ(q.size(),0);
  EXPECT_EQ(orders,stdresult);

}

TEST(UTILITY,BFS_ORDER_ILLEGAL_INPUT_TEST){
  //输入错误，孤立节点
  vector<vector<int>> v1={
    {1,2},
    {0},
    {0},
    {1,2},
  };
  queue<int> q1;
  q1.push(0);
  int num=4;
  vector<ProvinceServerNode> orders(num);
  for (int i=0;i<num;i++){
    orders[i].id=i;
  }

  
  int ret = bfs_province_access_order(v1,q1,orders);
  
  ASSERT_EQ(ret,COMMON_ERROR);
  //索引非法
  vector<vector<int>> v2={
    {1,2},
    {0,-2},
    {0,8},
    {1,2},
  };
  queue<int> q2;
  q2.push(0);
  
  ret = bfs_province_access_order(v2,q2,orders);
  
  ASSERT_EQ(ret,ERR_ILLEGAL_ID);

  //长度
  vector<ProvinceServerNode> orders2(num-1);
  for (int i=0;i<num;i++){
    orders[i].id=i;
  }
  
  ret=bfs_province_access_order(v2,q2,orders2);
  ASSERT_EQ(ret,COMMON_ERROR);

}

TEST(UTILITY,DOWN_RUN_TEST){

  TaskReqestRange trr;
  trr.start=0;
  trr.len=10;
  TaskWorker worker("url",1,1,trr);
  threadSafeQueue<DoneTask> tsq;
  atomic<uint64_t> doneTaskNum;
  doneTaskNum=0;
 
  log_level = ERR; 
  //http空指针
  int ret=down_run(worker,tsq,doneTaskNum);
  ASSERT_EQ(ret,ERR_NULL_PTR);
  EXPECT_EQ(doneTaskNum,0);
  EXPECT_EQ(tsq.size(),1);
  
  HttpClient http;
  http.downTime=100;

  worker.httpClient=&http;
  //正常下载
  ret=down_run(worker,tsq,doneTaskNum);
  ASSERT_EQ(ret,SUCCESS);
  EXPECT_EQ(doneTaskNum,1);
  EXPECT_EQ(tsq.size(),2);



}
