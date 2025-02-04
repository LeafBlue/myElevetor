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

class Elevetor:public QObject
{
    Q_OBJECT
public:
    Elevetor();
    void checkCurrentFloor();
    int add_stair(int newstair);
    int cancel_stair(int this_stair);
    int del_stair(int this_stair);
    int get_next_end();
    int wait_to_action();
    int get_out();

    int add_out(int newstair,int direct_);

    QMap<int,QPushButton*> btn_map;

    QWidget *ele;
    int nowstair;
    int endstair;
    int direct;
    bool inmove;
    bool isopen_door;
    bool isupdate;
    QPropertyAnimation *anime;
    QTimer checktime;

    QVector<int> action_list;
    QVector<int> wait_list;

    //这是两个指针，创建的两个对象内部的这指针将会指向同一个区域
    //这个区域用来存储外部请求组，达到两个对象内部数据同步的结果
    //这个指针指向的部分是共享的，在析构函数处理时需要注意不要造成重复析构
    QVector<int> *up_out;
    QVector<int> *down_out;

signals:
    void send_update(Elevetor& elevetor);
};

#endif // ELEVETOR_H
