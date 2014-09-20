cc = g++
SRC_DIR = ..
OBJ_DIR = ./obj
APP_DIR = ./app


all: client svr1 svr2
.PHONY:all

client: client.o base error
	cc -o $(APP_DIR)/client.app $(OBJ_DIR)/client.o $(OBJ_DIR)/base.o $(OBJ_DIR)/error.o

client.o: $(SRC_DIR)/client/client.cpp 
	cc -c $(SRC_DIR)/client/client.cpp -o $(OBJ_DIR)/client.o

svr1: svr1.o base error
	cc -o $(APP_DIR)/svr1.app $(OBJ_DIR)/svr1.o $(OBJ_DIR)/base.o $(OBJ_DIR)/error.o

svr1.o: $(SRC_DIR)/svr/svr1.cpp
	cc -c $(SRC_DIR)/svr/svr1.cpp -o $(OBJ_DIR)/svr1.o

svr2: svr2.o base error
	cc -o $(APP_DIR)/svr2.app $(OBJ_DIR)/svr2.o $(OBJ_DIR)/base.o $(OBJ_DIR)/error.o

svr2.o: $(SRC_DIR)/svr/svr2.cpp
	cc -c $(SRC_DIR)/svr/svr2.cpp -o $(OBJ_DIR)/svr2.o

base: $(SRC_DIR)/base/unp.h $(SRC_DIR)/base/base.cpp
	cc -c $(SRC_DIR)/base/base.cpp -o $(OBJ_DIR)/base.o

error: $(SRC_DIR)/base/unp.h $(SRC_DIR)/base/error.cpp
	cc -c $(SRC_DIR)/base/error.cpp -o $(OBJ_DIR)/error.o

.PHONY:clean
clean:
	-rm -f $(OBJ_DIR)/*.o
	-rm -f $(APP_DIR)/*.app
