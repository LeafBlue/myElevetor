#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),ele_A(),ele_B()
{
    ele_A.up_out = ele_B.up_out = new QVector<int>();
    ele_A.down_out = ele_B.down_out = new QVector<int>();

    ele_A.outbtn_up = ele_B.outbtn_up = new QMap<int,QPushButton*>();
    ele_A.outbtn_down = ele_B.outbtn_down = new QMap<int,QPushButton*>();


    move_v = 16;//假设60像素需1000毫秒

    setWindow();
    setcenter();

    //接收信号
    connect(&ele_A,&Elevetor::send_update,this,&MainWindow::update_endstair);
    connect(&ele_B,&Elevetor::send_update,this,&MainWindow::update_endstair);
}

MainWindow::~MainWindow() {
    delete ele_A.up_out;
    delete ele_A.down_out;
    delete ele_A.outbtn_up;
    delete ele_A.outbtn_down;
}


//------------------------------------------------------------------------绘制界面 开始------------------------------------------------------------------------
void MainWindow::setWindow()
{
    resize(1000,720);
}
/*设置大外框架*/
void MainWindow::setcenter()
{
    QWidget *centerwidget = new QWidget(this);
    setCentralWidget(centerwidget);

    QHBoxLayout *layout = new QHBoxLayout(centerwidget);
    layout->setContentsMargins(0,0,0,0);

    for(int i = 0; i < 5; i++){
        QVBoxLayout *layout1 = new QVBoxLayout();

        if(i==0){
            setinsidebtn(layout1,1);
        }else if(i==4){
            setinsidebtn(layout1,2);
        }
        else if(i ==1){
            setstairs(layout1,1);
        }
        else if(i ==3){
            setstairs(layout1,2);
        }
        else if(i == 2){
            setstairsbtn(layout1);
        }
        layout->addLayout(layout1,1);
    }
}

/*
绘制电梯内部按钮面板，并设置事件
*/
void MainWindow::setinsidebtn(QVBoxLayout *p_layout,int place)//1左2右
{
    QVBoxLayout *layout_up = new QVBoxLayout();
    QVBoxLayout *layout_down = new QVBoxLayout();
    p_layout->addLayout(layout_up,3);
    p_layout->addLayout(layout_down,1);

    QWidget *place_wid = new QWidget();
    place_wid->setContentsMargins(0,0,0,0);
    //place_wid->setStyleSheet("border:1px solid black;");
    layout_up->addWidget(place_wid);

    QWidget *btn_widget = new QWidget();
    btn_widget->setStyleSheet("border:1px solid black;background-color:grey;");
    layout_down->addWidget(btn_widget);


    QVBoxLayout *layoutbtn = new QVBoxLayout(btn_widget);
    for(int i = 0;i < 5; i++){
        QHBoxLayout *layout_row = new QHBoxLayout();
        for(int j = 0;j < 3; j++){
            int stair = 12 - (i * 3 + j);
            QPushButton *btn1 = new QPushButton(btn_widget);

            btn1->setFixedWidth(40);
            btn1->setFixedHeight(40);
            layout_row->addWidget(btn1);


            btn1->setStyleSheet("QPushButton { background-color: silver;color:black;font-size:20px;font-weight: bold; }"
                                "QPushButton:hover { background-color: darkgrey;color:red;font-size:20px;font-weight: bold; }");

            if(stair>0){
                btn1->setText(QString::number(stair));
                btn1->setProperty("pressedState","false");
                //为数字按钮单独设置样式
                btn1->setStyleSheet("QPushButton { background-color: silver;color:black;font-size:20px;font-weight: bold; }"
                                    "QPushButton[pressedState = \"true\"]{background-color: yellow;color:black;font-size:20px;font-weight: bold;}"
                                    "QPushButton[pressedState = \"false\"]{background-color: silver;color:black;font-size:20px;font-weight: bold;}"
                                    "QPushButton:hover { background-color: darkgrey;color:red;font-size:20px;font-weight: bold; }");

                if(place == 1){
                    ele_A.btn_map.insert(stair,btn1);
                    connect(btn1,&QPushButton::clicked,[this,stair,btn1](){
                        add_newstair(stair,this->ele_A,btn1);
                    });
                }else{
                    ele_B.btn_map.insert(stair,btn1);
                    connect(btn1,&QPushButton::clicked,[this,stair,btn1](){
                        add_newstair(stair,this->ele_B,btn1);
                    });
                }


            }else if(stair == 0){
                btn1->setText(QObject::tr(">|<"));
                connect(btn1,&QPushButton::clicked,[place,this](){
                    if(place == 1){
                        closedoor(ele_A);
                    }else{
                        closedoor(ele_B);
                    }
                });

            }else if(stair == -1){
                btn1->setText(QObject::tr("铃"));
            }else if(stair == -2){
                btn1->setText(QObject::tr("<|>"));
                connect(btn1,&QPushButton::clicked,[place,this](){
                    if(place == 1){
                        opendoor(ele_A);
                    }else{
                        opendoor(ele_B);
                    }
                });
            }
        }
        layoutbtn->addLayout(layout_row);
    }

}

/*
 绘制楼层界面
*/
void MainWindow::setstairs(QVBoxLayout *p_layout,int place)
{

    QWidget *big_stair = new QWidget();
    p_layout->addWidget(big_stair);

    initele(big_stair,place);

    for (int i = 0; i < 12; ++i) {
        QWidget *stair_widget = new QWidget(big_stair);
        stair_widget->move(0,60 * i);
        stair_widget->resize(190,60);
        stair_widget->setStyleSheet("border:2px solid grey;");
    }
}


/*
设置电梯外每一层 ↑ ↓ 按钮
*/
void MainWindow::setstairsbtn(QVBoxLayout *p_layout)
{

    QWidget *big_stair = new QWidget();
    p_layout->addWidget(big_stair);

    for (int i = 0; i < 12; ++i) {
        QWidget *stair_widget = new QWidget(big_stair);
        stair_widget->resize(190,60);
        stair_widget->move(0,60 * i);
        stair_widget->setStyleSheet("background-color:lightgrey;");


        QHBoxLayout *layout = new QHBoxLayout(stair_widget);
        //设置上按钮
        QPushButton *btn1 = new QPushButton();
        btn1->setFixedSize(40,40);
        btn1->setStyleSheet("QPushButton { background-color: silver;color:black;font-size:25px;font-weight: bold; }"
                            "QPushButton[pressedState = \"true\"]{background-color: yellow;color:black;font-size:25px;font-weight: bold;}"
                            "QPushButton[pressedState = \"false\"]{background-color: silver;color:black;font-size:25px;font-weight: bold;}"
                            "QPushButton:hover { background-color: darkgrey;color:red;font-size:25px;font-weight: bold; }");
        btn1->setText(QObject::tr("↑"));
        btn1->setProperty("pressedState","false");
        ele_A.outbtn_up->insert(12 - i,btn1);//将按钮存入共享指针
        connect(btn1,&QPushButton::clicked,[this,i,btn1](){
            add_outstair(12 - i,btn1,1);
        });

        //设置楼层显示
        QLabel *label = new QLabel();
        label->setStyleSheet("border:none;font-size:25px;");
        label->setText(QString::number(12 - i) + QObject::tr(" 楼"));

        //设置下按钮
        QPushButton *btn2 = new QPushButton();
        btn2->setFixedSize(40,40);
        btn2->setStyleSheet("QPushButton { background-color: silver;color:black;font-size:25px;font-weight: bold; }"
                            "QPushButton[pressedState = \"true\"]{background-color: yellow;color:black;font-size:25px;font-weight: bold;}"
                            "QPushButton[pressedState = \"false\"]{background-color: silver;color:black;font-size:25px;font-weight: bold;}"
                            "QPushButton:hover { background-color: darkgrey;color:red;font-size:25px;font-weight: bold; }");
        btn2->setText(QObject::tr("↓"));
        btn2->setProperty("pressedState","false");
        ele_A.outbtn_down->insert(12 - i,btn2);//将按钮存入共享指针
        connect(btn2,&QPushButton::clicked,[this,i,btn2](){
            add_outstair(12 - i,btn2,2);
        });

        //添加元素进界面
        layout->addWidget(btn1);
        layout->addWidget(label);
        layout->addWidget(btn2);
    }
}

/*
初始化电梯箱，为开门关门做准备
*/
void MainWindow::initele(QWidget *big_stair,int place)//1A2B
{
    if(place == 1){
        ele_A.ele = new QWidget(big_stair);
        ele_A.ele->resize(180,60);
        ele_A.ele->setStyleSheet("border:solid 2px darkcyan;background-color:white;");
        ele_A.ele->move(5,(12 - ele_A.nowstair) * 60);

        QWidget *A_left = new QWidget(ele_A.ele);
        A_left->resize(90,60);
        A_left->move(0,0);
        A_left->setStyleSheet("border:none;background-color:silver");

        QWidget *A_right = new QWidget(ele_A.ele);
        A_right->resize(90,60);
        A_right->move(90,0);
        A_right->setStyleSheet("border:none;background-color:silver");
    }else if(place == 2){
        ele_B.ele = new QWidget(big_stair);
        ele_B.ele->resize(180,60);
        ele_B.ele->move(5,(12 - ele_B.nowstair) * 60);
        ele_B.ele->setStyleSheet("border:solid 2px darkcyan;background-color:white;");

        QWidget *B_left = new QWidget(ele_B.ele);
        B_left->resize(90,60);
        B_left->move(0,0);
        B_left->setStyleSheet("border:none;background-color:silver");

        QWidget *B_right = new QWidget(ele_B.ele);
        B_right->resize(90,60);
        B_right->move(90,0);
        B_right->setStyleSheet("border:none;background-color:silver");
    }
}
//------------------------------------------------------------------------绘制界面 结束------------------------------------------------------------------------

//------------------------------------------------------------------------运行逻辑 开始------------------------------------------------------------------------

//电梯发出内部请求
void MainWindow::add_newstair(int newstair, Elevetor &thisele,QPushButton *btn1)
{
    bool ispress = btn1->property("pressedState").toBool();
    if(ispress){
        int iscancel = thisele.cancel_stair(newstair);
        if(iscancel == 1){
            btn1->setProperty("pressedState","false");
            btn1->style()->unpolish(btn1);
            btn1->style()->polish(btn1);
            btn1->update();
        }
        return;
    }
    //变颜色
    btn1->setProperty("pressedState","true");
    btn1->style()->unpolish(btn1);
    btn1->style()->polish(btn1);
    btn1->update();

    int add_result = thisele.add_stair(newstair);
    if(add_result == 1){
        thisele.status = 3;
        move_ele(thisele);
    }else if(add_result == 2){
        //理论上这里不需要修改状态
        if(thisele.direct == 1 && newstair < thisele.endstair){
            thisele.endstair = newstair;
            update_endstair(thisele);
        }else if(thisele.direct == 2 && newstair > thisele.endstair){
            thisele.endstair = newstair;
            update_endstair(thisele);
        }
    }
}

void MainWindow::arrive_stair(Elevetor &thisele)
{
    thisele.anime->deleteLater();
    //停止定时器
    thisele.checktime.stop();

    //找到对应按钮修改颜色
    QPushButton* thisbtn = thisele.btn_map[thisele.endstair];
    thisbtn->setProperty("pressedState","false");
    thisbtn->style()->unpolish(thisbtn);
    thisbtn->style()->polish(thisbtn);
    thisbtn->update();
    //修改外部按钮的颜色，首先检查同方向的，如果同方向的不需要修改则修改相反方向的
    //但这里并不要修改方向，通常我们认为向上运行处理向下请求后，需要根据电梯内部请求处理，而不是立刻就往下开始走，都不知道要去哪。
    if(thisele.direct == 1){
        QPushButton* outbtn = thisele.outbtn_up->value(thisele.endstair);
        bool ispress = outbtn->property("pressedState").toBool();
        if(ispress){
            outbtn->setProperty("pressedState","false");
            outbtn->style()->unpolish(outbtn);
            outbtn->style()->polish(outbtn);
            outbtn->update();
        }else{
            outbtn = thisele.outbtn_down->value(thisele.endstair);
            outbtn->setProperty("pressedState","false");
            outbtn->style()->unpolish(outbtn);
            outbtn->style()->polish(outbtn);
            outbtn->update();
        }

    }else{
        QPushButton* outbtn = thisele.outbtn_down->value(thisele.endstair);
        bool ispress = outbtn->property("pressedState").toBool();
        if(ispress){
            outbtn->setProperty("pressedState","false");
            outbtn->style()->unpolish(outbtn);
            outbtn->style()->polish(outbtn);
            outbtn->update();
        }else{
            outbtn = thisele.outbtn_up->value(thisele.endstair);
            outbtn->setProperty("pressedState","false");
            outbtn->style()->unpolish(outbtn);
            outbtn->style()->polish(outbtn);
            outbtn->update();
        }
    }

    thisele.status = 4;//开门动画中
    thisele.nowstair = thisele.endstair;
    //删除对应数组（外部请求数组会先删除再移动到内部，不用刻意删除）
    int over_size = thisele.del_stair(thisele.endstair);
    if(over_size==0){
        //重新设置方向，但并不开始移动，需要等待关门函数处理完毕
        thisele.wait_to_action();
    }

    //电梯停靠1秒后开始执行开门动画
    QTimer::singleShot(1000,this,[this,&thisele](){
        opendoor(thisele);
    });
}

//执行完这个函数后，认为一个流程结束
void MainWindow::after_closedoor(Elevetor &thisele)
{
    //一秒后关门，此阶段认为是未关门阶段
    QTimer::singleShot(1000,this,[this,&thisele](){
        int thenext = thisele.get_next_end();
        if(thenext!=0){
            thisele.status = 3;
            move_ele(thisele);
        }else{
            //在此处对外部请求进行检查，等下写
            thisele.status = 1;
            if(thisele.stop_checkout() == 1){
                thisele.status = 3;
                move_ele(thisele);
            }
        }
    });
}

//当按下外部请求时，执行这个函数，
void MainWindow::add_outstair(int newstair, QPushButton *btn1, int direct_)
{
    bool ispress = btn1->property("pressedState").toBool();
    if(ispress){
        //不允许多次请求，也不允许取消请求
        return;
    }
    //变颜色
    btn1->setProperty("pressedState","true");
    btn1->style()->unpolish(btn1);
    btn1->style()->polish(btn1);
    btn1->update();

    int addresult = ele_B.add_out(newstair,direct_);
    if(addresult == 1){
        //需找一台空电梯去接
        if(ele_A.status == 1 && ele_B.status == 1){
            if(abs(ele_A.nowstair - newstair) > abs(ele_B.nowstair - newstair)){
                ele_B.out_to_inside(newstair,direct_);
                ele_B.endstair = newstair;
                ele_B.status = 3;
                move_ele(ele_B);
            }else{
                ele_A.out_to_inside(newstair,direct_);
                ele_A.endstair = newstair;
                ele_A.status = 3;
                move_ele(ele_A);
            }
        }
        else if(ele_A.status == 1){
            //只有A是静止的，启用A电梯
            ele_A.out_to_inside(newstair,direct_);
            ele_A.endstair = newstair;
            ele_A.status = 3;
            move_ele(ele_A);
        }else if(ele_B.status == 1){
            ele_B.out_to_inside(newstair,direct_);
            ele_B.endstair = newstair;
            ele_B.status = 3;
            move_ele(ele_B);
        }
    }
    //其他情况无需处理，等待计时器处理
}

//------------------------------------------------------------------------运行逻辑 结束------------------------------------------------------------------------

//------------------------------------------------------------------------动画相关 开始------------------------------------------------------------------------

void MainWindow::opendoor(Elevetor &thisele)
{
    if(thisele.status != 4){
        return;
    }
    QList<QWidget*> widlist = thisele.ele->findChildren<QWidget*>();
    QWidget *left = widlist[0];
    QWidget *right = widlist[1];

    QPropertyAnimation *anim_left = new QPropertyAnimation(left,"geometry");
    anim_left->setDuration(1000);
    anim_left->setStartValue(QRect(0,0,thisele.ele->width()/2,thisele.ele->height()));
    anim_left->setEndValue(QRect(0,0,0,thisele.ele->height()));

    QPropertyAnimation *anim_right = new QPropertyAnimation(right,"geometry");
    anim_right->setDuration(1000);
    anim_right->setStartValue(QRect(thisele.ele->width()/2,0,thisele.ele->width()/2,thisele.ele->height()));
    anim_right->setEndValue(QRect(thisele.ele->width(),0,0,thisele.ele->height()));

    QParallelAnimationGroup *group_anime = new QParallelAnimationGroup(this);
    group_anime->addAnimation(anim_left);
    group_anime->addAnimation(anim_right);

    connect(group_anime,&QParallelAnimationGroup::finished,[this,&thisele](){
        thisele.status = 2;

        QTimer::singleShot(3000,this,[this,&thisele](){
            thisele.status = 5;
            closedoor(thisele);
        });
    });
    group_anime->start();
}


void MainWindow::closedoor(Elevetor &thisele)
{
    if(thisele.status != 5){
        return;
    }
    QList<QWidget*> widlist = thisele.ele->findChildren<QWidget*>();
    QWidget *left = widlist[0];
    QWidget *right = widlist[1];

    QPropertyAnimation *anim_left = new QPropertyAnimation(left,"geometry");
    anim_left->setDuration(1000);
    anim_left->setStartValue(QRect(0,0,0,thisele.ele->height()));
    anim_left->setEndValue(QRect(0,0,thisele.ele->width()/2,thisele.ele->height()));

    QPropertyAnimation *anim_right = new QPropertyAnimation(right,"geometry");
    anim_right->setDuration(1000);
    anim_right->setStartValue(QRect(thisele.ele->width(),0,0,thisele.ele->height()));
    anim_right->setEndValue(QRect(thisele.ele->width()/2,0,thisele.ele->width()/2,thisele.ele->height()));

    QParallelAnimationGroup *group_anime = new QParallelAnimationGroup(this);
    group_anime->addAnimation(anim_left);
    group_anime->addAnimation(anim_right);

    connect(group_anime,&QParallelAnimationGroup::finished,[this,&thisele](){
        after_closedoor(thisele);
    });

    group_anime->start();
}


void MainWindow::move_ele(Elevetor &thisele)
{
    int aspect = 0;//1上2下
    int steps = 0;
    if(thisele.nowstair < thisele.endstair){
        aspect = 1;
        steps = thisele.endstair - thisele.nowstair;
    }else if(thisele.nowstair > thisele.endstair){
        aspect = 2;
        steps = thisele.nowstair - thisele.endstair;
    }else{
        steps = 0;
    }
    QPoint nowpos = thisele.ele->pos();
    thisele.anime = new QPropertyAnimation(thisele.ele,"pos");
    thisele.anime->setDuration(60 * steps * move_v);
    thisele.anime->setStartValue(nowpos);
    if(aspect == 1){
        thisele.anime->setEndValue(QPoint(nowpos.x(),nowpos.y() - (60 * steps)));
    }else{
        thisele.anime->setEndValue(QPoint(nowpos.x(),nowpos.y() + (60 * steps)));
    }

    //此连接在动画意外stop后不会执行
    connect(thisele.anime,&QPropertyAnimation::finished,[this,&thisele](){
        arrive_stair(thisele);

    });

    thisele.anime->start();
    //启动定时器，每200毫秒更新一次楼层
    thisele.checktime.start();
}

//调用此函数时，要默认动画是进行状态
void MainWindow::update_endstair(Elevetor& thisele)
{
    if(thisele.status != 3){
        return;
    }
    QPoint newstart = thisele.ele->pos();
    int end_y  = (12 - thisele.endstair) * 60;
    int move_y = abs(newstart.y() - end_y);


    if(thisele.anime&&thisele.anime!=nullptr){
        //先断开连接，再执行stop，防止先执行stop触发原connect
        disconnect(thisele.anime,&QPropertyAnimation::finished,nullptr,nullptr);
        thisele.anime->stop();
        thisele.anime->deleteLater();
        thisele.anime = nullptr;
    }

    thisele.anime = new QPropertyAnimation(thisele.ele,"pos");
    //move_v 每像素移动所需时间
    thisele.anime->setDuration(move_v * move_y);
    thisele.anime->setStartValue(newstart);
    QPoint endpoint(newstart.x(),end_y);
    thisele.anime->setEndValue(endpoint);

    connect(thisele.anime,&QPropertyAnimation::finished,[this,&thisele](){
        arrive_stair(thisele);
    });
    thisele.anime->start();
}

//------------------------------------------------------------------------动画相关 结束------------------------------------------------------------------------


/*
//此函数在处理每一层的楼层信息以及中断操作上，操作简单，逻辑令人清晰舒适
//但是动画效果不尽如人意，每一层都会卡顿一下，故不得不抛弃此函数
void MainWindow::do_anime(Elevetor &thisele,int steps,int aspect)
{
    if(steps==0){
        return;
    }
    if(thisele.isupdate){
        //目的地修改，中止当前动画，重新开始一个新的动画
        move_ele(thisele);
        return;
    }
    QPoint nowpos = thisele.ele->pos();
    QPropertyAnimation *anime = new QPropertyAnimation(thisele.ele,"pos");
    anime->setDuration(1000);
    anime->setStartValue(nowpos);
    if(aspect == 1){
        anime->setEndValue(QPoint(nowpos.x(),nowpos.y() - 60));
        ++thisele.nowstair;
    }else{
        anime->setEndValue(QPoint(nowpos.x(),nowpos.y() + 60));
        --thisele.nowstair;
    }
    qDebug()<<thisele.nowstair;

    --steps;
    connect(anime,&QPropertyAnimation::finished,[this,anime,&thisele,steps,aspect](){
        anime->deleteLater();
        do_anime(thisele,steps,aspect);
    });
    anime->start();
}
*/













