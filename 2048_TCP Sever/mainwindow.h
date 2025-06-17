#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject> // { }
#include <QJsonArray> // [ ]
#include <QJsonDocument> // 解析Json
#include <QJsonValue> // int float double bool null { } [ ]
#include <QJsonParseError>
#include <QTcpServer>
#include <qtcpsocket.h>
#include <QtSql/QSqlDatabase>//数据库头文件
#include <qsqlquery.h>//操作数据库对象
#include <QMessageBox>
#include <my_thread.h>//线程
#include <QList>//容器
#include <room.h>
#define fstl(act) for(auto it=act.begin();it!=act.end();it++)
#define PORT 8000
#define ROOM_MAX_SIZE 50

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void new_client_hander();
    void threadslot(QString ba);
    void create_room(my_thread *p1);
    void join_room(my_thread *p2,int room_id);
    void on_userButton_clicked();//查看在线用户
    void on_roomsButton_clicked();//查看房间
    void on_clearButton_clicked();//清除日志

public:
    MainWindow(QWidget *parent = nullptr);
    void init_ui();
    void add_item(QString &m);
    void deconnected(QStringList &sl);
    void send_message(QString &message);
    void send_rooms(QStringList &sl);
    void get_rank(QStringList &sl);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpServer *sever;
    QList<my_thread*> mts;
    QVector<room*> rooms;
    QSqlDatabase db;
    void login_text(QStringList &sl);
    void enroll_text(QStringList &sl);
    void alter_text(QStringList &sl);
    void score_text(QStringList &sl);
    void public_message(QString &m,QStringList &sl);//发送全服消息
    void private_message(QString &m,QStringList &sl);//发送私聊消息
    void achievements_text(QStringList &sl);//获取成就列表
    void achievement_text(QStringList &sl);//达成成就
};
#endif // MAINWINDOW_H
