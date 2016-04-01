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
	// if (url.size() > 0){
		this->list.push(url);
		cout << "\tQueueing " << url.getUrl() << endl;
		this->size++;
	}
}

Url PriorityQueue::dequeueURL(){
	Url url;

	url.setUrl("No URL");

	if (!this->list.empty()){
		url.setUrl(this->list.top().getUrl());
		cout << "\tDequeueing " << url.getUrl() << endl;
		this->size--;
		this->list.pop();
	}

	return url;
}

int PriorityQueue::getSize(){
	return this->size;
}