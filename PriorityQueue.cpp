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

void PriorityQueue::queueURL(Url url){
	if (url.getSize() > 0){
		cout << "\tQueueing " << url.getUrl() << endl;
		this->list.push(url);
		this->size++;
	}

}

Url PriorityQueue::dequeueURL(){
	Url url;

	if (!this->list.empty()){
		url = this->list.top();
		this->list.pop();
		cout << "\tDequeueing " << url.getUrl() << endl;
		this->size--;
	}

	return url;
}

int PriorityQueue::getSize(){
	return this->size;
}