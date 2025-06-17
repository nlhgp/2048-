#ifndef MY_THREAD_H
#define MY_THREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>

class my_thread : public QThread
{
    Q_OBJECT
public:
    explicit my_thread(QObject *parent = nullptr);
    void run();
    void get_socket(QTcpSocket *s);
    QTcpSocket *socket;
    QString set_ip();
    QString get_ip();
    QString get_name();
    void recieve_taunt();//接收嘲讽
    void achieve_moving(QString s);

private:
    QString IP;//ip地址
    QString username;//用户名

public slots:
    void client_info_slot();
    void deconned();//断开连接，下线

signals:
    void message(QString ba);
    void send_moving(QString s);
    void join_room(my_thread *t,int id);
    void create_room(my_thread *thread);
    void taunt();
    void get_player_ip();
};

#endif // MY_THREAD_H
