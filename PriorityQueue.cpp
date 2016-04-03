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
		if (this->queued_url[url.getUrl()] <= 0){
			this->list.push(url);
			// cout << "\tQueueing " << url.getUrl() << endl;
			this->size++;
			this->queued_url.emplace(url.getUrl(), 1);
			// cout << "Hashing: " << url.getUrl() << " " << this->queued_url[url.getUrl()] << endl;
		}
	}
}

Url PriorityQueue::dequeueURL(){
	Url url;

	// url.setUrl("No URL");

	if (!this->list.empty()){
		url.setUrl(this->list.top().getUrl());
		// cout << "\tDequeueing " << url.getUrl() << endl;
		this->size--;
		this->list.pop();
	}

	return url;
}

int PriorityQueue::getSize(){
	return this->size;
}