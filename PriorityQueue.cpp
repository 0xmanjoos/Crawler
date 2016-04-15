#include "PriorityQueue.h"

// Constructors
PriorityQueue::PriorityQueue(){
}

// Destructor
PriorityQueue::~PriorityQueue(){
}

void PriorityQueue::push(string url){
	this->list.push_back(url);
	push_heap(this->list.begin(), this->list.end(), CompareURL());
}

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

string PriorityQueue::getFromVector(int i){
	string url;

	if (this->list.size() >= i){
		url = this->list[i];
	}

	return url;
}

string PriorityQueue::popFromVector(){
	string url;

	if (!this->list.empty()){
		url = this->list.front();
		this->list.erase(this->list.cbegin());
	}

	return url;
}

void PriorityQueue::clear(){
	this->list.clear();
	this->list.shrink_to_fit();
}