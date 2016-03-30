#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include "chilkat/CkSpider.h"
#include "chilkat/CkStringArray.h"
#include "Url.h"

using namespace std;

class PriorityQueue {

private:

	priority_queue<char*, std::vector<char*>, comparator> list;
	int size;

	int getURLsize(CkString url);
	char* cleaningURL(string url);
	
public:
	// Constructors
	PriorityQueue();
	PriorityQueue(vector<string> list, int size);
	// PriorityQueue(PriorityQueue *q);

	// Destructor
	~PriorityQueue();

	// Setters
	void queueURL(char* url);

	// Getters
	string getURL();

};

#endif