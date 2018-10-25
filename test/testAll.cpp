#include "../include/gtest/gtest.h"
#include <iostream>
using namespace std;

int main(int argc,char* argv[])
{
        //testing::GTEST_FLAG(output) = "xml:"; //若要生成xml结果文件
        testing::InitGoogleTest(&argc,argv); //初始化
        RUN_ALL_TESTS();                     //跑单元测试
        return 0;
}
