#include "PriorityQueue.h"

// Constructors

PriorityQueue::PriorityQueue(){
	this->size = 0;
}

// Destructor

PriorityQueue::~PriorityQueue(){
	// this->size = 0;
	// delete(&list);
	// del(list); // ?
}

void PriorityQueue::queueURL(string url){
	this->list.push(url);
	this->size++;
}

string PriorityQueue::dequeueURL(){
	// Url url ((char*)"");
	string url;

	// url.setUrl("No URL");

	if (!this->list.empty()){
		url = this->list.top();
		// cout << "\tDequeueing " << url.getUrl() << endl;
		this->size--;
		this->list.pop();
	}

	return url;
}

int PriorityQueue::getSize(){
	return this->size;
}