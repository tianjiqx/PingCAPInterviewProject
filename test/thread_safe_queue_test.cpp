#include "../include/gtest/gtest.h"
#include "../src/thread_safe_queue.h"



TEST(THREAD_SAFE_QUEUE,SINGLE_THREAD_TEST){


  threadSafeQueue<int> q1;

  q1.push(1);
  q1.push(2);


  queue<int> q2;
  q1.popALL(q2);

  EXPECT_EQ(q2.size(),2);
  EXPECT_EQ(q2.front(),1);
  EXPECT_EQ(q2.back(),2);
  EXPECT_EQ(q1.size(),0);
  

}
