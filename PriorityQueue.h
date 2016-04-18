#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <algorithm>
#include "Url.h"

using namespace std;

// Compare function used in the priority_queue to make a min heap
struct CompareURL {
	bool operator()(const string& url1, const string& url2) {
		return getURLsize(url1) > getURLsize(url2);
	}
};

class PriorityQueue{

private:
	vector<string> list;

public:
	PriorityQueue();
	~PriorityQueue();
	
	void push(string url);
	string pop();

	int size();
	bool empty();
	
	string getFromVector(int i);
	string popFromVector();
	void clear();

};

#endif