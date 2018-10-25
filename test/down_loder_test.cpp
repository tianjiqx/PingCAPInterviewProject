#include "../src/down_loder.h"
#include "../include/gtest/gtest.h"


TEST(DOWN_LOADER,PRODUCE_TASK_TASK_TEST){

  DownLoder  d;
  log_level=ERR;
  TaskReqestRange trr;
  trr.start=0;
  trr.len=10;
  d.getRedoTaskQueue().push(trr);
  
  TaskReqestRange range;
  //从redo队列获取任务
  int ret=d.produceTaskReqestRange(range);
  ASSERT_EQ(ret,SUCCESS);
  EXPECT_EQ(range.start,trr.start);
  EXPECT_EQ(range.len,trr.len);

  d.getResourcesInfo("url");
  //检查从任务范围获取任务
  ret=d.produceTaskReqestRange(range);
  ASSERT_EQ(ret,SUCCESS);
  EXPECT_EQ(range.start,0);
  EXPECT_EQ(range.len,MAX_HTTP_BODY_SIZE);

  ret=d.produceTaskReqestRange(range);
  ASSERT_EQ(ret,SUCCESS);
  EXPECT_EQ(range.start,MAX_HTTP_BODY_SIZE);
  EXPECT_EQ(range.len,MAX_HTTP_BODY_SIZE);

  //检查做完
  d.getRemainTask().len=0;
  ret=d.produceTaskReqestRange(range);
  ASSERT_EQ(ret,ERR_TASK_EMPTY);

}

TEST(DOWN_LOADER,GET_FREE_MACHINE_TEST){

  DownLoder d;

  vector<ProvinceServerState> pss;
  ProvinceServerState s1;
  ProvinceServerNode n1(0,0);
  s1.ps=n1;
  s1.aliveNum=1;
  s1.freeNum=1;
  (s1.freeMachinesIndex).push(0);
  MachineNode m1;
  m1.pid=0;
  m1.mid=0;
  (s1.machines).push_back(m1);
  pss.push_back(s1);
  
  d.initDownLoder(pss);

  MachineNode *freem=NULL;
  //空闲队列不为空
  int ret=d.popMachineNode(freem);
  ASSERT_EQ(ret,SUCCESS);
  EXPECT_EQ(freem->pid,m1.pid);
  EXPECT_EQ(freem->mid,m1.mid);
  //空闲队列为空
  ret=d.popMachineNode(freem);
  ASSERT_EQ(ret,ERR_QUEUE_EMPTY);

}

TEST(DOWN_LOADER,ADD_TASK_TEST){

  DownLoder d;

  vector<ProvinceServerState> pss;
  ProvinceServerState s1;
  ProvinceServerNode n1(0,0);
  s1.ps=n1;
  s1.aliveNum=1;
  s1.freeNum=1;
  (s1.freeMachinesIndex).push(0);
  MachineNode m1;
  m1.pid=0;
  m1.mid=0;
  (s1.machines).push_back(m1);
  pss.push_back(s1);
 
  log_level=ERR; 
  d.initDownLoder(pss);
  d.getResourcesInfo("url");
  atomic<uint64_t> & num =d.getDoneTaskNum();
  num=0;
  int ret=d.addTask();
  ASSERT_EQ(ret,SUCCESS);
  //等待子线程完成
  sleep(1);

  EXPECT_EQ(d.getDoneTaskQueue().size(),1);
  EXPECT_EQ(d.getDoneTaskNum(),1);

  //无空闲机器时
  ret=d.addTask();
  
  ASSERT_EQ(ret,SUCCESS);

  sleep(1);
  EXPECT_EQ(d.getDoneTaskNum(),1); 


}


TEST(DOWN_LOADER,HANDLE_DONE_TASK_TEST){

  DownLoder d;

  vector<ProvinceServerState> pss;
  ProvinceServerState s1;
  ProvinceServerNode n1(0,0);
  s1.ps=n1;
  s1.aliveNum=1;
  s1.freeNum=1;
  (s1.freeMachinesIndex).push(0);
  MachineNode m1;
  m1.pid=0;
  m1.mid=0;
  (s1.machines).push_back(m1);
  MachineNode *pm=NULL;
  s1.getFreeMachine(pm);

  pss.push_back(s1);
 
  log_level=ERR; 
  d.initDownLoder(pss);
  d.getResourcesInfo("url");
  
  DoneTask dt;
  dt.pid=0;
  dt.pid=0;
  dt.stat=TASK_REDO;
 
  d.getDoneTaskQueue().push(dt);
  //正常完成
  int ret=d.handleDoneTask();
  ASSERT_EQ(ret,SUCCESS);
  EXPECT_EQ(d.getRedoTaskQueue().size(),1);
  EXPECT_EQ(d.getDoneTaskQueue().size(),0);

  dt.stat=TASK_DONE; 
  d.getDoneTaskQueue().push(dt);
  //正常完成
  ret=d.handleDoneTask();
  ASSERT_EQ(ret,SUCCESS);
  EXPECT_EQ(d.getDoneTaskQueue().size(),0);
  //服务器异常
  dt.stat=TASK_SERVER_FAIL;
  ret=d.handleDoneTask();
  ASSERT_EQ(ret,SUCCESS);
  EXPECT_EQ(d.getRedoTaskQueue().size(),1);
  EXPECT_EQ(d.getDoneTaskQueue().size(),0);

}

TEST(DOWN_LOADER,DOWN_LOAD_TEST){

  DownLoder d;

  vector<ProvinceServerState> pss;
  ProvinceServerState s1;
  ProvinceServerNode n1(0,0);
  s1.ps=n1;
  s1.aliveNum=2;
  s1.freeNum=2;
  (s1.freeMachinesIndex).push(0);
  (s1.freeMachinesIndex).push(1);
  
  MachineNode m1;
  m1.pid=0;
  m1.mid=0;
  (s1.machines).push_back(m1);
  m1.pid=0;
  m1.mid=1;
  (s1.machines).push_back(m1);
  pss.push_back(s1);
  n1.id=1;
  n1.order=1;
  s1.ps=n1;
  s1.machines.clear();
  m1.pid=1;
  m1.mid=0;
  (s1.machines).push_back(m1);
  m1.pid=1;
  m1.mid=1;
  (s1.machines).push_back(m1);
  pss.push_back(s1);


  log_level=ERR; 
  d.initDownLoder(pss);

  int ret=d.DownLoad("url");
  ASSERT_EQ(ret,SUCCESS);
 
} 
