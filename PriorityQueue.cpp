#include "PriorityQueue.h"

// Constructors
PriorityQueue::PriorityQueue(){
}

// Destructor
PriorityQueue::~PriorityQueue(){
}

void PriorityQueue::push(string url){
	if (url.size()>0){
		this->list.push_back(url);
		push_heap(this->list.begin(), this->list.end(), CompareURL());
	}
}

// In case queue is empty, returns an empty string
string PriorityQueue::pop(){
	string url = "";

	if (!this->list.empty()){
		url = this->list.front();

		pop_heap(this->list.begin(), this->list.end(), CompareURL());

		this->list.pop_back();
	}

	return url;
}

int PriorityQueue::size(){
	return this->list.size();
}

bool PriorityQueue::empty(){
	return this->list.empty();
}

// In case i isn't in queue, returns empty string
string PriorityQueue::getFromVector(int i){
	string url = "";

	if (this->list.size() >= i){
		url = this->list[i];
	}

	return url;
}

// Returns first element in vector
string PriorityQueue::popFromVector(){
	string url;

	if (!this->list.empty()){
		url = this->list.front();
		this->list.erase(this->list.cbegin());
	}

	return url;
}

// Remove all elements
void PriorityQueue::clear(){
	this->list.clear();
	this->list.shrink_to_fit();
}