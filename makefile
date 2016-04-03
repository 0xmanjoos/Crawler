crawler: main.o queue.o url.o
	g++ -std=c++11 -lresolv -lpthread libchilkat.a main.o PriorityQueue.o Url.o -o crawler

main.o: main.cpp PriorityQueue.h Url.h chilkat/CkSpider.h chilkat/CkString.h chilkat/CkStringArray.h
	g++ -std=c++11 -c main.cpp

queue.o: PriorityQueue.cpp PriorityQueue.h Url.h chilkat/CkSpider.h chilkat/CkString.h chilkat/CkStringArray.h
	g++ -std=c++11 -c PriorityQueue.cpp

url.o: Url.cpp Url.h chilkat/CkSpider.h chilkat/CkString.h chilkat/CkStringArray.h
	g++ -std=c++11 -c Url.cpp

clean:
	rm *.o crawler