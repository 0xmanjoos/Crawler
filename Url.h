#ifndef URL_H
#define URL_H


#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include <memory>
#include "chilkat/include/CkSpider.h"
#include "chilkat/include/CkString.h"
#include "chilkat/include/CkStringArray.h"

using namespace std;

void split(const string& s, char c, vector<string>& v);
string getCleanUrl(string url);
bool isBrDomain(string url);
string canonicalizeUrl(string url);
int getURLsize(string url);
string getNormalizedUrl(string url);
string getDomain(string url);

#endif