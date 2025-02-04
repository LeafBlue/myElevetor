#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QWidget>
#include<QLayout>
#include<QPushButton>
#include<QString>
#include<QLabel>
#include<QList>
#include<QVector>
#include<QPropertyAnimation>
#include<QParallelAnimationGroup>
#include<QPoint>
#include"elevetor.h"
#include<cmath>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    void setWindow();
    void setcenter();
    void setinsidebtn(QVBoxLayout *layout,int place);
    void setstairs(QVBoxLayout *p_layout,int place);
    void setstairsbtn(QVBoxLayout *p_layout);
    void initele(QWidget *big_stair,int place);
    void opendoor(Elevetor &thisele);
    void closedoor(Elevetor &thisele);
    void move_ele(Elevetor &thisele);
    void update_endstair(Elevetor &thisele);

    void add_newstair(int newstair,Elevetor &thisele,QPushButton *btn1);

private:


    QVector<int> v_A;
    QVector<int> v_B;

    Elevetor ele_A;
    Elevetor ele_B;


    int move_v;//每一像素移动所需时间


};
#endif // MAINWINDOW_H
