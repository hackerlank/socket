cc = g++
dir = ..

all: client svr1
.PHONY:all

client: client.o base.o error
	cc -o client.app client.o base.o error.o

client.o: ./client/client.cpp 
	cc -c ./client/client.cpp

svr1: svr1.o base.o error
	cc -o svr1.app svr1.o base.o error.o

svr1.o: ./svr/svr1.cpp
	cc -c ./svr/svr1.cpp

base.o: ./base/unp.h ./base/base.cpp
	cc -c ./base/base.cpp

error: ./base/unp.h ./base/error.cpp
	cc -c ./base/error.cpp

.PHONY:clean
clean:
	-rm -f *.o
	-rm -f *.app
