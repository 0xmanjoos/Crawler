UNAME_S := $(shell uname -s)
TOP := $(shell pwd)
ifeq ($(UNAME_S),Linux)
    FLAGS += $(TOP)/chilkat/lib/libchilkat-9.5.0.so
endif
ifeq ($(UNAME_S),Darwin)
    FLAGS += chilkat/lib/libchilkat.a
endif
FLAGS += -lresolv -lpthread

crawler: main.o queue.o url.o
	g++ -std=c++11 main.o PriorityQueue.o Url.o $(FLAGS) -o crawler

main.o: main.cpp PriorityQueue.h Url.h chilkat/include/CkSpider.h chilkat/include/CkString.h chilkat/include/CkStringArray.h
	g++ -std=c++11 -c main.cpp

queue.o: PriorityQueue.cpp PriorityQueue.h Url.h chilkat/include/CkSpider.h chilkat/include/CkString.h chilkat/include/CkStringArray.h
	g++ -std=c++11 -c PriorityQueue.cpp

url.o: Url.cpp Url.h chilkat/include/CkSpider.h chilkat/include/CkString.h chilkat/include/CkStringArray.h
	g++ -std=c++11 -c Url.cpp

clean:
	rm *.o crawler