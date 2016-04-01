#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include "chilkat/CkSpider.h"
#include "chilkat/CkString.h"
#include "chilkat/CkStringArray.h"
#include "Url.h"

using namespace std;

struct CompareURL {
	// bool operator()(const Url& url1, const Url& url2) {
	bool operator()(string url1, string url2) {
		// return url1.getSize() > url2.getSize();
		return url1.size() > url2.size();
	}
};

class PriorityQueue {

private:

	// priority_queue<Url, std::vector<Url>, CompareURL> list;
	priority_queue<string, std::vector<string>, CompareURL> list;
	int size;
	
public:
	// Constructors
	PriorityQueue();

	// Destructor
	~PriorityQueue();

	// Setters
	// void queueURL(Url url);
	void queueURL(string url);

	// Getters
	// Url dequeueURL();
	string dequeueURL();
	int getSize();

};

#endif