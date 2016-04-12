#include "PriorityQueue.h"

// Constructors
PriorityQueue::PriorityQueue(){
}

// Destructor
PriorityQueue::~PriorityQueue(){
}

void PriorityQueue::queueURL(string url){
	this->list.push(url);
}

string PriorityQueue::dequeueURL(){
	string url;

	if (!this->list.empty()){
		url = this->list.top();
		// cout << "\tDequeueing " << url.getUrl() << endl;
		this->list.pop();
	}

	return url;
}

int PriorityQueue::getSize(){
	return this->list.size();
}

bool PriorityQueue::empty(){
	return this->list.empty();
}