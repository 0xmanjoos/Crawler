#include "PriorityQueue.h"

// Constructors

PriorityQueue::PriorityQueue(){
}

// Destructor

PriorityQueue::~PriorityQueue(){
	// this->size = 0;
	// delete(&list);
	// del(list); // ?
}

void PriorityQueue::queueURL(string url){
	this->list.push_back(url);
}

string PriorityQueue::dequeueURL(){
	// Url url ((char*)"");
	string url;

	// url.setUrl("No URL");

	if (!this->list.empty()){
		url = this->list.back();
		// cout << "\tDequeueing " << url.getUrl() << endl;
		this->list.pop_back();
		this->list.shrink_to_fit();
	}

	return url;
}

int PriorityQueue::getSize(){
	return this->list.size();
}

bool PriorityQueue::empty(){
	return this->list.empty();
}

// void PriorityQueue::gettingSpace(){
// 	this->list.shrink_to_fit();
// }