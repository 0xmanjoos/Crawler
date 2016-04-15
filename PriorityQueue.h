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

// Allowing access to priority_queue container (ref: http://stackoverflow.com/a/1385520)
// It makes backing up queue faster
template <class T, class S, class C>
    S& Container(priority_queue<T, S, C>& q) {
        struct HackedQueue : private priority_queue<T, S, C> {
            static S& Container(priority_queue<T, S, C>& q) {
                return q.*&HackedQueue::c;
            }
        };
    return HackedQueue::Container(q);
}

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