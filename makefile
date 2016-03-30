crawler: main.o queue.o url.o
	g++ -lresolv -lpthread libchilkat.a main.o PriorityQueue.o Url.o -o crawler

main.o: main.cpp chilkat/CkSpider.h PriorityQueue.h
	g++ -c main.cpp

queue.o: PriorityQueue.cpp PriorityQueue.h chilkat/CkSpider.h chilkat/CkStringArray.h Url.h
	g++ -c PriorityQueue.cpp

url.o: Url.cpp Url.h
	g++ -c Url.cpp

clean:
	rm *.o crawler