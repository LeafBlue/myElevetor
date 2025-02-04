#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),ele_A(),ele_B()
{
    ele_A.up_out = ele_B.up_out = new QVector<int>();
    ele_A.down_out = ele_B.down_out = new QVector<int>();



    move_v = static_cast<int>(50/3);//假设60像素需1000毫秒

    setWindow();
    setcenter();


    //接收信号
    connect(ele_A,&Elevetor::send_update,this,&MainWindow::update_endstair);
    connect(ele_B,&Elevetor::send_update,this,&MainWindow::update_endstair);
}

MainWindow::~MainWindow() {}

void MainWindow::setWindow()
{
    resize(1000,720);
}

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
                }else{
                    ele_B.btn_map.insert(stair,btn1);
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



            //验证按钮功能
            /*
            connect(btn1, &QPushButton::clicked, [btn1]() {
                qDebug() << "Button clicked:" << btn1->text();
            });

            */
        }
        layoutbtn->addLayout(layout_row);
    }

}

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
        QPushButton *btn1 = new QPushButton();
        btn1->setFixedSize(40,40);

        btn1->setStyleSheet("QPushButton { background-color: silver;color:black;font-size:25px;font-weight: bold; }"
                            "QPushButton:hover { background-color: darkgrey;color:red;font-size:25px;font-weight: bold; }");
        btn1->setText(QObject::tr("↑"));

        QLabel *label = new QLabel();
        label->setStyleSheet("border:none;font-size:25px;");
        label->setText(QString::number(12 - i) + QObject::tr(" 楼"));

        QPushButton *btn2 = new QPushButton();
        btn2->setFixedSize(40,40);

        btn2->setStyleSheet("QPushButton { background-color: silver;color:black;font-size:25px;font-weight: bold; }"
                            "QPushButton:hover { background-color: darkgrey;color:red;font-size:25px;font-weight: bold; }");
        btn2->setText(QObject::tr("↓"));

        layout->addWidget(btn1);
        layout->addWidget(label);
        layout->addWidget(btn2);
    }
}

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

void MainWindow::opendoor(Elevetor &thisele)
{
    if(thisele.isopen_door){
        return;
    }
    if(thisele.inmove){
        return;
    }
    QList<QWidget*> widlist = thisele.ele->findChildren<QWidget*>();
    QWidget *left = widlist[0];
    QWidget *right = widlist[1];

    QPropertyAnimation *anim_left = new QPropertyAnimation(left,"geometry");
    anim_left->setDuration(1500);
    anim_left->setStartValue(QRect(0,0,thisele.ele->width()/2,thisele.ele->height()));
    anim_left->setEndValue(QRect(0,0,0,thisele.ele->height()));

    QPropertyAnimation *anim_right = new QPropertyAnimation(right,"geometry");
    anim_right->setDuration(1500);
    anim_right->setStartValue(QRect(thisele.ele->width()/2,0,thisele.ele->width()/2,thisele.ele->height()));
    anim_right->setEndValue(QRect(thisele.ele->width(),0,0,thisele.ele->height()));

    QParallelAnimationGroup *group_anime = new QParallelAnimationGroup(this);
    group_anime->addAnimation(anim_left);
    group_anime->addAnimation(anim_right);

    connect(group_anime,&QParallelAnimationGroup::finished,[&thisele](){
        thisele.isopen_door = true;
    });
    group_anime->start();


}

void MainWindow::closedoor(Elevetor &thisele)
{
    if(!thisele.isopen_door){
        return;
    }
    if(thisele.inmove){
        return;
    }
    QList<QWidget*> widlist = thisele.ele->findChildren<QWidget*>();
    QWidget *left = widlist[0];
    QWidget *right = widlist[1];

    QPropertyAnimation *anim_left = new QPropertyAnimation(left,"geometry");
    anim_left->setDuration(1500);
    anim_left->setStartValue(QRect(0,0,0,thisele.ele->height()));
    anim_left->setEndValue(QRect(0,0,thisele.ele->width()/2,thisele.ele->height()));

    QPropertyAnimation *anim_right = new QPropertyAnimation(right,"geometry");
    anim_right->setDuration(1500);
    anim_right->setStartValue(QRect(thisele.ele->width(),0,0,thisele.ele->height()));
    anim_right->setEndValue(QRect(thisele.ele->width()/2,0,thisele.ele->width()/2,thisele.ele->height()));

    QParallelAnimationGroup *group_anime = new QParallelAnimationGroup(this);
    group_anime->addAnimation(anim_left);
    group_anime->addAnimation(anim_right);
    connect(group_anime,&QParallelAnimationGroup::finished,[&thisele](){
        thisele.isopen_door = false;
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
    connect(thisele.anime,&QPropertyAnimation::finished,[&thisele](){
        thisele.anime->deleteLater();
        thisele.checktime.stop();
        thisele.nowstair = thisele.endstair;
        thisele.inmove = false;
    });

    thisele.inmove = true;
    thisele.anime->start();
    //启动定时器，每200毫秒更新一次楼层
    thisele.checktime.start();
}

//调用此函数时，要默认动画是开始状态
void MainWindow::update_endstair(Elevetor &thisele)
{
    if(!thisele.inmove){
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

    connect(thisele.anime,&QPropertyAnimation::finished,[&thisele](){
        thisele.anime->deleteLater();
        thisele.checktime.stop();
        thisele.nowstair = thisele.endstair;
        thisele.inmove = false;
    });

    thisele.anime->start();
}

void MainWindow::add_newstair(int newstair, Elevetor &thisele,QPushButton *btn1)
{
    bool ispress = btn1->property("pressedState").toBool();
    if(ispress){
        return;
    }

    int add_result = thisele.add_stair(newstair);
    if(add_result == 1){
        move_ele(thisele);
    }else if(add_result == 2){
        if(thisele.inmove){
            if(thisele.direct == 1 && newstair < thisele.endstair){
                thisele.endstair = newstair;
                update_endstair(thisele);
            }else if(thisele.direct == 2 && newstair > thisele.endstair){
                thisele.endstair = newstair;
                update_endstair(thisele);
            }
        }
    }
}




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













