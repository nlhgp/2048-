# 2048-

## 配置说明

**QT版本：**
QMake version 3.1
Using Qt version 6.5.3 in D:/QT/6.5.3/msvc2019_64/lib构建套件：Desktop Qt %{Qt:Version} MSVC2019 64bit

**数据库：**
mysql  Ver 8.0.39 for Win64 on x86_64 (MySQL Community Server - GPL)

大二下实训大作业

## 项目简介

本项目是一个基于Qt框架开发的网络版2048游戏系统，采用C/S架构设计，实现了完整的服务端和客户端功能，两端均采用前后端分离的设计理念。
客户端前端部分采用QWidget，实现账号登录与注册页面、用户聊天界面、单人/双人游戏界面、联机大厅界面、全服排名界面、成就列表界面、设置界面；后端采用C++/QT实现游戏引擎map.cpp，所有游戏移动逻辑均在游戏引擎对象中处理，连接了控件的信号与槽，基于AlphaBeta剪枝算法的简单游戏ai，基于QT-NetWork接入DeepSeek Api给出当前游戏局势建议，客户端通过TcpSocket向服务端发送请求并接收信息。
服务端和客户端通过TCP/IP协议进行数据传输。
服务端前端亦采用QWidget，显示部分自定义日志信息、在线用户列表和对战房间列表；后端为C++/QT实现基于TCP/IP协议的多线程服务器，将用户信息、成就信息部分接入MySql数据库，游戏对战房间以对象的形式存储在服务端内存中，通过TcpSocket接收客户端传来的请求进行相应数据库操作，并返回结果值。

**已隐去项目中原本使用的DeepSeek Api，如需使用相关功能请在代码里边换成你自己的API key**
