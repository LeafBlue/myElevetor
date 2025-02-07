#ifndef ELEVETOR_H
#define ELEVETOR_H
#include<QWidget>
#include<QPropertyAnimation>
#include<QPushButton>
#include<QTimer>
#include<QObject>
#include<QVector>
#include<algorithm>
#include<QMap>
#include<QStyle>
#include<QMutex>


/*
电梯的思路：

电梯存在内部请求和外部请求。内部请求指的是在电梯里按数字，外部请求指的是在电梯外按上下按钮。

当外部请求按下时，这里需要改改思路。
当外部请求按下时，我会优先安排空闲电梯去接，
（这个做法其实是不对的，因为很可能另一台电梯顺路，如果这台顺路的电梯先到达，会导致对请求“视而不见”，因为此时，外部请求已经被删除，分配给了电梯的等待库）

对于外部请求的实时响应
当电梯启动后，我会设置一个定时器，每200毫秒更新一次当前楼层，并且检测电梯“当前层的next”是否存在顺路的外部请求，如果存在，立刻中断动画，修改目的地。
为什么这样设计？因为我考虑到，我不应该立刻将外部请求分配到某个电梯。假设有人在8要求往上走，而两台电梯都顺路的话，我们很可能不知道哪台电梯会先到达，
假如在当前状态下A电梯距离更近，我们也需要考虑A电梯被其他楼层截停导致B电梯先接到的情况。

当电梯内部按下请求（这里要考虑两台电梯都按照此逻辑执行），
检查当前是否顺路，如果顺路加入当前电梯的“执行库”， 否则加入当前电梯的“等待库”。当执行库添加成功，立刻计算是否要修改目的地。

当电梯内部按下已按下的请求，发出删除请求
如果当前目标就是要删除的请求，则无视此删除操作，这样便避免了部分冲突
如果取消操作时，还同时存在外部请求，这里我们也做了处理——没有限制相同数据插入数组，这样的好处是取消时只会删除一个数据。

与取消不同，到达目的地，会将所有的当前楼层数据都删除

这种取消操作不能用在外部电梯，因为内部电梯的单个请求可能属于一个人，而外部电梯的请求并不属于一个人，你不想坐电梯了，别人还想坐。

当电梯内部不存在任何请求时，一旦接收到第一个请求，立刻开始运行。

当电梯处理完毕执行库，理论上，“等待库”所有的数字应当都在电梯当前位置的其中一个方向，否则应该在电梯运行时处理
但是这里还需要考虑“外部等待库”，理论上，外部等待库应该也在另一个方向（不确定），否则应该在电梯运行时处理

当电梯执行库减少一个数据时，应该执行删除操作，当删除完毕时，将等待库添加入当前电梯执行库
如果等待库没有数据，电梯会停止，这时候电梯会检测是否存在尚未处理的外部请求，并决定是否处理
（这里很明显存在一个bug，如果电梯接受了某个外部请求，这个外部请求会转入电梯内部执行库，导致另一台电梯（假如正在运行）路过时，对请求“视而不见”）

当电梯执行库处理完毕时，电梯可能正在开门关门，此时若是相对等待库的另一方向发出请求，是否该先响应它们？理论上不该，因为相对于等待库，它们都是后来的请求
*/

class Elevetor:public QObject
{
    Q_OBJECT
public:
    explicit Elevetor(QObject *parent = nullptr);
    ~Elevetor();


    //这是两个指针，创建的两个对象内部的这指针将会指向同一个区域
    //这个区域用来存储外部请求组，达到两个对象内部数据同步的结果
    //这个指针指向的部分是共享的，在析构函数处理时需要注意不要造成重复析构

    //共享数组
    QVector<int> *up_out;
    QVector<int> *down_out;

    //电梯内部按钮
    QMap<int,QPushButton*> btn_map;

    //共享按钮
    QMap<int,QPushButton*>* outbtn_up;
    QMap<int,QPushButton*>* outbtn_down;

    //电梯箱
    QWidget *ele;
    //当前楼层
    int nowstair;
    //当前目标
    int endstair;
    //当前运行方向
    int direct;
    //状态
    //1，静止，3，运行动画中，4，开门动画中，2，暂停中（这个状态认为在开门后等待关门时期），5关门动画中
    int status;

    //当前电梯使用的通用动画
    QPropertyAnimation *anime;

    //定时器，每200毫秒触发一次
    QTimer checktime;

    //当前电梯执行库和等待库
    QVector<int> action_list;
    QVector<int> wait_list;

    //保护共享资源，加个锁
    QMutex mutex1;


    void checkCurrentFloor();
    int add_stair(int newstair);
    int cancel_stair(int this_stair);
    int del_stair(int this_stair);
    int get_next_end();
    int wait_to_action();

    int add_out(int newstair,int direct_);
    int del_out(int newstair,int direct_);
    int checkout();
    int out_to_inside(int newstair,int direct_);
    int stop_checkout();

signals:
    void send_update(Elevetor& thisele);
};

#endif // ELEVETOR_H
