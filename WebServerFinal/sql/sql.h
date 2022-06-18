#ifndef SQL_CONN_H
#define SQL_CONN_H

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../locker.h"

using namespace std;

class connection_pool
{
public:
	MYSQL *GetConnection();				 //获取数据库连接
	bool release(MYSQL *conn); //释放连接
	void destroy();					 //销毁所有连接

	//单例模式
	static connection_pool *GetInstance();

	void init(string url, string User, string PassWord, string DataBaseName, int Port, unsigned int MaxConn); 
	
	connection_pool();
	~connection_pool();

private:
	unsigned int connNum;  //连接数

private:
	locker lock;
	list<MYSQL *> connList; 
	sem reserve;

private:
	string host;		 //主机地址
	string Port;		 //数据库端口号
	string User;		 //数据库用户名
	string PassWord;	 //数据库密码
	string DatabaseName; //数据库名
};

class connectionRAII{

public:
	connectionRAII(MYSQL **con, connection_pool *connPool);
	~connectionRAII();
	
private:
	MYSQL *conRAII;
	connection_pool *poolRAII;
};

#endif

