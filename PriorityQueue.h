#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include "Url.h"

using namespace std;

struct CompareURL {
	bool operator()(const string& url1, const string& url2) {
		return getURLsize(url1) > getURLsize(url2);
	}
};

template <class T, class S, class C>
    S& Container(priority_queue<T, S, C>& q) {
        struct HackedQueue : private priority_queue<T, S, C> {
            static S& Container(priority_queue<T, S, C>& q) {
                return q.*&HackedQueue::c;
            }
        };
    return HackedQueue::Container(q);
}


class PriorityQueue {

private:

	priority_queue<string, std::vector<string>, CompareURL> list;
	
public:
	// Constructors
	PriorityQueue();

	// Destructor
	~PriorityQueue();

	// Setters
	void queueURL(string url);

	// Getters
	string dequeueURL();
	int getSize();
	string backingUp();

	bool empty();

};

#endif