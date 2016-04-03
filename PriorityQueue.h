#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include "Url.h"
#include <unordered_map> // C++11 Hash Table

using namespace std;

struct CompareURL {
	bool operator()(const Url& url1, const Url& url2) {
		return url1.getSize() > url2.getSize();
	}
};

class PriorityQueue {

private:

	priority_queue<Url, std::vector<Url>, CompareURL> list;
	unordered_map<string, char> queued_url;
	int size;
	
public:
	// Constructors
	PriorityQueue();

	// Destructor
	~PriorityQueue();

	// Setters
	void queueURL(Url url);

	// Getters
	Url dequeueURL();
	int getSize();

};

#endif