#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QMovie>
#include <QPaintEvent>
#include <QPainter>
#include <qnetworkrequest.h>
#include <QNetworkReply>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket;//创建socket对象
    manager = new QNetworkAccessManager;
    m1.init();
    m2.init();

    /*
     * 初始化界面
     */
    init_ui();

    /*
     * 总控部分
     */
    connect(ui->hideButton,&QPushButton::clicked,this,&MainWindow::hide_message_layout);//隐藏对话框
    connect(timer,&QTimer::timeout,this,&MainWindow::finish_taunt);//计时器
    connect(timer1,&QTimer::timeout,this,&MainWindow::finish_be_taunt);
    connect(ui->settingButton,&QPushButton::clicked,this,&MainWindow::on_setButton_clicked);//转到设置
    ui->voiceSlider->setValue(30);//音效和音量默认最大
    ui->musicSlider->setValue(30);

    /*
     * 连接服务器
     */
    socket->connectToHost(QHostAddress(ip),port.toShort());
    connect(socket,&QTcpSocket::connected,this,&MainWindow::finish_link);

    /*
     * 账号处理
     */
    //登录账号
    connect(ui->id_line,&QLineEdit::returnPressed,this,&MainWindow::switch_line);
    connect(ui->password_line,&QLineEdit::returnPressed,this,&MainWindow::on_DownloadButton_clicked);
    connect(ui->loginButton,&QPushButton::clicked,this,&MainWindow::on_DownloadButton_clicked);
    connect(ui->EnrollButton,&QPushButton::clicked,this,&MainWindow::on_EnrollButton_clicked);//页面跳转
    connect(ui->canseebox,SIGNAL(stateChanged(int)),this,SLOT(switch_hidepassword()));
    //注册账号
    connect(ui->canseebox_2,SIGNAL(stateChanged(int)),this,SLOT(switch_hidepassword()));
    connect(ui->namelineEdit,&QLineEdit::returnPressed,this,&MainWindow::switch_line);
    connect(ui->passwordlineEdit,&QLineEdit::returnPressed,this,&MainWindow::switch_line);
    connect(ui->againlineEdit,&QLineEdit::returnPressed,this,&MainWindow::on_EnrollpushButton_clicked);
    connect(ui->FinishEnroll,&QPushButton::clicked,this,&MainWindow::on_EnrollpushButton_clicked);

    /*
     * 网络部分
     */
    //接收消息
    connect(socket,&QTcpSocket::readyRead,this,&MainWindow::recieve_message);//接收服务器信息
    //发消息
    connect(ui->sendButton,&QPushButton::clicked,this,&MainWindow::send_message);
    connect(ui->sendline,&QLineEdit::returnPressed,this,&MainWindow::send_message);

    /*
     * 设置页面
     */
    //启动道具模式
    // 连接信号与槽
    connect(ui->propBox, &QCheckBox::stateChanged, this, &MainWindow::on_propBox_checkStateChanged);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//初始化界面
void MainWindow::init_ui()
{
    setWindowTitle("2048客户端");
    //单人模式的deepseek对话框不可编辑
    ui->textEdit->setReadOnly(true);
    hide_message_layout();//隐藏对话框
    QMovie *movie1 = new QMovie(":/left.gif");
    ui->tauntlabel1->setMovie(movie1); // 设置要显示的GIF动画图片
    movie1->setScaledSize(ui->tauntlabel1->size());
    QMovie *movie2 = new QMovie(":/right.gif");
    ui->tauntlabel2->setMovie(movie2); // 设置要显示的GIF动画图片
    movie2->setScaledSize(ui->tauntlabel2->size());

    //背景音乐
    background_music = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(0.25f);
    background_music->setAudioOutput(audioOutput);
    //D:/QT/test/2048/是前缀，换成自己的
    background_music->setSource(QUrl(QString("D:/QT/test/2048/Elliot Hsu-Sunken Guardian.mp3")));
    background_music->play();
    background_music->setLoops(QMediaPlayer::Infinite);

    //音效
    music_effect = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(0.25f);
    music_effect->setAudioOutput(audioOutput);
    music_effect->setSource(QUrl::fromLocalFile(QString("D:/QT/test/2048/Effect.mp3")));

    //音游模式
    this->voice_timer = new QTimer();
    connect(voice_timer,&QTimer::timeout,this,&MainWindow::voice_timer_used);
    music_game = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(0.25f);
    music_game->setAudioOutput(audioOutput);
    music_game->setSource(QUrl::fromLocalFile(QString("D:/QT/test/2048/game_effect.mp3")));

    //嘲讽播放器
    effect=new QSoundEffect();
    effect->setSource(QUrl::fromLocalFile(":/taunt1.WAV"));
    effect->setLoopCount(1);  //循环次数
    effect->setVolume(0.25f); //音量  0~1之间
    timer = new QTimer;//计时器初始化
    //被嘲讽
    effect1=new QSoundEffect();
    effect1->setSource(QUrl::fromLocalFile(":/taunt2.WAV"));
    effect1->setLoopCount(1);  //循环次数
    effect1->setVolume(0.25f); //音量  0~1之间
    timer1  = new QTimer;

    //注册与登录页面ui设置
    ui->hideButton->setVisible(false);
    ui->textEdit->setVisible(false);
    ui->pushButton->setVisible(false);
    ui->cfButton->setVisible(false);
    ui->personButton->setVisible(false);
    ui->ipline->setText(this->ip);
    ui->portline->setText(this->port);
    ui->stackedWidget->setCurrentWidget(ui->loginpage);
    this->current_page=0;//current_page的值对应不同的页面
    //设置输入框为密码格式
    ui->password_line->setEchoMode(QLineEdit::Password);
    ui->againlineEdit->setEchoMode(QLineEdit::Password);
    ui->passwordlineEdit->setEchoMode(QLineEdit::Password);
    ui->selectRoomLine->setPlaceholderText("请输入房间号");

    ui->radioButton->setChecked(1);//ai默认为阿尔法贝塔剪枝
    ui->oneclickButton->setChecked(1);//音效默认为单击

    //设置样式表
    init_qss();
    styleAchievementList();//美化成就列表
}

//设置样式表
void MainWindow::init_qss()
{
    //登录页面修改
    ui->nolinkButton->setStyleSheet(
        "background-color: transparent;"
        "color: #2e7d32;"// 深绿色文字
        );
    //所有按钮和输入框样式修改
    ui->centralwidget->setStyleSheet(
        "QPushButton {"
        "border-radius: 10px;"
        "background-color: rgba(245, 245, 245, 200)"
        "}"

        " QLineEdit {"
        "   background: rgba(255, 255, 255, 180);" // 半透白底
        "   border: 2px solid rgba(127, 191, 143, 120);"
        "   border-radius: 6px;"
        "   padding: 8px 12px;"                  // 内边距
        //"   font-size: 12pt;"
        "   color: #004d40;"                     // 墨绿文字
        "   selection-color: #ffffff;"           // 选中文字颜色
        "   selection-background-color: #4caf50;"// 选中背景
        " }"

        " QLineEdit:focus {"
        "   border-color: #4caf50;"              // 聚焦边框
        " }"
        );

    /*
     * 登录页面
     */
    ui->loginFrame->setStyleSheet(
        "QFrame {"
        "background-color: rgba(245, 245, 245, 200);"
        "   border-radius: 10px;"   // 可选圆角
        "}"
        // 子控件样式
        " QLabel {"                 // 深绿色文字
        "   color: #2e7d32;"
        "   font-weight: bold;"
        "   font-size: 12pt;"                    // 字号统一
        "background-color: transparent;"
        " }"

        " QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x1:0, y1:1," // 渐变按钮
        "               stop:0 #81c784, stop:1 #4caf50);"
        "   border: none;"
        "   border-radius: 6px;"
        "   color: white;"
        "   padding: 10px 24px;"
        "   font-weight: bold;"
        " }"
        " QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x1:0, y1:1,"
        "               stop:0 #94d496, stop:1 #66bb6a);"
        " }"
        " QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x1:0, y1:1,"
        "               stop:0 #4caf50, stop:1 #388e3c);"
        " }"
        );

    /*
     * 注册页面
     */
    ui->enrollFrame->setStyleSheet(
        "QFrame {"
        "background-color: rgba(245, 245, 245, 200);"
        "   border-radius: 10px;"   // 可选圆角
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x1:0, y1:1," // 渐变按钮
        "               stop:0 #81c784, stop:1 #4caf50);"
        "   border: none;"
        "   border-radius: 6px;"
        "   color: white;"
        "   padding: 10px 24px;"
        "   font-weight: bold;"
        " }"
        " QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x1:0, y1:1,"
        "               stop:0 #94d496, stop:1 #66bb6a);"
        " }"
        " QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x1:0, y1:1,"
        "               stop:0 #4caf50, stop:1 #388e3c);"
        " }"
        " QLabel {"                 // 深绿色文字
        "   color: #2e7d32;"
        "   font-weight: bold;"
        "   font-size: 12pt;"                    // 字号统一
        "background-color: transparent;"
        " }"
        );
    ui->Title->setStyleSheet("color:#81c784");

    /*
     * 聊天窗口
     */
    ui->chatFrame->setStyleSheet(
        "QFrame#chatFrame {"
        "background-color: rgba(224, 255, 232, 220);"  /* 晨雾薄荷背景 */
        "border-radius: 12px;"
        "border: 1px solid rgba(127, 191, 143, 150);"
        "}"
        "QListWidget {"     /*消息列表样式*/
        "background-color: transparent;"
        "border: none;"
        "padding: 8px;"
        "outline: 0;"
        "font-family: 'Segoe UI';}"
        "QListWidget::item {"   /* 消息气泡通用样式 */
        "   padding: 8px;"
        "   border-bottom: 1px solid #bdc3c7;"
        "}"
    /* 发送按钮 */
        "QPushButton#sendButton {"
        "background: qlineargradient(x1:0, y1:0, x1:0, y1:1,"
        "stop:0 #81c784, stop:1 #4caf50);"
        "border: none;"
        "border-radius: 8px;"   //圆角
        "min-width: 40px;"
        "padding: 7px 15px;"    //长宽
        "color: white;"
        //"font-weight: bold;"  //字体
        "font-size: 10pt;"
        "}"
        "QPushButton#sendButton:hover {"
        "background: qlineargradient(x1:0, y1:0, x1:0, y1:1,"
        "stop:0 #94d496, stop:1 #66bb6a);"
        "}"
        "QPushButton#sendButton:pressed {"
        "background: qlineargradient(x1:0, y1:0, x1:0, y1:1,"
        "stop:0 #4caf50, stop:1 #388e3c);"
        "}"
    );

    /*
     * 全服排名
     */
    ui->tableWidget->setStyleSheet(
        "QTableWidget {"
        "    background-color: rgba(224, 255, 232, 180);"  // 半透晨雾绿背景
        "    border-radius: 10px;"
        "    border: 1px solid rgba(127, 191, 143, 150);"
        "    gridline-color: rgba(165, 214, 167, 80);"     // 浅绿色网格线
        "    font-family: 'Segoe UI';"
        "    font-size: 12pt;"
        "}"
        ""
        "QHeaderView::section {"                           // 表头样式
        "    background: qlineargradient(x1:0, y1:0, x1:0, y1:1,"
        "                stop:0 #c8e6c9, stop:1 #81c784);" // 垂直渐变
        "    color: #1b5e20;"                              // 深绿文字
        "    border: none;"
        "    padding: 12px;"
        "    font-weight: bold;"
        "    font-size: 13pt;"
        "    border-radius: 6px;"                          // 表头圆角
        "}"
        ""
        "QTableWidget::item {"                             // 单元格通用样式
        "    border-bottom: 1px solid rgba(165, 214, 167, 100);"
        "    color: #2e7d32;"                              // 中绿色文字
        "    padding: 10px 15px;"
        "}"
        ""
        "QTableWidget::item:selected {"                    // 选中状态
        "    background-color: rgba(76, 175, 80, 100);"    // 半透亮绿
        "    color: #fffde7;"
        "    border: 1px solid #4caf50;"
        "}"
        "QTableCornerButton::section {"                   // 角落按钮
        "    background: #81c784;"
        "    border-radius: 6px 0 0 0;"
        "}"
        "/* 滚动条样式 */"
        "QScrollBar:vertical {"
        "    width: 10px;"
        "    background: rgba(200, 214, 200, 80);"
        "    border-radius: 4px;"
        "}"
        ""
        "QScrollBar::handle:vertical {"
        "    background: #66bb6a;"
        "    border-radius: 4px;"
        "    min-height: 30px;"
        "}"
        "/* 鼠标悬停效果 */"
        "QTableWidget::item:hover {"
        "    background-color: rgba(169, 223, 191, 150);"  // 薄荷绿悬停
        "    border: 1px solid #4caf50;"
        "}");
}

//重写paintEvent函数来显示背景图片
void MainWindow::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(rect(),QPixmap(":/background.jpg"));
}

//连接主机成功
void MainWindow::finish_link()
{
    this->linked=true;
    ui->hideButton->setVisible(true);
    ui->textEdit->setVisible(true);
    ui->pushButton->setVisible(true);
    ui->cfButton->setVisible(true);
    ui->personButton->setVisible(true);
    //hide_message_layout();
    this->setMinimumSize(0, 0);
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

//隐藏对话框
void MainWindow::hide_message_layout()
{
    if(messageHide)
    {
        ui->chatFrame->hide();
        ui->hideButton->setText("显示对话框");
        messageHide = false;
    }
    else
    {
        ui->chatFrame->show();
        ui->hideButton->setText("隐藏对话框");
        messageHide = true;
    }
}

/*
 * 重写键盘事件
 *成就2048也在这里触发
*/
void MainWindow::keyPressEvent(QKeyEvent * event)
{
    static int ONE_TIME[13] = {300,400,300,300,700,500,700 , 400,400,300,400,400,600};
    static int point = 0;
    //按键音效
    if(this->voice_type == 0)
        music_effect->play();
    else
    {
        music_game->play();
        voice_timer->start(ONE_TIME[point]);
        point = (point+1)%13;
    }

    switch (event->key())
    {
        case Qt::Key_W:
        if(this->type)
            {
                if(!m1.up()) break;
                //新建方块并映射
                //新建道具
                if(m1.is_prop_mode()){m1.use_prop();m1.new_prop();}
                m1.new_two();

                if(this->type==3)
                {
                    //this->socket->write("moving#w"); 已废弃此类信息
                    link_move();
                }
                else if(this->type == 4)
                    alpha_beta_move();//阿尔法贝塔剪枝算法ai移动
                mapping();
            }break;
        case Qt::Key_A:
            if(this->type)
            {
                if(!m1.left()) break;
                //新建道具
                if(m1.is_prop_mode()){m1.use_prop();m1.new_prop();}
                m1.new_two();
                if(this->type==3){//联机模式，连接两方移动
                    //this->socket->write("moving#a");
                    link_move();
                }
                else if(this->type == 4)
                    alpha_beta_move();//阿尔法贝塔剪枝算法ai移动
                mapping();
            }break;
        case Qt::Key_S:
            if(this->type)
            {
                if(!m1.down()) break;
                //新建道具
                if(m1.is_prop_mode()){m1.use_prop();m1.new_prop();}
                m1.new_two();
                if(this->type==3){
                    //this->socket->write("moving#s");
                    link_move();
                }
                else if(this->type == 4)
                    alpha_beta_move();//阿尔法贝塔剪枝算法ai移动
                mapping();
            }break;
        case Qt::Key_D:
            if(this->type)
            {
                if(!m1.right()) break;
                //新建道具
                if(m1.is_prop_mode()){m1.use_prop();m1.new_prop();}
                m1.new_two();
                if(this->type==3) {
                    //this->socket->write("moving#d");
                    link_move();
                }
                else if(this->type == 4)
                    alpha_beta_move();//阿尔法贝塔剪枝算法ai移动
                mapping();
            }break;
        case Qt::Key_Up:
            if(this->type==2)
            {
                if(!m2.up()) break;
                m2.new_two();
                //新建道具
                if(m2.is_prop_mode()){m2.new_prop();}
                mapping();
            }break;
        case Qt::Key_Down:
            if(this->type==2)
            {
                if(!m2.down()) break;
                m2.new_two();
                //新建道具
                if(m2.is_prop_mode()){m2.new_prop();}
                mapping();
            }break;
        case Qt::Key_Left:
            if(this->type==2)
            {
                if(!m2.left()) break;
                m2.new_two();
                //新建道具
                if(m2.is_prop_mode()){m2.new_prop();}
                mapping();
            }break;
        case Qt::Key_Right:
            if(this->type==2)
            {
                if(!m2.right()) break;
                m2.new_two();
                //新建道具
                if(m2.is_prop_mode()){m2.new_prop();}
                mapping();
            }break;
        case Qt::Key_C://R和c两个键都能嘲讽
        case Qt::Key_R:
            if(this->type>0)
            {
                this->on_tauntButton_clicked();
            }break;
        case Qt::Key_0:
            if(this->type==2)
            {
                this->be_taunt();
            }break;
        default: break;
    }
    if(m1.falsed())
    {
        QMessageBox::StandardButton reply = QMessageBox::information(this,"游戏结束","玩家 "+username+" \n得分："+QString::number(m1.get_score()) + "\n是否重来？",QMessageBox::Yes | QMessageBox::No);
        socket->write(("sql#score#" + username + "#" + QString::number(m1.get_score())).toUtf8());
        if(reply == QMessageBox::Yes)
        {m1.init();m2.init();mapping();}
        else this->type=0;
    }
    if(m1.finished())
    {
        QMessageBox::StandardButton reply = QMessageBox::information(this,"恭喜通关","得分："+QString::number(m1.get_score()) + "\n是否重来？",QMessageBox::Yes | QMessageBox::No);
        socket->write(("sql#score#" + username + "#" + QString::number(m1.get_score())).toUtf8());
        if(reply == QMessageBox::Yes)
        {
            //qDebug() << "重开";
            m1.init();m2.init();mapping();
        }
        else this->type=0;
    }

    if(!_2048ed && m1.get_max_num() == 2048)
    {
        _2048ed = true;
        QMessageBox::information(this,"达成成就","2048带师\n第一次搓出2048方块");
        //成就列表achievements是复数有s，这个没有凹
        this->socket->write(("achievement#" + (username.length()==0?"2":username) + "#2").toUtf8());
    }

    if(m2.finished())
    {
        QMessageBox::StandardButton reply = QMessageBox::information(this,"恭喜通关","得分："+QString::number(m2.get_score()) + "\n是否重来？",QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {m1.init();m2.init();mapping();}
        else this->type=0;
    }
    if(m2.falsed())
    {
        QMessageBox::StandardButton reply = QMessageBox::information(this,"游戏结束","得分："+QString::number(m2.get_score()) + "\n是否重来？",QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {
            m1.init();m2.init();mapping();
        }
        else this->type=0;
    }
}

// 根据数字值返回对应的样式表
QString MainWindow::getTileStyle(int number) {
    switch(number) {
    case 0:
        return "background-color: #d6e0d0; color: transparent;";  // 空白格浅绿灰
    case 2:
        return "background-color: #e8f5e9; color: #2e7d32; font-size: 20pt;";      // 薄荷绿/深绿文字
    case 4:
        return "background-color: #c8e6c9; color: #1b5e20; font-size: 20pt;";       // 浅叶绿/墨绿文字
    case 8:
        return "background-color: #a5d6a7; color: #f9f6f2; font-size: 20pt;";       // 青瓷绿/米白文字
    case 16:
        return "background-color: #81c784; color: #f9f6f2; font-size: 20pt;";       // 翡翠绿
    case 32:
        return "background-color: #66bb6a; color: #f9f6f2; font-size: 20pt;";       // 活力绿
    case 64:
        return "background-color: #4caf50; color: #f9f6f2; font-size: 20pt;";       // 经典绿
    case 128:
        return "background-color: #43a047; color: #fffde7; font-size: 24pt;"; // 深丛林绿
    case 256:
        return "background-color: #388e3c; color: #fffde7; font-size: 24pt;";  // 松针绿
    case 512:
        return "background-color: #2e7d32; color: #fffde7; font-size: 24pt;";  // 常青藤绿
    case 1024:
        return "background-color: #1b5e20; color: #fffde7; font-size: 20pt;"; // 森林绿
    case 2048:
        return "background-color: #004d40; color: #fffde7; font-size: 20pt;";  // 深海绿
    case 1:
        return "background-image: url(:/lie.png);";//道具 消列
    case 3:
        return "background-image: url(:/hua.jpg);";//道具 消列
    case 5:
        return "background-image: url(:/hang.png);";//道具 消列
    default:
        return "background-color: #00251a; color: #fffde7; font-size: 18pt;";  // 极夜黑绿
    }
}//1消列 3炸一圈 5消行

//前后端映射
void MainWindow::mapping()
{
    ui->score_label->setText("Score:" + QString::number(m1.get_score()));
    auto setTileStyle = [this](QLabel* label, int num) {
        label->setStyleSheet(getTileStyle(num));
        label->setText(num ? QString::number(num) : "");
        label->setAlignment(Qt::AlignCenter);
    };
    if(this->type==1){
        setTileStyle(ui->label_1, m1.num(0,0));
        setTileStyle(ui->label_2, m1.num(0,1));
        setTileStyle(ui->label_3, m1.num(0,2));
        setTileStyle(ui->label_4, m1.num(0,3));
        setTileStyle(ui->label_5, m1.num(1,0));
        setTileStyle(ui->label_6, m1.num(1,1));
        setTileStyle(ui->label_7, m1.num(1,2));
        setTileStyle(ui->label_8, m1.num(1,3));
        setTileStyle(ui->label_9, m1.num(2,0));
        setTileStyle(ui->label_10, m1.num(2,1));
        setTileStyle(ui->label_11, m1.num(2,2));
        setTileStyle(ui->label_12, m1.num(2,3));
        setTileStyle(ui->label_13, m1.num(3,0));
        setTileStyle(ui->label_14, m1.num(3,1));
        setTileStyle(ui->label_15, m1.num(3,2));
        setTileStyle(ui->label_16, m1.num(3,3));
    }
    else
    {//玩家1
        setTileStyle(ui->label_33, m1.num(0,0));
        setTileStyle(ui->label_34, m1.num(0,1));
        setTileStyle(ui->label_35, m1.num(0,2));
        setTileStyle(ui->label_36, m1.num(0,3));
        setTileStyle(ui->label_37, m1.num(1,0));
        setTileStyle(ui->label_38, m1.num(1,1));
        setTileStyle(ui->label_39, m1.num(1,2));
        setTileStyle(ui->label_40, m1.num(1,3));
        setTileStyle(ui->label_41, m1.num(2,0));
        setTileStyle(ui->label_42, m1.num(2,1));
        setTileStyle(ui->label_43, m1.num(2,2));
        setTileStyle(ui->label_44, m1.num(2,3));
        setTileStyle(ui->label_45, m1.num(3,0));
        setTileStyle(ui->label_46, m1.num(3,1));
        setTileStyle(ui->label_47, m1.num(3,2));
        setTileStyle(ui->label_48, m1.num(3,3));
        //玩家2
        setTileStyle(ui->label_49, m2.num(0,0));
        setTileStyle(ui->label_50, m2.num(0,1));
        setTileStyle(ui->label_51, m2.num(0,2));
        setTileStyle(ui->label_52, m2.num(0,3));
        setTileStyle(ui->label_53, m2.num(1,0));
        setTileStyle(ui->label_54, m2.num(1,1));
        setTileStyle(ui->label_55, m2.num(1,2));
        setTileStyle(ui->label_56, m2.num(1,3));
        setTileStyle(ui->label_57, m2.num(2,0));
        setTileStyle(ui->label_58, m2.num(2,1));
        setTileStyle(ui->label_59, m2.num(2,2));
        setTileStyle(ui->label_60, m2.num(2,3));
        setTileStyle(ui->label_61, m2.num(3,0));
        setTileStyle(ui->label_62, m2.num(3,1));
        setTileStyle(ui->label_63, m2.num(3,2));
        setTileStyle(ui->label_64, m2.num(3,3));
    }
}

//连接服务器
void MainWindow::on_linkButton_clicked()
{
    //断开连接，防止报错
    socket->abort();
    socket->connectToHost(QHostAddress(ui->ipline->text()),ui->portline->text().toShort());

}

//切换到群聊模式
void MainWindow::on_cfButton_clicked()
{
    this->chat_type=1;//群发模式
    ui->iplabel->setText("服务器IP：");
    ui->linkButton->show();
}

//切换到私聊模式
void MainWindow::on_personButton_clicked()
{
        this->chat_type=2;//私聊模式
        ui->iplabel->setText("对方IP：");
        ui->linkButton->hide();
        this->socket->write("get_player_ip");
}

//获取对方ip
void MainWindow::set_player_ip(QStringList &sl)//设置对方ip
{
    ui->ipline->setText(sl[1]);
}

//发送文字消息
void MainWindow::send_message()
{
    QString chat=this->username;
    if(this->type == 0)
    {
        QMessageBox::information(this,"提示：","请先登录");
        return;
    }
    if(ui->sendline->text().length()!=0)
    {
        if(this->chat_type==1)//群发模式
        {
            chat+="：";
            chat+=ui->sendline->text();//处理聊天信息
            this->add_chat(chat);
            chat = "public#";
            chat+=this->username+"#";
            chat+=ui->sendline->text();
            //public#username#text
        }
        else if(this->chat_type==2)//私聊模式
        {
            chat += " 对 ";
            chat += ui->ipline->text();
            chat += " 说：";
            chat += ui->sendline->text();
            this->add_chat(chat);
            chat = "private#";
            chat += this->username + "#";
            chat += ui->ipline->text()+ "#";
            chat += ui->sendline->text();
            qDebug() << chat;
            //private#我方username#对方ip#信息
        }
        socket->write((chat).toUtf8());//发送信息
        ui->sendline->clear();
    }
}

//接收服务器信息
void MainWindow::recieve_message()
{
    QString message = socket->readAll();
    QStringList sl=message.split('#');
    QString chat;
    QString sign;
    if(sl.size()>1)
    {
        sign=sl[1];
        //qDebug() << sl[1];
    }
    if(sl[0]=="login")//login#00
    {
        if(sign=="00")
        {
            QMessageBox::information(this,"ERROR!","用户名未注册");
        }
        else if(sign=="01")
        {
            //QMessageBox::information(this,"","登陆成功！");
            this->type=-1;
            ui->stackedWidget->setCurrentWidget(ui->menuPage);
            //2 菜单
            this->current_page = 2;
            hide_message_layout();//隐藏对话框
            this->socket->write(("init#" + this->username).toUtf8());
        }
        else
            QMessageBox::information(this,"登陆失败","密码错误！");
    }
    else if(sl[0]=="enroll")
    {
        if(sign=="00")
        {
            QMessageBox::information(this,"ERROR!","用户名已占用\n请重新指定用户名");
        }
        else if(sign=="01")
        {
            QMessageBox::information(this,"","账号未注册\n该用户名可用");
        }
        else if(sign=="02")
        {
            QMessageBox::information(this,"","注册成功！");
            //1 注册
            this->current_page = 1;
            ui->stackedWidget->setCurrentWidget(ui->loginpage);
        }
        else
        {
            QMessageBox::information(this,"","注册失败");
        }
    }
    else if(sl[0]=="init"||sl[0]=="alter")
    {
        this->username=sl[1];
    }
    else if(sl[0]=="move")
    {
        recieve_move(sl);
    }
    else if(sl[0]=="rooms") recieve_rooms(sl);
    else if(sl[0]=="room_id") ui->roomIdLabel->setText("房间id：" + sl[1]);
    else if(sl[0]=="taunt") this->be_taunt();
    else if(sl[0]=="rank") this->recieve_rank(sl);
    else if(sl[0]=="player_ip") this->set_player_ip(sl);
    else if(sl[0]=="run_time")
    {
        if(sl.length()>1 && sl[1].toInt() >= 1) this->finish_fist_game();
    }
    else if(sl[0]=="achievements") this->init_achievements_items(sl);
    else
    {
        add_chat(message);
    }
    qDebug() << message;
}

/*
 * 登录账号部分
 */
//登录账号按钮
void MainWindow::on_DownloadButton_clicked()
{
    //获取lineedit数据
    this->username = ui->id_line->text();
    QString password = ui->password_line->text();
    this->ip=ui->ipline->text();
    this->port=ui->portline->text();

    if(this->type!=0)
    {
        socket->disconnectFromHost();
        socket->connectToHost(QHostAddress(ip),port.toShort());
    }
    // QString sql=QString("select password_hash from user where username = '%1';").arg(username);
    // QSqlQuery query;
    if(username.length()==0)
    {
        QMessageBox::information(this,"ERROR!","账号不能为空");
        return;
    }
    else if(password.length()==0)
    {
        QMessageBox::information(this,"ERROR!","密码不能为空");
        return;
    }

    //port->get_logintext(sql,ui->password_line->text(),ui->ipline->text());//向客户端传递信号
    //"sql#login#username#password"
    QString message="sql#login#";
    // if(this->type == 0)
    // {
    //     socket->abort();
    //     socket->connectToHost(QHostAddress(ip),port.toShort());
    // }
    message += username+"#";
    message += password + "#";
    //message += '\n';
    qDebug() << message;
    this->socket->write(message.toUtf8());
}

//注册时按下回车后切换输入框
void MainWindow::switch_line()
{
    //在账号框按回车就跳到密码框
    if(focusWidget()==ui->namelineEdit)
    {
        ui->passwordlineEdit->setFocus();
    }
    //在密码框按回车就跳到重复框
    else if(focusWidget()==ui->passwordlineEdit)
    {
        ui->againlineEdit->setFocus();
    }
    //登录时候
    else if(focusWidget()==ui->id_line)
    {
        ui->password_line->setFocus();
    }
}

//账号注册页面跳转
void MainWindow::on_EnrollButton_clicked()
{
    ui->id_line->clear();
    ui->password_line->clear();
    ui->stackedWidget->setCurrentWidget(ui->enrollpage);
    //1 注册
    this->current_page=1;
}

//添加聊天记录
void MainWindow::add_chat(QString &message)
{
    ui->chatlist->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));//添加消息时间
    ui->chatlist->addItem(message);//添加聊天记录
    ui->chatlist->scrollToBottom(); // 添加后滚动到底部
}

//隐藏密码
void MainWindow::switch_hidepassword()
{
    if(ui->canseebox_2->isChecked())
    {
        ui->passwordlineEdit->setEchoMode(QLineEdit::Normal);
        ui->againlineEdit->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->passwordlineEdit->setEchoMode(QLineEdit::Password);
        ui->againlineEdit->setEchoMode(QLineEdit::Password);
    }
    if(ui->canseebox->isChecked())
    {
        ui->password_line->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->password_line->setEchoMode(QLineEdit::Password);
    }
}

//检查账号是否已注册
void MainWindow::on_checkpushButton_clicked()
{
    QString sql;
    if(ui->namelineEdit->text().length()==0)
    {
        QMessageBox::information(this,"ERROR!","用户名不能为空");
    }
    if(ui->passwordlineEdit->text() != ui->againlineEdit->text())
    {
        QMessageBox::information(this,"ERROR!","两次输入密码不一致！");
    }
    sql=QString("select * from user where username=%1;#").arg(ui->namelineEdit->text());
    QString message="sql#enroll#";
    message += ui->namelineEdit->text() + "#" +ui->passwordlineEdit->text() + "#";
    message += sql;
    if(this->type==0)
    {
        this->ip = ui->ipline->text();
        socket->abort();
        socket->connectToHost(QHostAddress(ip),port.toShort());
    }
    //sql#enroll#check#sql语句
    //sql#enroll#账号#密码#sql语句
    //message += '\n';
    this->socket->write(message.toUtf8());
}

//注册账号
void MainWindow::on_EnrollpushButton_clicked()
{
    QString sql;
    this->type=0;
    //从密码文本框获取字符串
    QString password=ui->passwordlineEdit->text();
    QString name=ui->namelineEdit->text();
    this->username=name;
    if(ui->passwordlineEdit->text().length()==0)
    {
        QMessageBox::information(this,"ERROR!","密码不能为空");
        return;
    }
    sql=QString("insert into user(username,password_hash,total_score,highest_score) VALUES('%1','%2',0,0);").arg(name).arg(password);
    QString message="sql#enroll#";
    message += name + "#";
    message += sql;
    if(this->type==0)
    {
        this->ip=ui->ipline->text();
        socket->abort();
        socket->connectToHost(QHostAddress(ip),port.toShort());
    }
    //sql#enroll#check#sql语句
    //sql#enroll#账号#密码#sql语句
    this->socket->write(message.toUtf8());
}

//返回登录界面
void MainWindow::on_backButton_clicked()
{
    this->type=0;
    ui->stackedWidget->setCurrentWidget(ui->loginpage);
    this->current_page = 0;
}

//单机模式
void MainWindow::on_nolinkButton_clicked()
{
    this->type=1;
    mapping();
    ui->hideButton->setVisible(false);
    ui->stackedWidget->setCurrentWidget(ui->page);
    //3 单人模式
    this->current_page = 3;
    ui->textEdit->setVisible(false);
    ui->pushButton->setVisible(false);
    ui->cfButton->setVisible(false);
    ui->personButton->setVisible(false);
    ui->label->setText("单机模式");
}

//回到菜单
void MainWindow::on_menuButton_clicked()
{
    this->type=-1;
    ui->stackedWidget->setCurrentWidget(ui->menuPage);
    //2 菜单
    this->current_page = 2;
    m1.init();
    m2.init();
    mapping();
    ui->score_label->setText("score:");
}

//单人模式
void MainWindow::on_oneButton_clicked()
{
    //进入单人模式界面
    this->type=1;
    m1.init();
    m2.init();

    //m1.set_num(0,0,1024);
    //m1.set_num(0,1,1024);

    mapping();
    ui->stackedWidget->setCurrentWidget(ui->page);
    ui->Title->setFocus();
    //ui->deepseek->setVisible(false);
    //3 单人模式
    this->current_page = 3;
}

//人机对战
void MainWindow::on_robotButton_clicked()
{
    this->type = 4;
    //m2正式由ai托管
    if(this->robot_type==0)
    {
        this->ab = new alpha_beta(this->m2);
    }
    else if(this->robot_type == 2)
    {

    }
    mapping();
    ui->stackedWidget->setCurrentWidget(ui->twopage);
    ui->pktitle->setText("人机对战");
    //4 双人界面
    this->current_page = 4;
}

//单机双人
void MainWindow::on_pkButton_clicked()
{
    this->type = 2;
    mapping();
    ui->stackedWidget->setCurrentWidget(ui->twopage);
    //4 双人模式
    this->current_page = 4;
}

//全服排名
void MainWindow::on_rankButton_clicked()
{
    this->type=-1;
    ui->stackedWidget->setCurrentWidget(ui->rankPage);
    this->socket->write("rank");
    //6 全服排名
    this->current_page = 6;
}

//账号登出
void MainWindow::on_enloginButton_clicked()
{
    this->type=0;
    hide_message_layout();//隐藏对话框
    ui->stackedWidget->setCurrentWidget(ui->loginpage);
    //0 登录界面
    this->current_page = 0;
}

//优 雅 嘲 讽
void MainWindow::on_tauntButton_clicked()
{
    ui->tauntlabel1->movie()->start(); // 启动动画
    effect->play();
    timer->start(5600);
    if(this->type==3)
    {
        this->socket->write("taunt");
    }
}

void MainWindow::be_taunt()//接收嘲讽
{
    ui->tauntlabel2->movie()->start(); // 启动动画
    effect1->play();
    timer1->start(5600);
}

//时间到自动结束嘲讽
void MainWindow::finish_taunt()
{
    timer->stop();
    ui->tauntlabel1->movie()->stop();//结束动画
}
//嘲讽结束自动结束
void MainWindow::finish_be_taunt()
{
    timer1->stop();
    ui->tauntlabel2->movie()->stop();//结束动画
}

//联机对战
void MainWindow::on_linkpkButton_clicked()
{
    //this->type = 3;
    ui->stackedWidget->setCurrentWidget(ui->hallpage);

    //双方退出道具模式
    this->m2.exit_prop_mode();
    this->m1.exit_prop_mode();

    //5 联机大厅
    this->current_page = 5;
    this->socket->write("get_rooms");//加载房间列表
    ui->pktitle->setText("联机对战");
}

//连接对战两边移动信息
//map#move#十六格信息
void MainWindow::link_move()
{
    if(this->type<=2) return;
    QString message= "move";
    int i,j;
    fi(4)fj(4)
    {
        message+='#';
        message+=QString::number(m1.num(i,j));
    }
    //message += '\n';
    this->socket->write(message.toUtf8());
}

//接收对方的移动信息
//move#十六格信息
void MainWindow::recieve_move(QStringList &sl)
{
    int i,j;
    fi(4)fj(4)
    {
        m2.set_num(i,j,sl[i*4+j+1].toInt());
    }
    mapping();
}

//转到设置页面
void MainWindow::on_setButton_clicked()
{
    this->type=-1;
    ui->stackedWidget->setCurrentWidget(ui->settingpage);
    //m1.init();m1.new_two();
    //m2.init();m2.new_two();
    mapping();
}

//创建联机对战房间
void MainWindow::on_createRoomButton_clicked()
{
    this->type=3;
    this->socket->write("create_room");
    ui->stackedWidget->setCurrentWidget(ui->twopage);
    //4 双人模式
    this->current_page = 4;
    mapping();
}

//加入房间
void MainWindow::on_joinRoomButton_clicked()
{
    this->type=3;
    QString message="join#";
    QListWidgetItem* item = ui->roomListWidget->currentItem();
    if(item==NULL) return;
    message+=item->text();
    this->socket->write(message.toUtf8());
    ui->stackedWidget->setCurrentWidget(ui->twopage);
    //4 双人模式
    this->current_page = 4;
    mapping();
}

//接收房间信息
void MainWindow::recieve_rooms(QStringList &sl)
{
    ui->roomListWidget->clear();
    if(sl.size()<=1) return;
    for(int i=1;i<sl.size();++i)
    {
        ui->roomListWidget->addItem(sl[i]);
    }
}

//刷新房间列表
void MainWindow::on_flushButton_clicked()
{
    this->socket->write("get_rooms");//加载房间列表
}

//接收排名信息
void MainWindow::recieve_rank(QStringList &sl)//接收排名信息
{
    ui->my_name->setText("本机名称："+this->username);
    //将json格式文本转化成json对象
    QJsonDocument doc = QJsonDocument::fromJson(sl[1].toUtf8());
    if(doc.isNull()) return;
    //以json对象为标准限制表格尺寸
    auto data = doc.array();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(data.size());
    ui->tableWidget->setColumnCount(2);
    //填充表格数据
    for(int i=0; i<data.size(); ++i) {
        QJsonObject obj = data[i].toObject();
        //qDebug() << obj["username"].toString() << "得分：" << obj["highest_score"].toString();

        // 玩家名
        QTableWidgetItem* nameItem = new QTableWidgetItem(obj["username"].toString());
        ui->tableWidget->setItem(i, 0, nameItem);

        // 分数
        QTableWidgetItem* scoreItem = new QTableWidgetItem(obj["highest_score"].toString());
        ui->tableWidget->setItem(i, 1, scoreItem);
    }
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//延伸显示(水平铺满)
    QStringList l;
    l << "用户名" << "得分";
    ui->tableWidget->setHorizontalHeaderLabels(l);
}

//设置->返回界面
void MainWindow::on_backPageButton_clicked()
{
    //0账号登录，1注册，2菜单，3，4双人，5房间列表，6全服排名，7成就
    this->setFocus();
    switch(this->current_page)
    {
    case 0:
        ui->stackedWidget->setCurrentWidget(ui->loginpage);break;
    case 1:
        ui->stackedWidget->setCurrentWidget(ui->enrollpage);break;
    case 2:
        ui->stackedWidget->setCurrentWidget(ui->menuPage);break;
    case 3:
        ui->stackedWidget->setCurrentWidget(ui->page);this->type = 1;ui->Title->setFocus();break;
    case 4:
        ui->stackedWidget->setCurrentWidget(ui->twopage);this->type = 2;break;
    case 5:
        ui->stackedWidget->setCurrentWidget(ui->hallpage);break;
    case 6:
        ui->stackedWidget->setCurrentWidget(ui->rankPage);break;
    case 7:
        ui->stackedWidget->setCurrentWidget(ui->achievementpage);break;
    }
}


//音效声音大小修改
void MainWindow::on_voiceSlider_sliderMoved(int position)
{
    float Loudness = position / 100.0;//设置音量
    if(this->music_effect!=nullptr)
    {
        QAudioOutput *device = music_effect->audioOutput();
        if (device!=nullptr) device->setVolume(Loudness);

        if(music_game != nullptr)
        {
            QAudioOutput *device1 = music_game->audioOutput();
            if (device1!= nullptr) music_game->audioOutput()->setVolume(Loudness);
        }

        if (effect!=nullptr)
            effect->setVolume(Loudness);
        if (effect1!=nullptr)
            effect1->setVolume(Loudness);
    }
}


//背景音乐大小修改
void MainWindow::on_musicSlider_sliderMoved(int position)
{
    if(position==0)
    {
        this->background_music->pause();
        this->is_playing = false;
        ui->backgroundMusicPushButton->setText("播放");
    }
    if(this->is_playing == false)
    {
        this->background_music->play();
        this->is_playing = true;
        ui->backgroundMusicPushButton->setText("暂停");
    }
    float Loudness = position / 100.0;//设置音量
    if(this->background_music!=nullptr)
    {
        QAudioOutput *device = background_music->audioOutput();
        if (device!=nullptr) device->setVolume(Loudness);
    }
}

//背景音乐暂停与播放
void MainWindow::on_backgroundMusicPushButton_clicked()
{
    if (!this->is_playing)
    {
        background_music->play();
        ui->backgroundMusicPushButton->setText("暂停");
        this->is_playing = true;
    }
    else
    {
        background_music->pause();
        ui->backgroundMusicPushButton->setText("播放");
        this->is_playing = false;
    }
}

/*
 * 成就部分
 */

//成就列表
void MainWindow::on_achievementButton_clicked()
{
    this->current_page = 7;
    ui->stackedWidget->setCurrentWidget(ui->achievementpage);
    socket->write(QString("achievements#%1").arg(username).toUtf8());//向服务器发送消息
}

//初始化成就列表项
void MainWindow::init_achievements_items(QStringList &sl)
{
    //将json格式文本转化成json对象
    QJsonDocument doc = QJsonDocument::fromJson(sl[1].toUtf8());
    if(doc.isNull()) return;
    //以json对象为标准
    auto data = doc.array();
    //填充表格数据
    for(int i=0; i<data.size(); ++i) {
        QJsonObject obj = data[i].toObject();
        // 设置成就项示例
        QListWidgetItem *item1 = new QListWidgetItem(
            QIcon(obj["file_path"].toString()), obj["name"].toString() + "\n" + obj["description"].toString());
        if(obj["finish"]=="未完成")
        {
            item1->setData(Qt::UserRole, false);   // 未解锁
            item1->setForeground(QColor("#78909c"));
        }
        else
        {
            item1->setData(Qt::UserRole, true);   // 已解锁
            if(i==0) item1->setIcon(QIcon(":/achievement/first_game_finish.png"));
            else if(i==1)item1->setIcon(QIcon(":/achievement/2048_finish.png"));
            item1->setForeground(QColor("#4CDD13"));
        }
        ui->achievementListWidget->addItem(item1);
    }
}

//美化成就列表
void MainWindow::styleAchievementList()
{
    ui->achievementListWidget->setViewMode(QListWidget::IconMode);//显示模式
    ui->achievementListWidget->setStyleSheet(
        "QListWidget {"
        "   background-color: rgba(224, 255, 232, 120);"  // 半透晨雾绿
        "   border-radius: 8px;"
        "   padding: 0;"  // 关键1：消除容器内边距
        "   border: 1px solid rgba(127, 191, 143, 150);"
        "   padding: 6px;"
        "}"

        "QListWidget::item {"
        "   background: rgba(255,255,255,180);"         // 半透白底
        "   border-radius: 6px;"
        "   margin: 4px 6px;"  // 关键2：仅保留垂直间距
        //"   margin: 4px 2px;"// 原本的项间距
        "   border: 1px solid #c8e6c9;"
        //"   width: 100%;"    // 关键3：强制宽度填充
        "}"
        "QListWidget::item:hover {"
        "   background: qlineargradient(x1:0, y1:0, x1:0, y1:1,"
        "               stop:0 rgba(232, 245, 233, 200),"
        "               stop:1 rgba(200, 230, 201, 200));"
        "   border-color: #4caf50;"
        "}"
        "QListWidget::item:selected {"
        "   background: rgba(76, 175, 80, 150);"        // 选中状态
        "   color: #fffde7;"
        "}"
        ".locked { color: #78909c; }"                   // 未解锁样式类
        ".unlocked { color: #2e7d32; }"                 // 已解锁样式类

        "QScrollBar:vertical {"
        "   width: 10px;"
        "   background: rgba(200, 214, 200, 80);"
        "   border-radius: 4px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #81c784;"
        "   border-radius: 4px;"
        "   min-height: 30px;"
        "}"
        );

    // 设置项高度和字体
    ui->achievementListWidget->setIconSize(QSize(32,32));
    ui->achievementListWidget->setFont(QFont("Segoe UI", 11));
}

//完成第一局游戏
void MainWindow::finish_fist_game()
{
    QMessageBox::information(this,"达成成就","初出茅庐\n达成第一局游戏");
    this->socket->write(("achievement#" + (username.length()==0?"2":username) + "#1").toUtf8());
}

//ai移动（阿尔法贝塔剪枝算法）
void MainWindow::alpha_beta_move()
{
    if(this->robot_type == 2)
    {
        deepseek_move();
        return;
    }
    if(m2.finished()||m2.falsed()){
        return;
    }
    m2.move(ab->getBestMove());
    m2.new_two();
    ab->set_map(m2.copy());
}

//ai移动（DeepSeek）
void MainWindow::deepseek_move()
{
    QString text = "2048游戏目前的棋盘值（无方块为0）从第一行到第四行依次为：";
    int i,j;
    fi(4)
    {
        fj(4)
        {
            text += QString::number(m2.num(i,j)) + " ";
        }
        text+=",";
    }
    text += "请给出下一步最优的走法，你的回复只需一个字，回复向哪个方向移动即可";
    //qDebug() << text;

    ui->textEdit->clear();
    /*
     * 头部分
     */
    QNetworkRequest request;
    request.setUrl(QUrl("https://api.deepseek.com/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setRawHeader("Accept","application/json");
    request.setRawHeader("Authorization",""这里填你自己的apikey);
    /*
     * json对象
     */
    QJsonArray messages;
    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    systemMsg["content"] = "You are a helpful assistant";
    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = text;
    messages.append(systemMsg);
    messages.append(userMsg);

    QJsonObject requestBody;//请求体
    requestBody["messages"] = messages;
    requestBody["model"] = "deepseek-chat";
    requestBody["max_tokens"] = 2048;
    requestBody["stream"] = true;
    requestBody["temperature"] = 0.7;

    //发送请求
    QNetworkReply *reply = manager->post(request,QJsonDocument(requestBody).toJson());
    connect(reply,&QNetworkReply::readyRead,this,[=]{
        // qDebug() << reply->readAll();
        while(reply->canReadLine())
        {
            QString line = reply->readLine().trimmed();//去除开头或最后的回车和空格
            if(!line.startsWith("data: ")) continue;
            line.remove(0,6);
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(),&error);
            if(error.error == QJsonParseError::NoError)
            {
                QString content = doc.object()["choices"].toArray().first().toObject()["delta"].toObject()["content"].toString();
                if(!content.isEmpty())
                {
                    qDebug() << content;
                    if(content == "左") m2.left();
                    else if(content == "右") m2.right();
                    else if(content == "上") m2.up();
                    else if(content == "下") m2.down();
                    mapping();
                }
            }
        }
    });
    connect(reply,&QNetworkReply::finished,this,[=] {
        ui->textEdit->moveCursor(QTextCursor::End);
        ui->textEdit->insertPlainText("\n\n");
        reply->deleteLater();
    });
}


//接入deepseek
void MainWindow::on_pushButton_clicked()
{
    QString text = "2048游戏目前的棋盘值（无方块为0）从第一行到第四行依次为：";
    int i,j;
    fi(4)
    {
        fj(4)
        {
            text += QString::number(m1.num(i,j)) + " ";
        }
        text+=",";
    }
    text += "请给出下一步最优的走法，回复控制在200字以内";
    //qDebug() << text;

    ui->textEdit->clear();
    /*
     * 头部分
     */
    QNetworkRequest request;
    request.setUrl(QUrl("https://api.deepseek.com/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setRawHeader("Accept","application/json");
    request.setRawHeader("Authorization",""这里填你自己的API Key);
    /*
     * json对象
     */
    QJsonArray messages;
    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    systemMsg["content"] = "You are a helpful assistant";
    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = text;
    messages.append(systemMsg);
    messages.append(userMsg);

    QJsonObject requestBody;//请求体
    requestBody["messages"] = messages;
    requestBody["model"] = "deepseek-chat";
    requestBody["max_tokens"] = 2048;
    requestBody["stream"] = true;
    requestBody["temperature"] = 0.7;

    //发送请求
    QNetworkReply *reply = manager->post(request,QJsonDocument(requestBody).toJson());
    connect(reply,&QNetworkReply::readyRead,this,[=]{
        // qDebug() << reply->readAll();
        while(reply->canReadLine())
        {
            QString line = reply->readLine().trimmed();//去除开头或最后的回车和空格
            if(!line.startsWith("data: ")) continue;
            line.remove(0,6);
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(),&error);
            if(error.error == QJsonParseError::NoError)
            {
                QString content = doc.object()["choices"].toArray().first().toObject()["delta"].toObject()["content"].toString();
                if(!content.isEmpty())
                {
                    ui->textEdit->moveCursor(QTextCursor::End);
                    ui->textEdit->insertPlainText(content);
                }
            }
        }
    });
    connect(reply,&QNetworkReply::finished,this,[=] {
        ui->textEdit->moveCursor(QTextCursor::End);
        ui->textEdit->insertPlainText("\n\n");
        reply->deleteLater();
    });
}

//阿尔法贝塔剪枝
void MainWindow::on_radioButton_clicked()
{
    this->robot_type=0;
}

//接入deepseek
void MainWindow::on_DPR_Button_clicked()
{
    this->robot_type=2;
}

//直接通过房间号加入游戏
void MainWindow::on_selectRoomButton_clicked()
{
    this->socket->write(("join#"+ui->selectRoomLine->text()).toUtf8());
    ui->stackedWidget->setCurrentWidget(ui->twopage);
    //4 双人模式
    this->current_page = 4;
    mapping();
}

//查看帮助
void MainWindow::on_helpButton_clicked()
{
    QMessageBox::information(this,"帮助：",
                             "玩家1按键盘字母键WASD移动\n"
                             "玩家2按键盘右侧上下左右移动\n"
                             "设置中打开道具模式更有趣\n"
                             "道具模式下游戏格填满或游戏格子为空视为游戏失败\n"
                             "双人模式下键盘的R键和右边的数字键0按下有惊喜\n"
                             "多人游戏界面先有一方创建房间，另一方再通过房间号加入房间\n"
                             "DeepSeek按钮按下后可能有网络延迟，请耐心等待\n"
                             "设置页面可以调整音量和音效的大小\n"
                             "如果遇见人机犯唐请不要惊讶，毕竟是手搓的ai");
}

//设置音效为单击
void MainWindow::on_oneclickButton_clicked()
{
    this->voice_type = 0;
}

//设置音效为音游模式
void MainWindow::on_musicgameButton_clicked()
{
    this->voice_type = 1;
    static bool is_inited = false;

    if(is_inited) return;//加载过了就退出
    /*
     * 初始化音游播放器
     */
    //计时器跳过歌曲前摇
    this->music_game->play();
    voice_timer->start(700);

}

//音游模式
void MainWindow::voice_timer_used()
{
    music_game->pause();
    voice_timer->stop();
}

//选中状态改变
void MainWindow::on_propBox_checkStateChanged(int stage)
{
    qDebug() << "clicked";
    if(ui->propBox->isChecked())
    {
        this->m1.set_prop_mode();
        this->m2.set_prop_mode();
    }
    else
    {
        this->m1.exit_prop_mode();
        this->m2.exit_prop_mode();
    }
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    this->m1.set_finish();
}

