#include "elevetor.h"

Elevetor::Elevetor() {

    ele = nullptr;
    nowstair = 1;
    endstair = 1;
    direct = 0;//0不动1上2下
    inmove = false;
    isopen_door = false;
    isupdate = false;
    anime = nullptr;

    //先设为nullptr
    up_out = nullptr;
    down_out = nullptr;

    checktime.setInterval(200);

    //变量没有封装，所以不需要提供额外的启动定时器等函数
    connect(&checktime,&QTimer::timeout,this,&Elevetor::checkCurrentFloor);
}

void Elevetor::checkCurrentFloor()
{
    QPoint nowpos = ele->pos();
    //这nowpos是从顶部算的，导致电梯顶部一进2楼就变成了2楼，但我们要的是电梯底部还在1楼范围时应当算1楼
    nowstair = 11 - (nowpos.y()/60);

    //检查当前路径上有无需要停下的地方，此函数只会在运行时产生

    //这里发射的信号要么是2，要么就是0，不可能出现1，因为1是出现在电梯停止状态的
    //所以get_out返回1的情况要在“mainwindow按下按钮”时触发，届时返回值根据add_stair返回情况决定（但它已经不用这里处理了）。
    if(get_out() == 2){
        emit send_update(*this);
    }
}

//检查当前路径上有无需要停下的地方（处理外部请求）
//返回值：0，什么都不做，1，立即执行运行操作，2，计算是否要修改目的地
int Elevetor::get_out()
{
    if(direct == 0){
        if(down_out->size()>0 && nowstair > down_out->at(down_out->size() - 1)){
            direct = direct + 2;
            return add_stair(down_out->at(down_out->size() - 1));
        }
        if(up_out->size()>0 && nowstair < up_out->at(0)){
            direct = direct + 1;
            return add_stair(up_out->at(0));
        }
        if(direct == 3){//说明满足以上两种情况，需要作出比较来选择目的地
            if((nowstair - down_out->at(down_out->size() - 1)) > (up_out->at(0) - nowstair)){
                direct = 1;
                return add_stair(up_out->at(0));
            }else{
                direct = 2;
                return add_stair(down_out->at(down_out->size() - 1));
            }
        }
        return 0;
    }
    else if(direct == 1){
        if(up_out->size()>0){
            //检查是否存在一个值等于当前楼层+1，如果存在立刻添加并修改目的地
            if(up_out->contains(nowstair + 1)){

                up_out->erase(
                    std::remove(up_out->begin(),up_out->end(),nowstair + 1),
                    up_out->end()
                    );

                action_list.push_back(nowstair + 1);
                endstair = nowstair + 1;
                return 2;
            }
        }
    }
    else if(direct == 2){
        if(down_out->size()>0){
            //检查是否存在一个值等于当前楼层+1，如果存在立刻添加并修改目的地
            if(down_out->contains(nowstair - 1)){

                down_out->erase(
                    std::remove(down_out->begin(),down_out->end(),nowstair - 1),
                    down_out->end()
                    );
                action_list.push_back(nowstair - 1);
                endstair = nowstair - 1;
                return 2;
            }
        }
    }
    return 0;
}

//添加一个数据在外部电梯
int Elevetor::add_out(int newstair,int direct_)
{
    if(direct_ == 1){
        up_out->push_back(newstair);
        std::sort(up_out->begin(),up_out->end());
        if(up_out->size() > 0){
            return get_out();
        }
    }else{
        down_out->push_back(newstair);
        std::sort(down_out->begin(),down_out->end());
        if(down_out->size() > 0){
            return get_out();
        }
    }
}

/*
电梯的思路：

电梯存在内部请求和外部请求。内部请求指的是在电梯里按数字，外部请求指的是在电梯外按上下按钮。

当外部请求按下时，检查哪一台电梯可以接，接的标准是“停顿or正在朝当前方向运行”，如果都不能接（都朝着反方向运行），则挂起请求，保留在“外部等待库”（这里不对，外部等待库不应该直接添加）
如果哪一台电梯可以接，立刻将此请求分配到对应的“电梯库”，并计算是否要修改目的地

当电梯内部按下请求（这里要考虑两台电梯都按照此逻辑执行），
检查当前是否顺路，如果顺路加入当前电梯的“执行库”， 否则加入当前电梯的“等待库”。当执行库添加成功，立刻计算是否要修改目的地。

当电梯内部按下已按下的请求，发出删除请求，此时检查外面按钮是否亮起，作为是否删除数组中数据的标志
如果当前目标就是要删除的请求，则无视此删除操作，这样便避免了部分冲突
这里的删除不要和电梯到达目的地的删除操作一起使用

当电梯内部不存在任何请求时，一旦接收到第一个请求，立刻开始运行。
当电梯处理完毕执行库，理论上，“等待库”所有的数字应当都在电梯当前位置的其中一个方向，否则应该在电梯运行时处理
但是这里还需要考虑“外部等待库”，理论上，外部等待库应该也在另一个方向（不确定），否则应该在电梯运行时处理

当电梯执行库减少一个数据时，应该执行删除操作，当删除完毕时，将外部等待库和等待库添加入当前电梯执行库（这里不对，外部等待库不应该直接添加）

当电梯执行库处理完毕时，电梯可能正在开门关门，此时若是相对等待库的另一方向发出请求，是否该先响应它们？理论上不该，因为相对于等待库，它们都是后来的

*/
//假设此函数用来响应电梯内部操作
//返回值：0，什么都不做，1，立即执行运行操作，2，计算是否要修改目的地
//理论上不应该在这里接收外部请求
int Elevetor::add_stair(int newstair){

    if(action_list.size()>0){
        //添加的楼层与电梯运行在同一路径
        //这里我接受了“本层”请求
        if((direct == 1 && newstair > nowstair)||(direct == 2 && newstair <= nowstair)){
            /*
            if(action_list.contains(newstair)){
                return 0;
            }*/
            action_list.push_back(newstair);
            std::sort(action_list.begin(),action_list.end());

            return 2;
        }else{
            //这里就不排序了，没必要
            wait_list.push_back(newstair);
            return 0;
        }
    }else{
        //电梯没有行动列表
        action_list.push_back(newstair);
        endstair = newstair;
        if(endstair > nowstair){
            direct = 1;
        }else{
            direct = 2;
        }
        return 1;
    }
}

//取消用的函数，默认只删除一个值，这个取消只能在电梯内部操作，所以检查当前两个数组即可
//此函数需要与外部配合使用
//1,取消成功，2，取消失败
int Elevetor::cancel_stair(int this_stair)
{
    if(endstair == this_stair){
        //拒绝取消
        return 2;
    }
    int index = action_list.indexOf(this_stair);
    if(index != -1){
        action_list.remove(index,1);
        return 1;
    }else {
        index = wait_list.indexOf(this_stair);
        if(index != -1){
            wait_list.remove(index,1);
            return 1;
        }
        return 2;
    }
}

//这个函数应该在电梯抵达某楼层后调用，用来删除action中所有数据，这里应该不会涉及别的东西，我要返回当前剩余size，用来判断是否需要掉头
int Elevetor::del_stair(int this_stair)
{
    action_list.erase(
        std::remove(action_list.begin(),action_list.end(),this_stair),
        action_list.end()
        );
    return action_list.size();
}

//这个函数返回一个值，列表中距离nowstair最近的值，当值不存在时返回0
//这里只要比较第一个值和最后一个值即可
int Elevetor::get_next_end()
{
    if(action_list.size()>0){
        if(nowstair < action_list[0]){
            return action_list[0];
        }
        else if(nowstair > action_list[action_list.size() - 1]){
            return action_list[action_list.size() - 1];
        }
    }
    return 0;
}

//通常这个函数在action全部执行完毕后进行如此操作
//执行完这个函数应当立即检查“外部等待库”
int Elevetor::wait_to_action()
{
    action_list.swap(wait_list);
    //在这里进行初次排序，然后立即开始反向运行：
    if(action_list.size() > 0){
        std::sort(action_list.begin(),action_list.end());
    }
    //重新设置航向，这里反向应该是没错的
    if(action_list.size() > 0){
        if(direct == 1){
            direct = 2;
        }else{
            direct = 1;
        }
        return 1;
    }else{
        direct = 0;
        return 0;
    }
}




























