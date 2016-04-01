crawler: main.o queue.o url.o
	g++ -lresolv -lpthread libchilkat.a main.o PriorityQueue.o Url.o -o crawler

main.o: main.cpp PriorityQueue.h Url.h chilkat/CkSpider.h chilkat/CkString.h chilkat/CkStringArray.h
	g++ -c main.cpp

queue.o: PriorityQueue.cpp PriorityQueue.h Url.h chilkat/CkSpider.h chilkat/CkString.h chilkat/CkStringArray.h
	g++ -c PriorityQueue.cpp

url.o: Url.cpp Url.h chilkat/CkSpider.h chilkat/CkString.h chilkat/CkStringArray.h
	g++ -c Url.cpp

clean:
	rm *.o crawler