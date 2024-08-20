#include "pch.h"
#include <iostream>
#include <mysql.h>
#include "Connection.h"
#include "CommentConnectionPool.h"
using namespace std;
int main()
{   
   
    //std::cout << "Hello World!\n";
    //Connection conn;
    //string sql = "INSERT INTO user(name,age,sex) values('zhangsan',20,'male');";
    //conn.connect("127.0.0.1", 3306, "root", "258369", "chat");
    //conn.update(sql);
    ConnectionPool *cp = ConnectionPool::getConnectionPool();

    return 0;
}
