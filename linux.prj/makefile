cc = g++
SRC_DIR = ..
OBJ_DIR = ./obj
APP_DIR = ./app

CFLAGS = -g #-v

all: client svr1 svr2 svr3 svr4 svr5 svr6 svr7 
.PHONY:all

# client.app
client: client.o base error
	cc $(CFLAGS) -o $(APP_DIR)/client.app $(OBJ_DIR)/client.o $(OBJ_DIR)/base.o $(OBJ_DIR)/error.o

client.o: $(SRC_DIR)/client/client.cpp 
	cc $(CFLAGS) -c $(SRC_DIR)/client/client.cpp -o $(OBJ_DIR)/client.o

# svr1 最简单 svr 服务器，使用了 fork，但不做相关的错误处理
svr1: svr1.o base error
	cc $(CFLAGS) -o $(APP_DIR)/svr1.app $(OBJ_DIR)/svr1.o $(OBJ_DIR)/base.o $(OBJ_DIR)/error.o

svr1.o: $(SRC_DIR)/svr/svr1.cpp
	cc $(CFLAGS) -c $(SRC_DIR)/svr/svr1.cpp -o $(OBJ_DIR)/svr1.o

#svr2 服务器是来一个 connect ，则 fork 出一个进程处理，并处理了子进程的异常情况——僵死，是 SIGCHLD 信号，以及处理了主进程的退出信号 SIGINT
svr2: svr2.o base error svr_header
	cc $(CFLAGS) -o $(APP_DIR)/svr2.app $(OBJ_DIR)/svr2.o $(OBJ_DIR)/base.o $(OBJ_DIR)/error.o $(OBJ_DIR)/svr_header.o

svr2.o: $(SRC_DIR)/svr/svr2.cpp
	cc $(CFLAGS) -c $(SRC_DIR)/svr/svr2.cpp -o $(OBJ_DIR)/svr2.o

#svr3 服务器预先派生一组进程，但是 accept 并不上锁
svr3: svr3.o base error svr_header
	cc $(CFLAGS) -o $(APP_DIR)/svr3.app $(OBJ_DIR)/svr3.o $(OBJ_DIR)/base.o $(OBJ_DIR)/error.o $(OBJ_DIR)/svr_header.o

svr3.o: $(SRC_DIR)/svr/svr3.cpp
	cc $(CFLAGS) -c $(SRC_DIR)/svr/svr3.cpp -o $(OBJ_DIR)/svr3.o

#svr4 服务器预先派生一组进程，在 accept 前加上一个文件锁：因为有些系统在实现 accept 的时候……不清楚，和它非原子操作，以及流的实现有关系
svr4: svr4.o base error svr_header
	cc $(CFLAGS) -o $(APP_DIR)/svr4.app $(OBJ_DIR)/svr4.o $(OBJ_DIR)/base.o $(OBJ_DIR)/error.o $(OBJ_DIR)/svr_header.o

svr4.o: $(SRC_DIR)/svr/svr4.cpp
	cc $(CFLAGS) -c $(SRC_DIR)/svr/svr4.cpp -o $(OBJ_DIR)/svr4.o

#svr5 服务器预先派生一组进程，在 accept 前加上一个线程锁，这样同一进程，或者多进程之间，都能够被保护到
svr5: svr5.o base error svr_header
	cc $(CFLAGS) -o $(APP_DIR)/svr5.app $(OBJ_DIR)/svr5.o $(OBJ_DIR)/base.o $(OBJ_DIR)/error.o $(OBJ_DIR)/svr_header.o

svr5.o: $(SRC_DIR)/svr/svr5.cpp
	cc $(CFLAGS) -o $(OBJ_DIR)/svr5.o -c $(SRC_DIR)/svr/svr5.cpp

#svr6 服务器预先派生一组进程，通过双工管道传递 connfd，并且还通过对管道装态的设置，再使用 select 读取，完成父子之间的通讯
svr6: svr6.o base error svr_header
	cc $(CFLAGS) -o $(APP_DIR)/svr6.app $(OBJ_DIR)/svr6.o $(OBJ_DIR)/base.o $(OBJ_DIR)/error.o $(OBJ_DIR)/svr_header.o

svr6.o: $(SRC_DIR)/svr/svr6.cpp
	cc $(CFLAGS) -o $(OBJ_DIR)/svr6.o -c $(SRC_DIR)/svr/svr6.cpp

#svr7 服务器预先派生一组进程，通过双工管道传递 connfd，并且还通过对管道装态的设置，再使用 select 读取，完成父子之间的通讯
svr7: svr7.o base error svr_header
	cc $(CFLAGS) -o $(APP_DIR)/svr7.app $(OBJ_DIR)/svr7.o $(OBJ_DIR)/base.o $(OBJ_DIR)/error.o $(OBJ_DIR)/svr_header.o

svr7.o: $(SRC_DIR)/svr/svr7.cpp
	cc $(CFLAGS) -o $(OBJ_DIR)/svr7.o -c $(SRC_DIR)/svr/svr7.cpp

svr_header: $(SRC_DIR)/svr/svr_header.cpp
	cc $(CFLAGS) -c $(SRC_DIR)/svr/svr_header.cpp -o $(OBJ_DIR)/svr_header.o

base: $(SRC_DIR)/base/unp.h $(SRC_DIR)/base/base.cpp
	cc $(CFLAGS) -c $(SRC_DIR)/base/base.cpp -o $(OBJ_DIR)/base.o

error: $(SRC_DIR)/base/unp.h $(SRC_DIR)/base/error.cpp
	cc $(CFLAGS) -c $(SRC_DIR)/base/error.cpp -o $(OBJ_DIR)/error.o

.PHONY:clean
clean:
	-rm -f $(OBJ_DIR)/*.o
	-rm -f $(APP_DIR)/*.app
