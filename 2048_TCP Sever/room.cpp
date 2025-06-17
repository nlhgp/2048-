#include "room.h"

room::room(QObject *parent)
    : QObject{parent}
{
}

//1->2
void room::send_map(QString m)
{
    if(this->player2 != NULL)
    player2->achieve_moving(m);
}

//2->1
void room::achieve_map(QString s)
{
    if(this->player1 != NULL)
    player1->achieve_moving(s);
}

//1->2
void room::send_taunt()
{
    if(this->player2 != NULL)
        player2->recieve_taunt();
}

//2->1
void room::achieve_taunt()
{
    if(this->player1 != NULL)
        player1->recieve_taunt();
}

void room::set_id(int id)
{
    this->roomId=id;
}

int room::get_id()
{
    return this->roomId;
}

//获取玩家1ip
void room::get_1_ip()
{
    this->player2->socket->write(("player_ip#" + this->player1->get_ip()).toUtf8());
}

//获取玩家2ip
void room::get_2_ip()
{
    this->player1->socket->write(("player_ip#" + this->player2->get_ip()).toUtf8());
}

//创建房间，设置房主
void room::create_room(my_thread *p)
{
    this->player1=p;
    connect(this->player1,&my_thread::send_moving,this,&room::send_map);
    connect(this->player1,&my_thread::taunt,this,&room::send_taunt);
    connect(this->player1,&my_thread::get_player_ip,this,&room::get_2_ip);
    p->socket->write(("room_id#"+QString::number(roomId)).toUtf8());
}

//加入房间，设置访客
void room::join_room(my_thread *p)
{
    this->player2=p;
    connect(this->player2,&my_thread::send_moving,this,&room::achieve_map);
    connect(this->player2,&my_thread::taunt,this,&room::achieve_taunt);
    connect(this->player2,&my_thread::get_player_ip,this,&room::get_1_ip);
    p->socket->write(("room_id#"+QString::number(roomId)).toUtf8());
}
