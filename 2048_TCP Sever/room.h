#ifndef ROOM_H
#define ROOM_H

#include <QObject>
#include <my_thread.h>

class room : public QObject
{
    Q_OBJECT
public:
    explicit room(QObject *parent = nullptr);
    void set_id(int id);
    int get_id();
    void create_room(my_thread *p);
    void join_room(my_thread *p);
    void set_name(QString &name){this->roomName=name;}
    QString& get_name(){return this->roomName;}

private slots:
    void send_map(QString s);
    void achieve_map(QString s);
    void send_taunt();
    void achieve_taunt();
    void get_1_ip();
    void get_2_ip();

//signals:

private:
    QString roomName;//房间名
    int roomId;//房间id
    my_thread *player1=NULL,*player2=NULL;
};

#endif // ROOM_H
