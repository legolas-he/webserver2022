#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include <iostream>
#include "sql.h"

using namespace std;

connection_pool::connection_pool()
{
	this->connNum=0;
}

connection_pool *connection_pool::GetInstance()
{
	static connection_pool connPool;
	return &connPool;
}

void connection_pool::init(string host, string User, string PassWord, string DBName, int Port, unsigned int MaxConn)
{
	this->host = host;
	this->Port = Port;
	this->User = User;
	this->PassWord = PassWord;
	this->DatabaseName = DBName;
	
	lock.lock();
	for (int i = 0; i < MaxConn; i++)
	{
		MYSQL *con = NULL;
		con = mysql_init(con);

		if (con == NULL)
		{
			cout << "Error:" << mysql_error(con);
			exit(1);
		}
		con = mysql_real_connect(con, host.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port, NULL, 0);

		if (con == NULL)
		{
			cout << "Error: " << mysql_error(con);
			exit(1);
		}
		connList.push_back(con);
		++connNum;
	}

	reserve = sem(connNum);
	
	lock.unlock();
}

MYSQL *connection_pool::GetConnection()
{
	MYSQL *con = NULL;

	if (0 == connList.size())
		return NULL;

	reserve.wait();
	
	lock.lock();

	con = connList.front();
	connList.pop_front();


	lock.unlock();
	return con;
}


bool connection_pool::release(MYSQL *con)
{
	if (NULL == con)
		return false;

	lock.lock();

	connList.push_back(con);

	lock.unlock();

	reserve.post();
	return true;
}

void connection_pool::destroy()
{

	lock.lock();
	if (connList.size() > 0)
	{
		list<MYSQL *>::iterator it;
		for (it = connList.begin(); it != connList.end(); ++it)
		{
			MYSQL *con = *it;
			mysql_close(con);
		}
		connNum=0;
		connList.clear();

		lock.unlock();
	}

	lock.unlock();
}

connection_pool::~connection_pool()
{
	destroy();
}

connectionRAII::connectionRAII(MYSQL **SQL, connection_pool *connPool){
	*SQL = connPool->GetConnection();
	
	conRAII = *SQL;
	poolRAII = connPool;
}

connectionRAII::~connectionRAII(){
	poolRAII->release(conRAII);
}
