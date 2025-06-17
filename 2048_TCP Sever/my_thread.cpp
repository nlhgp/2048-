#include "my_thread.h"
my_thread::my_thread(QObject *parent)
    : QThread{parent}
{
}

void my_thread::run()
{
    connect(socket,&QTcpSocket::readyRead,this,&my_thread::client_info_slot);
}

void my_thread::get_socket(QTcpSocket *s)//获取socket对象
{
    this->socket=s;
    this->IP=s->peerAddress().toString();
    connect(socket,&QTcpSocket::disconnected,this,&my_thread::deconned);
}

void my_thread::client_info_slot()
{
    QString m=socket->readAll();
    QStringList sl=m.split('#');//将字符串以#分割
    if(sl[0]=="init")//获取用户名
        this->username=sl[1];
    else if(sl[0]=="move")
    {
        emit send_moving(m);
    }
    else if(sl[0]=="create_room")
    {
        emit create_room(this);
    }
    else if(sl[0]=="join")
    {
        emit join_room(this,sl[1].toInt());
    }
    else if(sl[0]=="taunt")//嘲讽
    {
        emit taunt();
    }
    else if(sl[0]=="get_player_ip")
    {
        emit get_player_ip();
    }
    emit message(IP+"#"+m.toUtf8());
    //qDebug() << m;
}

void my_thread::deconned()
{
    emit message((IP+"#deconned").toUtf8());
}

QString my_thread::get_ip()
{
    return this->IP;
}

QString my_thread::get_name()
{
    return this->username;
}

void my_thread::achieve_moving(QString s)
{
    this->socket->write(s.toUtf8());
}

void my_thread::recieve_taunt()//接收嘲讽
{
    this->socket->write("taunt#");
}
