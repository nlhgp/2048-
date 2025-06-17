#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>//数据库头文件
#include <qtcpsocket.h>//网络
#include <qhostaddress.h>//ip地址类
#include <qmessagebox.h>//提示窗口
#include <QMouseEvent>//鼠标事件

#include <QSoundEffect>//提示音效
#include <QMediaPlayer>//播放音乐
#include <QAudioOutput>

#include <QTimer>//计时器
#include <QJsonObject> // { }
#include <QJsonArray> // [ ]
#include <QJsonDocument> // 解析Json
#include <QJsonValue> // int float double bool null { } [ ]
#include <QJsonParseError>
#include <alpha_beta.h>//阿尔法贝塔剪枝
#include <qnetworkaccessmanager.h>//deepseek发送信息
#include <QDebug>

//#define //音节数


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void add_chat(QString&);
    void mapping();//将数据印射到界面中
    void keyPressEvent(QKeyEvent * event);//重写键盘点击事件
    void paintEvent(QPaintEvent * event);//重载paintevent
    void be_taunt();//接收嘲讽
    void recieve_rooms(QStringList &sl);//接收房间信息
    void recieve_rank(QStringList &sl);//接收排名信息
    void set_player_ip(QStringList &sl);//设置对方ip
    QString getTileStyle(int number);//根据数字不同返回样式表
    ~MainWindow();

private slots:
    /*
     * 账号注册
    */
    void on_checkpushButton_clicked();
    void on_EnrollpushButton_clicked();
    void switch_line();
    void switch_hidepassword();

    /*
     * 账号登录
     */
    void on_DownloadButton_clicked();
    void on_EnrollButton_clicked();

    /*
     * 网络部分
     */
    void send_message();//发送信息
    void on_linkButton_clicked();//连接服务器
    void on_cfButton_clicked();//切换至群聊
    void on_personButton_clicked();//切换至私聊
    void recieve_message();//接收服务器信息
    void finish_link();//连接主机成功

    /*
     * 界面
     */
    void hide_message_layout();//隐藏对话框
    void on_backButton_clicked();
    void on_nolinkButton_clicked();
    void on_menuButton_clicked();
    void on_oneButton_clicked();
    void on_robotButton_clicked();
    void on_pkButton_clicked();
    void on_rankButton_clicked();
    void on_enloginButton_clicked();

    /*
     * 优雅嘲讽
     */
    void on_tauntButton_clicked();
    void finish_taunt();
    void finish_be_taunt();
    void on_linkpkButton_clicked();
    void on_setButton_clicked();

    /*
     * 操作部分
     */
    void link_move();//向服务器发送移动信息
    void recieve_move(QStringList &sl);//接收对方的移动信息
    void on_createRoomButton_clicked();
    void on_joinRoomButton_clicked();
    void on_flushButton_clicked();
    void on_backPageButton_clicked();//从设置页面返回
    void on_achievementButton_clicked();//进入成就列表
    void on_voiceSlider_sliderMoved(int position);
    void on_musicSlider_sliderMoved(int position);
    void on_backgroundMusicPushButton_clicked();//背景音乐开关

    void on_pushButton_clicked();

    void on_radioButton_clicked();

    void on_DPR_Button_clicked();

    void on_selectRoomButton_clicked();

    void on_helpButton_clicked();

    void on_oneclickButton_clicked();

    void on_musicgameButton_clicked();

    void voice_timer_used();//音游播放用槽函数

    void on_propBox_checkStateChanged(int stage);

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket=NULL;//网络对象指针
    QString user_id,username=NULL;//当前用户数据库id，用于信号传递
    char type = 0;//查看当前属于什么模式1单人 2单机双人 3联机对战 4人机对战
    char robot_type = 0;//0阿尔法贝塔剪枝 1机器学习 2deepseek
    char voice_type = 0;//0单机模式 1音效模式
    char chat_type=1;//查看当前聊天模式
    char current_page=0;//当前在哪一页 0账号登录，1注册，2菜单，3单人，4双人，5房间列表，6全服排名，7成就
    bool linked=false;//是否处于联机模式
    bool messageHide=true;//对话框是否显示
    bool is_clicked=false;//是否正在点击
    bool is_playing = true;//播放器是否播放
    bool _2048ed = false;
    QString ip = "127.0.0.1";
    QString port = "8000";
    map m1,m2;
    QTimer *timer,*timer1;//计时器
    QTimer *voice_timer;//控制音效播放
    QSoundEffect *effect,*effect1;//嘲讽播放器
    QMediaPlayer *background_music,*music_effect,*music_game;//背景音乐播放器
    QAudioOutput *audioOutput;
    QPoint *mouse;//鼠标坐标
    alpha_beta *ab = NULL;//阿尔法贝塔剪枝算法
    QNetworkAccessManager *manager;
    void init_ui();
    void init_qss();
    void styleAchievementList();//成就列表样式设置
    void init_achievements_items(QStringList &sl);//初始化成就列表项
    void finish_fist_game();//达成成就，完成首次游戏
    void alpha_beta_move();//阿尔法贝塔剪枝算法ai移动
    void deepseek_move();//deepseek移动
};
#endif // MAINWINDOW_H
