#ifndef ELEVETOR_H
#define ELEVETOR_H
#include<QWidget>
#include<QPropertyAnimation>
#include<QTimer>
#include<QObject>
#include<QVector>
#include<algorithm>

class Elevetor:public QObject
{
    Q_OBJECT
public:
    Elevetor();
    void checkCurrentFloor();
    void add_stair(int newstair);

    QWidget *ele;
    int nowstair;
    int endstair;
    bool inmove;
    bool isopen_door;
    bool isupdate;
    QPropertyAnimation *anime;
    QTimer checktime;

    QVector<int> action_list;
    QVector<int> wait_list;
};

#endif // ELEVETOR_H
