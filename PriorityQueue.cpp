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

// void PriorityQueue::queueURL(Url url){
void PriorityQueue::queueURL(string url){
	// if (url.getSize() > 0){
	if (url.size() > 0){
		this->list.push(url);
		cout << "\tQueueing " << url << endl;
		this->size++;
	}
}

// Url PriorityQueue::dequeueURL(){
string PriorityQueue::dequeueURL(){
	// Url url;

	// url.setUrl("No URL");

	string url;

	if (!this->list.empty()){
		// url.setUrl(this->list.top().getUrl());
		url = this->list.top();
		// cout << "\tDequeueing " << url.getUrl() << endl;
		cout << "\tDequeueing " << url << endl;
		this->size--;
		this->list.pop();
	}

	return url;
}

int PriorityQueue::getSize(){
	return this->size;
}