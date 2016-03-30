crawler: main.o queue.o
	g++ -lresolv -lpthread libchilkat.a main.o PriorityQueue.o -o crawler

main.o: main.cpp chilkat/CkSpider.h PriorityQueue.h
	g++ -c main.cpp

queue.o: PriorityQueue.cpp PriorityQueue.h chilkat/CkSpider.h chilkat/CkStringArray.h
	g++ -c PriorityQueue.cpp

clean:
	rm *.o crawler