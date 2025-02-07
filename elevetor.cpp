#include "elevetor.h"

Elevetor::~Elevetor()
{
    delete ele;
    delete anime;
}
Elevetor::Elevetor(QObject *parent)
    : QObject(parent) {

    //有三个容器不需要初始化

    ele = nullptr;
    nowstair = 1;
    endstair = 1;
    direct = 0;//0不动1上2下
    status = 1;//1，静止，2，暂停中，3，运行动画中，4，开门动画中，5关门动画中

    anime = nullptr;

    //先设为nullptr
    up_out = nullptr;
    down_out = nullptr;

    outbtn_up = nullptr;
    outbtn_down = nullptr;

    checktime.setInterval(200);

    //变量没有封装为private，所以不需要提供额外的启动定时器等函数
    connect(&checktime,&QTimer::timeout,this,&Elevetor::checkCurrentFloor);
}

/*
定时器
*/
void Elevetor::checkCurrentFloor()
{
    QPoint nowpos = ele->pos();
    //这nowpos是从顶部算的，导致电梯顶部一进2楼就变成了2楼，但我们要的是电梯底部还在1楼范围时应当算1楼
    if(nowpos.y() % 60 == 0){
        nowstair = 12 - (nowpos.y()/60);
    }else{
        nowstair = 11 - (nowpos.y()/60);
    }
    //检查当前路径上有无需要停下的地方，此函数只会在运行时产生
    if(checkout() == 1){
        emit send_update(*this);
    }
}
//------------------------------------------------------------------------外部请求相关数组 开始------------------------------------------------------------------------
//实时检查外部请求，这个函数用在运行时的定时器中，所以需要尽可能简单
int Elevetor::checkout()
{
    if(direct == 1){
        if(up_out->size()>0){
            //检查是否存在一个值等于当前楼层+1，如果存在立刻添加并修改目的地
            if(up_out->contains(nowstair + 1)){

                up_out->erase(
                    std::remove(up_out->begin(),up_out->end(),nowstair + 1),
                    up_out->end()
                    );

                action_list.push_back(nowstair + 1);
                std::sort(action_list.begin(),action_list.end());
                endstair = nowstair + 1;
                return 1;
            }
        }
    }else if(direct == 2){
        if(down_out->size()>0){
            //检查是否存在一个值等于当前楼层-1，如果存在立刻添加并修改目的地
            if(down_out->contains(nowstair - 1)){
                down_out->erase(
                    std::remove(down_out->begin(),down_out->end(),nowstair - 1),
                    down_out->end()
                    );
                action_list.push_back(nowstair - 1);
                std::sort(action_list.begin(),action_list.end());
                endstair = nowstair - 1;
                return 1;
            }
        }
    }
    return 0;
}

//在电梯进入静止状态后，对外部请求进行检查，查看有没有适合自己处理的请求
//用锁锁死，防止两个对象同时处理
int Elevetor::stop_checkout()
{
    if(up_out->size() > 0 && down_out->size() > 0){
        if(abs(up_out->at(0) - nowstair) > abs(down_out->at(0) - nowstair)){
            //处理向下请求
            endstair = down_out->at(0);
            out_to_inside(down_out->at(0),2);
            return 1;
        }else{
            endstair = up_out->at(0);
            out_to_inside(up_out->at(0),2);
            return 1;
        }
    }
    else if(up_out->size() > 0){
        endstair = up_out->at(0);
        out_to_inside(up_out->at(0),1);
        return 1;
    }else if(down_out->size() > 0){
        endstair = down_out->at(0);
        out_to_inside(down_out->at(0),2);
        return 1;
    }
    return 0;
}

//当外部数组元素可以作为目标值时，将其设为endstair，并添加入内部数组
//此函数涉及到具体对象
//操作此函数时，电梯有可能在运行，有可能不在运行
//direct_ 首次出现此参数，这个参数和成员变量direct不一样，它表示的是当前传入的请求是向上还是向下
int Elevetor::out_to_inside(int newstair,int direct_)
{
    endstair = newstair;
    if(direct_ == 1){
        up_out->erase(
            std::remove(up_out->begin(),up_out->end(),newstair),
            up_out->end()
            );
        action_list.push_back(newstair);
        std::sort(action_list.begin(),action_list.end());
    }else{
        down_out->erase(
            std::remove(down_out->begin(),down_out->end(),newstair),
            down_out->end()
            );
        action_list.push_back(newstair);
        std::sort(action_list.begin(),action_list.end());
    }
    if(status == 3){
        //需要修改目的地
        return 2;
    }else if(status == 1){
        //无需修改目的地，但需要启动去寻找目标
        return 1;
    }else{
        //通常不会出现此情况
        return 0;
    }
}

//添加一个数据到外部数组
//此函数只会操作共享数据，因此无所谓哪个对象调用
//当添加的数据返回为1时，证明当前添加入了一个新的，很可能电梯未在运行，需结合电梯状态进行分配
//这里考虑不对内部数据进行排序，以得到第一个是最先插入的数据
int Elevetor::add_out(int newstair,int direct_)
{
    size_t ask_size = 0;
    if(direct_ == 1){
        up_out->push_back(newstair);
        //std::sort(up_out->begin(),up_out->end());
        return up_out->size();
    }else{
        down_out->push_back(newstair);
        //std::sort(down_out->begin(),down_out->end());
        return down_out->size();
    }
    return 0;
}

//删除外部数组的一个数据
int Elevetor::del_out(int newstair, int direct_)
{
    if(direct_ == direct){
        if(direct_ == 1){
            up_out->erase(
                std::remove(up_out->begin(),up_out->end(),newstair),
                up_out->end()
                );
            return 1;
        }else{
            down_out->erase(
                std::remove(down_out->begin(),down_out->end(),newstair),
                down_out->end()
                );
            return 1;
        }
    }
    return 0;
}
//------------------------------------------------------------------------外部请求相关数组 结束------------------------------------------------------------------------


//------------------------------------------------------------------------内部请求相关数组 开始------------------------------------------------------------------------

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
            //由于direct是1或者2，这里不可能存在status=1
            if(status == 3){
                return 2;
            }else{
                return 0;//不操作，等待电梯自己关门处理
            }
        }else{
            //这里就不排序了，没必要
            wait_list.push_back(newstair);
            return 0;
        }
    }else{
        //电梯没有行动列表
        action_list.push_back(newstair);
        endstair = newstair;
        if(status == 1){
            if(endstair > nowstair){
                direct = 1;
            }else{
                direct = 2;
            }
            return 1;
        }
    }
    return 0;
}

//取消用的函数，默认只删除一个值，这个取消只能在电梯内部操作，所以检查当前两个数组即可
//此函数需要与外部配合使用
//1,取消成功，2，取消失败
int Elevetor::cancel_stair(int this_stair)
{
    if(endstair == this_stair&&status == 3){
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
            endstair = action_list[0];
            return action_list[0];
        }
        else if(nowstair > action_list[action_list.size() - 1]){
            endstair = action_list[action_list.size() - 1];
            return action_list[action_list.size() - 1];
        }
    }
    return 0;
}

//通常这个函数在action全部执行完毕后进行如此操作
//需要注意的是，执行完这个函数可能导致一直在等待的外部等待库无人处理
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

//------------------------------------------------------------------------内部请求相关数组 结束------------------------------------------------------------------------


























