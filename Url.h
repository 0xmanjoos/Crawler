#ifndef URL_H
#define URL_H

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include "chilkat/CkSpider.h"
#include "chilkat/CkStringArray.h"

using namespace std;

class Url {

private:
	char* url;
	int size;

	char* cleaningURL(string url);
	
public:
	// Constructors
	Url();
	Url(char* url);

	// Destructor
	~Url();

	// Getters
	int getURLsize(CkString url);
	char* getUrl();

};

#endif