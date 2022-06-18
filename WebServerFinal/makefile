WebServer: main.c ./threadpool/threadpool.h ./http/http_conn.cpp ./http/http_conn.h ./locker.h ./log/log.cpp ./log/log.h ./sql/sql.cpp ./sql/sql.h
	g++ -o server main.c ./threadpool/threadpool.h ./http/http_conn.cpp ./http/http_conn.h ./locker.h ./log/log.cpp ./log/log.h ./sql/sql.cpp ./sql/sql.h -lpthread -lmysqlclient


clean:
	rm  -r WebServer
