#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include <iostream>
#include <mysql.h>
#include "Connection.h"
#include "CommentConnectionPool.h"


using namespace std;
int main()
{   

    Connection conn;
    conn.connect("127.0.0.1", 3306, "root", "123456", "chat");  //对于mysql来说，使用同一用户名登录连接不合法，在多线程开始前连接一次可解决
    clock_t begin = clock();

    thread t1([]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 1250; i++) {
            auto sp = cp->getConnection();
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                "zhang san", 20, "M");
            sp->update(sql);
        }
        });
    thread t2([]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 1250; i++) {
            auto sp = cp->getConnection();
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                "zhang san", 20, "M");
            sp->update(sql);
        }
        });
    thread t3([]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 1250; i++) {
            auto sp = cp->getConnection();
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                "zhang san", 20, "M");
            sp->update(sql);
        }
        });
    thread t4([]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 1250; i++) {
            auto sp = cp->getConnection();
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                "zhang san", 20, "W");
            sp->update(sql);
        }
        });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

#if 0
    Connection conn;
    conn.connect("127.0.0.1", 3306, "root", "123456", "chat");  //对于mysql来说，使用同一用户名登录连接不合法，在多线程开始前连接一次可解决
    std::cout << "begin!!!" << endl;
    clock_t begin = clock();

    thread t1([]() {
        for (int i = 0; i < 250; i++) {
            Connection conn;
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                "zhang san", 20, "M");
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
            conn.update(sql);
        }
    });
    thread t2([]() {
        for (int i = 0; i < 250; i++) {
            Connection conn;
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                "zhang san", 20, "M");
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
            conn.update(sql);
        }
    });
    thread t3([]() {
        for (int i = 0; i < 250; i++) {
            Connection conn;
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                "zhang san", 20, "M");
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
            conn.update(sql);
        }
    });
    thread t4([]() {
        for (int i = 0; i < 250; i++) {
            Connection conn;
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                "zhang san", 20, "M");
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
            conn.update(sql);
        }
        });

    t1.join();
    t2.join();
    t3.join();
    t4.join();
#endif
#if 0

    for (int i = 0; i < 10000; i++) 
    {
        Connection conn;
        char sql[1024] = { 0 };
        sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
            "zhang san", 20, "M");
        conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
        conn.update(sql);

        //ConnectionPool* cp = ConnectionPool::getConnectionPool();
        //auto sp = cp->getConnection();
        ////cp->getInformation();
        //char sql[1024] = { 0 };
        //sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
        //    "zhang san", 20, "M");
        //sp->update(sql);
        

    }
#endif
    /*clock_t end = clock();
    std::cout << (end - begin) << "ms" << endl;*/

    return 0;
}
