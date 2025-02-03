#include "elevetor.h"

Elevetor::Elevetor() {

    ele = nullptr;
    nowstair = 1;
    endstair = 1;
    inmove = false;
    isopen_door = false;
    isupdate = false;
    anime = nullptr;



    checktime.setInterval(200);
    //变量没有封装，所以不需要提供额外的启动定时器等函数
    connect(&checktime,&QTimer::timeout,this,&Elevetor::checkCurrentFloor);
}

void Elevetor::checkCurrentFloor()
{
    QPoint nowpos = ele->pos();
    //这nowpos是从顶部算的，导致电梯顶部一进2楼就变成了2楼，但我们要的是电梯底部还在1楼范围时应当算1楼
    nowstair = 11 - (nowpos.y()/60);

}

/*
电梯的思路：

电梯存在内部请求和外部请求。内部请求指的是在电梯里按数字，外部请求指的是在电梯外按上下按钮。

当外部请求按下时，检查哪一台电梯可以接，接的标准是“停顿or正在朝当前方向运行”，如果都不能接（都朝着反方向运行），则挂起请求，保留在“外部等待库”
如果哪一台电梯可以接，立刻将此请求分配到对应的“电梯库”，并计算是否要修改目的地

当电梯内部按下请求（这里要考虑两台电梯都按照此逻辑执行），
检查当前是否顺路，如果顺路加入当前电梯的“执行库”， 否则加入当前电梯的“等待库”。当执行库添加成功，立刻计算是否要修改目的地。

当电梯内部按下已按下的请求，发出删除请求，此时检查外面按钮是否亮起，作为是否删除数组中数据的标志
如果当前目标就是要删除的请求，则无视此删除操作，这样便避免了部分冲突
这里的删除不要和电梯到达目的地的删除操作一起使用

当电梯内部不存在任何请求时，一旦接收到第一个请求，立刻开始运行。
当电梯处理完毕执行库，理论上，“等待库”所有的数字应当都在电梯当前位置的其中一个方向，否则应该在电梯运行时处理
但是这里还需要考虑“外部等待库”，理论上，外部等待库应该也在另一个方向（不确定），否则应该在电梯运行时处理

当电梯执行库减少一个数据时，应该执行删除操作，当删除完毕时，将外部等待库和等待库添加入当前电梯执行库

当电梯执行库处理完毕时，电梯可能正在开门关门，此时若是相对等待库的另一方向发出请求，是否该先响应它们？理论上不该，因为相对于等待库，它们都是后来的

*/
//假设此函数用来响应电梯内部操作
void Elevetor::add_stair(int newstair){

    if(action_list.size()>0){
        //添加的楼层与电梯运行在同一路径
        if((newstair > nowstair && action_list[0] > nowstair)||(newstair < nowstair && action_list[0] < nowstair)){
            if(action_list.contains(newstair)){
                return;
            }
            action_list.push_back(newstair);
            std::sort(action_list.begin(),action_list.end());
        }else{
            wait_list.push_back(newstair);
        }
    }else{
        //电梯没有行动列表
        action_list.push_back(newstair);
    }
}

