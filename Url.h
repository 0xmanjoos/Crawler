#ifndef URL_H
#define URL_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include "chilkat/CkSpider.h"
#include "chilkat/CkString.h"
#include "chilkat/CkStringArray.h"

using namespace std;

class Url {

private:
	string url;
	int size;

	char* cleaningURL(string url);
	int getURLsize(char* url);
	
public:
	// Constructors
	Url();
	Url(char* url);
	Url(string url);
	Url(CkString url);

	// Destructor
	~Url();

	// Setters
	void setUrl(char* url);
	void setUrl(string url);
	void setUrl(const char* url);
	void setUrl(CkString url);

	// Getters
	string getUrl();
	string getUrl() const;
	int getSize();
	int getSize() const;

};

#endif