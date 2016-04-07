#ifndef URL_H
#define URL_H


#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include "chilkat/include/CkSpider.h"
#include "chilkat/include/CkString.h"
#include "chilkat/include/CkStringArray.h"

using namespace std;

class Url {

private:
	string url;
	char size;

	char* cleaningURL(string url);
	int getURLsize(string url);
	string canonicalizeUrl(string url);
	
public:
	// Constructors
	Url();
	Url(char* url);
	Url(string url);
	Url(CkString url);
	Url(Url const& url);

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
	string getCleanUrl();
	int getSize();
	int getSize() const;
	string getNormalizedUrl();

	string getDomain();

	bool isBrDomain();

};

#endif