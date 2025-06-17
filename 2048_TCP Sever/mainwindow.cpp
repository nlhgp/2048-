#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include "QGraphicsDropShadowEffect"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sever = new QTcpServer;//服务器对象
    sever->listen(QHostAddress::AnyIPv4,PORT);//监听
    setWindowTitle("服务器");
    //连接数据库
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("mygamer");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("159357");
    if(!db.open())//检测是否连接成功
    {
        //QMessageBox::information(this,"连接提示","连接成功");
        QMessageBox::warning(this,"连接提示","数据库连接失败！");
    }
    //信号与槽
    connect(sever,&QTcpServer::newConnection,this,&MainWindow::new_client_hander);//连接客户端
    //初始化房间数
    rooms.reserve(ROOM_MAX_SIZE);
    //初始化界面
    init_ui();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//发消息
void MainWindow::send_message(QString &message)
{

}


void MainWindow::init_ui()
{
    // ================== 主窗口样式 ==================
    this->setStyleSheet(
        "QMainWindow {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2c3e50, stop:1 #3498db);"
        "}"

        // ================== 全局字体 ==================
        "QLabel {"
        "   color: #ecf0f1;"
        "   font-family: 'Microsoft YaHei';"
        "}"

        // ================== 输入框样式 ==================
        "QLineEdit {"
        "   background: rgba(255,255,255,0.9);"
        "   border: 2px solid #2980b9;"
        "   border-radius: 8px;"
        "   padding: 6px 12px;"
        "   font-size: 14px;"
        "   color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #e74c3c;"
        "   background: #ffffff;"
        "}"

        // ================== 列表控件 ==================
        "QListWidget {"
        "   background: rgba(255,255,255,0.85);"
        "   border-radius: 10px;"
        "   padding: 8px;"
        "   font-size: 13px;"
        "   color: #34495e;"
        "   alternate-background-color: #f0f0f0;"
        "}"
        "QListWidget::item {"
        "   padding: 8px;"
        "   border-bottom: 1px solid #bdc3c7;"
        "}"
        "QListWidget::item:selected {"
        "   background: #3498db;"
        "   color: white;"
        "}"

        // ================== 标题样式 ==================
        "#title {"
        "   background: rgba(52, 152, 219, 0.8);"
        "   border-radius: 15px;"
        "   padding: 15px;"
        "   font-size: 24px !important;"
        "   font-weight: bold;"
        "   color: #ecf0f1;"
        "}"

        // ================== 状态栏 ==================
        "QStatusBar {"
        "   background: rgba(44, 62, 80, 0.9);"
        "   color: #bdc3c7;"
        "   font-size: 12px;"
        "}"
        );

    // ================== 布局优化 ==================

    // 调整布局间距
    ui->gridLayout->setContentsMargins(15, 15, 15, 15);
    ui->gridLayout_3->setVerticalSpacing(10);
    ui->gridLayout_2->setContentsMargins(0, 0, 0, 10);

    // 设置列表控件的交替行颜色
    ui->userlist->setAlternatingRowColors(true);
    ui->messagelist->setAlternatingRowColors(true);

    // ================== 动态效果 ==================
    // 添加输入框动画
    QGraphicsDropShadowEffect *inputEffect = new QGraphicsDropShadowEffect;
    inputEffect->setBlurRadius(15);
    inputEffect->setOffset(3);
    inputEffect->setColor(QColor(52, 152, 219, 150));
    ui->iplineEdit->setGraphicsEffect(inputEffect);
    ui->portlineEdit->setGraphicsEffect(inputEffect);
    ui->messageline->setGraphicsEffect(inputEffect);

    // 标题浮出效果
    QGraphicsOpacityEffect *titleOpacity = new QGraphicsOpacityEffect(ui->title);
    titleOpacity->setOpacity(0.95);
    ui->title->setGraphicsEffect(titleOpacity);

    // ================== 字体优化 ==================
    QFont titleFont("Microsoft YaHei", 18, QFont::Bold);
    ui->title->setFont(titleFont);

    QFont listFont("Segoe UI", 11);
    ui->userlist->setFont(listFont);
    ui->messagelist->setFont(listFont);

    // ================== 高级样式 ==================
    // 自定义滚动条
    QString scrollBarStyle =
        "QScrollBar:vertical {"
        "   border: none;"
        "   background: rgba(200,200,200,50);"
        "   width: 10px;"
        "   margin: 0px 0 0px 0;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: rgba(52, 152, 219, 150);"
        "   border-radius: 4px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "}";
    ui->userlist->setStyleSheet(scrollBarStyle);
    ui->messagelist->setStyleSheet(scrollBarStyle);
}

//添加信息记录
void MainWindow::add_item(QString &m)
{
    ui->messagelist->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));//添加消息时间
    ui->messagelist->addItem(m.toUtf8());
    //ui->messagelist->addItem("");//空一行，不然挤着难看
    ui->messagelist->scrollToBottom();
}

//发送全服消息
void MainWindow::public_message(QString &m,QStringList &sl)
{
    int i;
    m=sl[2]+=" 说：";
    for(i=3;i<sl.size();++i)
    {
        m+=sl[i];
    }
    fstl(this->mts)
    {
        if((*it)->socket->peerAddress().toString()!=sl[0])
            (*it)->socket->write(m.toUtf8());
    }
}

//发送私聊消息
void MainWindow::private_message(QString &m,QStringList &sl)
{
    int i;
    m=sl[2] + " 对你说：";
    for(i=4;i<sl.size();++i)
    {
        m+=sl[i];
    }
    fstl(this->mts)
    {
        if((*it)->socket->peerAddress().toString() == sl[3])
        {
            qDebug() << "**" << m;
            (*it)->socket->write(m.toUtf8());
        }
    }
}

//ip#sql#login#password#...
//172.17.15.220#sql#login#user123#select password from users where username = 'user1';
//处理登录账号信息
void MainWindow::login_text(QStringList &sl)
{
    QString message="login#";
    QSqlQuery query;

    QString sql=QString("select password_hash from user where username = '%1';").arg(sl[3]);
    qDebug() << sql;
    if(query.exec(sql))
    {
        if(query.next()==false)//没找到账号
        {
            message += "00";//00，未找到账号
        }
        else if(query.value(0)==sl[4])//找到了，传回密码
        {
            message += "01";//01，密码正确，登录成功
            ui->userlist->addItem(sl[3]);
        }
        else
        {
            message += "02";//02，密码错误
        }
    }
    //回话
    fstl(this->mts)
    {
        if(sl[0]==(*it)->socket->peerAddress().toString())
        {
            qDebug() << message;
            ((*it)->socket->write((message).toUtf8()));
        }
    }
}

//处理注册账号消息
void MainWindow::enroll_text(QStringList &sl)
{
    QString message="enroll#";
    QSqlQuery query;
    //ip#sql#enroll#check#sql语句
    if(sl[3]=="check")//检查账号是否已注册
    {
        if(query.exec(sl[4]))//运行sql语句
        {
            if(query.next()!=false)//找到账号了
            {
                message += "00";//00,账号已注册
            }
            else message += "01";//账号未注册
        }
    }
    //ip#sql#enroll#账号#密码#sql语句
    else
    {
        if(query.exec(sl[4]))
        {
            message += "02";//注册成功
        }
        else
        {
            QString sql=QString("select * from user where id=%1;#").arg(sl[3]);
            if(query.exec(sql))
            {
                if(query.next()!=false)
                    message += "00";//00,账号已注册
            }
        }
    }
    fstl(this->mts)
    {
        if(sl[0]==(*it)->socket->peerAddress().toString())
        {
            qDebug() << message;
            ((*it)->socket->write((message).toUtf8()));
        }
    }
}

//处理修改信息
void MainWindow::alter_text(QStringList &sl)
{
    QSqlQuery query;
    QString message = "alter#";
    //ip#sql#alter/init#账号#sql语句
    if(sl[2] == "init")
    {
        message = "init#";
        if(query.exec(sl[4]))//初始化组件
        {
            while(query.next())
            {
                message += query.value(1).toString()+"#";//用户名
                if(query.value(3).toString().length()==0) message += "null#";
                else message+=query.value(3).toString()+"#";
                if(query.value(4).toString().length()==0) message += "null#";
                else message+=query.value(4).toString()+"#";
                if(query.value(5).toString().length()==0) message += "null";
                else message+=query.value(5).toString();
                //init#username#s_id#email#phone
            }
        }
    }
    else if(sl[2]=="alter")
    {
        if(query.exec(sl[4]))
        {
            message += "00";//修改成功
        }
        else message += "01";
    }
    fstl(this->mts)
    {
        if(sl[0]==(*it)->socket->peerAddress().toString())
        {
            ((*it)->socket->write((message).toUtf8()));
            break;
        }
    }
    qDebug() <<message;
}

//建立新线程
void MainWindow::new_client_hander()
{
    //建立tcp连接，连接也是对象，类型为socket
    QTcpSocket *socket = sever->nextPendingConnection();
    socket->peerAddress();//获取客户端地址
    socket->peerPort();//获取客户端端口号
    ui->iplineEdit->setText(socket->peerAddress().toString());
    ui->portlineEdit->setText(QString::number(socket->peerPort()));
    // //收到消息socket会发送readysocket信号
    // connect(socket,&QTcpSocket::readyRead,this,&Widget::message);

    //启动线程
    my_thread *t=new my_thread;
    t->get_socket(socket);
    t->start();
    mts.push_back(t);
    connect(t,&my_thread::message,this,&MainWindow::threadslot);
    connect(t,&my_thread::create_room,this,&MainWindow::create_room);
    connect(t,&my_thread::join_room,this,&MainWindow::join_room);
}

//接收线程传来的信号
void MainWindow::threadslot(QString m)
{
    qDebug() << m;
    QStringList sl=m.split('#');//将字符串以#分割
    if(sl[1]=="sql")
    {
        //ip#sql#login#password#...
        if(sl[2]=="login")
            this->login_text(sl);

        //ip#sql#enroll#check#sql语句
        //ip#sql#enroll#账号#密码#sql语句
        if(sl[2]=="enroll")
            this->enroll_text(sl);

        //ip#sql#alter#账号#sql语句
        if(sl[2]=="alter"||sl[2]=="init")
            this->alter_text(sl);

        //ip#sql#score#账号#分数值
        if(sl[2]=="score")
            this->score_text(sl);
    }
    else if(sl[1]=="public")
    {
        this->public_message(m,sl);
    }
    else if(sl[1]=="private")
    {
        this->private_message(m,sl);
    }
    else if(sl[1]=="get_rooms")
    {
        this->send_rooms(sl);
    }
    else if(sl[1]=="deconned")//发送下线信息
    {
        deconnected(sl);
    }
    else if(sl[1]=="rank")
    {
        get_rank(sl);
    }
    else if(sl[1]=="achievements") achievements_text(sl);
    else if(sl[1]=="achievement") achievement_text(sl);
    ui->messageline->setText(m.toUtf8());
    add_item(m);
}

//断开连接
void MainWindow::deconnected(QStringList &sl)
{
    QList<my_thread*>::iterator p;
    fstl(this->mts)
    {
        if((*it)->get_ip()==sl[0])
        {
            p=it;continue;
        }
        if((*it)->socket->state() == QAbstractSocket::ConnectedState)
        {
            (*it)->socket->write(((*it)->get_name()+"已下线").toUtf8());
        }
        else
        {
            for(int i = 0;i < ui->userlist->count();i++)
            {
                if((*it)->get_name() == ui->userlist->item(i)->text())
                {
                    QListWidgetItem* item = ui->userlist->item(i);
                    ui->userlist->removeItemWidget(item);
                    delete item;
                    break;
                }
                //qDebug() << ui->userlist->item(i)->text() << "#" << (*it)->get_name();
            }
        }
    }
    mts.erase(p);
    delete(*p);
}

//创建房间
void MainWindow::create_room(my_thread *p1)
{
    room *r=new room();
    r->set_id(rooms.size()+1);
    r->create_room(p1);
    rooms.push_back(r);
    //p1->socket->write( ("room_id#" + QString::number(r->get_id())).toUtf8() );
}

//发送房间列表
void MainWindow::send_rooms(QStringList &sl)
{
    QString s="rooms";
    for(int i=0;i<rooms.size();++i)
    {
        s+="#";
        s+=QString::number(rooms[i]->get_id());
        // s+="$";
        // s+=rooms[i]->get_name();
    }
    fstl(this->mts)
    {
        if(sl[0]==(*it)->socket->peerAddress().toString())
        {
            ((*it)->socket->write((s).toUtf8()));
            break;
        }
    }
}

//加入房间
void MainWindow::join_room(my_thread *p2,int id)
{
    if(id < 0 || id > rooms.length()) return;
    rooms[id-1]->join_room(p2);
}

//查看在线用户
void MainWindow::on_userButton_clicked()
{
    ui->userlist->clear();
    ui->userlabel->setText("当前用户：");
    for(int i=0;i<mts.size();++i)
    {
        ui->userlist->addItem(mts[i]->get_name());
    }
}

//查看在线房间
void MainWindow::on_roomsButton_clicked()
{
    ui->userlist->clear();
    ui->userlabel->setText("当前房间：");
    for(int i=0;i<rooms.size();++i)
    {
        ui->userlist->addItem(QString::number(rooms[i]->get_id()));
    }
}

//清除日志
void MainWindow::on_clearButton_clicked()
{
    ui->messagelist->clear();
}

//获取排名
void MainWindow::get_rank(QStringList &sl)
{
    QString sql = "SELECT username, highest_score FROM user ORDER BY highest_score DESC LIMIT 10";
    if(db.open())
    {
        QSqlQuery query(db);
        QJsonArray resultArray;
        //查询数据库获取json对象
        if(query.exec(sql)) {
            while(query.next()) {
                QJsonObject obj;
                obj.insert("username" , query.value(0).toString());
                obj.insert("highest_score" , query.value(1).toString());
                resultArray.append(obj);
            }
        }
        //自定义通讯协议
        QString message = "rank#" + QJsonDocument(resultArray).toJson();
        //回信息
        fstl(this->mts)
        {
            if(sl[0]==(*it)->socket->peerAddress().toString())
            {
                ((*it)->socket->write((message).toUtf8()));
                break;
            }
        }
    }
}

//处理得分信息
//ip#sql#score#账号#分数值
void MainWindow::score_text(QStringList &sl)
{
    QString sql = QString("CALL UpdateUserScoreSafe('%1',%2);").arg(sl[3]).arg(sl[4]);
    QSqlQuery query(db);
    query.exec(sql);
    //查询用户进行了几次游戏
    sql = QString("select run_time from user where username = '%1';").arg(sl[3]);
    if(query.exec(sql))
    {
        fstl(this->mts)
        {
            if(sl[0]==(*it)->socket->peerAddress().toString())
            {
                ((*it)->socket->write(("run_time#" + query.value(0).toString()).toUtf8()));
                break;
            }
        }
    }
    // if(query.exec(sql))
    //     qDebug() << sql;
}

//处理成就信息
//这里怀念一下java的数据库映射，QT做真tm烦
void MainWindow::achievements_text(QStringList &sl)
{
    if(sl.size() < 3) return;
    QString sql = QString("CALL GetUserAchievements('%1')").arg(sl[2]);
    QSqlQuery query(db);
    if(query.exec(sql))
    {
        QJsonArray resultArray;
        //查询数据库获取json对象
        if(query.exec(sql)) {
            while(query.next()) {
                QJsonObject obj;
                obj.insert("name" , query.value(0).toString());
                obj.insert("description" , query.value(1).toString());
                obj.insert("file_path" , query.value(2).toString());
                obj.insert("finish",query.value(3).toString());
                resultArray.append(obj);
            }
        }
        //自定义通讯协议
        QString message = "achievements#" + QJsonDocument(resultArray).toJson();
        //回信息
        fstl(this->mts)
        {
            if(sl[0]==(*it)->socket->peerAddress().toString())
            {
                ((*it)->socket->write((message).toUtf8()));
                break;
            }
        }
    }
}

//普通成就信息
void MainWindow::achievement_text(QStringList &sl)
{
    QString sql = QString("CALL UnlockAchievement('%1', %2);").arg(sl[2]).arg(sl[3]);
    qDebug() << sql;
    QSqlQuery query(db);
    if(query.exec(sql))
    {
        while(query.next()) qDebug() << "achievement#" << query.value(0);
    }
}
