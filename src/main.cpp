
#include "utility.h"
#include "down_loder.h"

/*
 * 功能实现
 * 题目：假设我们在全国 33 个省市各有两台主机( 有外网ip )，
 * 每台机器上都可以通过 ip 直接下载 pingcap.tar.gz
 * 这个包有 2g (如http://192.168.1.1/pingcap.tar.gz)
 *
 * 设计一个系统，让每个地区的人能使用最快的速度下载到这个包
 *
 * 要求：
 * 下载速度要尽可能的快
 * 要考虑到容错
 *
 * 注意代码可读性，添加必要的注释
 * 注意代码风格与规范，添加必要的单元测试和文档
 * 注意异常处理，尝试优化性能
 *
 */


int main(){

    vector<vector<int>> v = init_provinces_neighbor_ralation();

//        for (int i=0;i<v.size();i++){
//            for (int j=0;j<v[i].size();j++){
//                cout<<v[i][j]<<" ";
//            }
//            cout<<endl;
//       }

    vector<ProvinceServerNode> po(PROVINCE_NUM);

    for (int i=0;i< PROVINCE_NUM ;i++){
        po[i].id=i;
    }

    get_province_access_order(v,1,po);

    vector<ProvinceServerState> pss;
    for (int i=0;i< PROVINCE_NUM ;i++){
        ProvinceServerState p;
        p.ps=po[i];
        for (int j=0;j<MACHINE_NUM;j++){
             p.freeMachinesIndex.push(j);
             MachineNode m(p.ps.id,j,id2ip(p.ps.id,j));
             p.machines.push_back(m);
        }
        p.aliveNum=p.freeMachinesIndex.size();
        p.freeNum=p.freeMachinesIndex.size();
        pss.push_back(p);
    }

//    for (int i=0;i< PROVINCE_NUM ;i++){
//        cout<<po[i].id<<" "<< po[i].order<<endl;
//    }

    //设置日志级别
    log_level=INFO;

    DownLoder d1;
    string url="pingcap.tar.gz";
    d1.initDownLoder(pss);
    d1.DownLoad(url);


    return 0;
}





