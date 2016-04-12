#include "Url.h"

void split(const string& s, char c, vector<string>& v) {
	string::size_type i = 0;
	string::size_type j = s.find(c);

	while (j != string::npos) {
		v.push_back(s.substr(i, j-i));
		i = ++j;
		j = s.find(c, j);

		if (j == string::npos)
		v.push_back(s.substr(i, s.length()));
	}
}

// Remove "http[s]?://" from url
string getCleanUrl(string url){
	string http ("http");
	string delimitation ("://");
	string new_url = url;
	size_t found = url.find(http); // Locate the position where "http" starts in the url

	// Test if "http" is within the url
	if (found!=std::string::npos){
		// Teste if "http" starts in the beginning of the url
		if (!found){
			// Locate the position where "://" starts in the url
			found = url.find(delimitation, http.size());
			// Tests if "://" is within the url
			if (found!=std::string::npos){
				// Remove "http[s]?://" from the url
				new_url = url.erase(0,found+delimitation.size());
			}
		}

	}

	return new_url;
}

bool isBrDomain(string url){
	string domain = getDomain(url);
	string delimitation (".br");

	if (domain.back() == '/'){
		domain.pop_back();
	}

	size_t found = domain.find(delimitation);

	return (found == (domain.size()-delimitation.size()));

}

//Also removes "www."
string canonicalizeUrl(string url){
	CkString new_url;
	CkSpider spider;

	spider.CanonicalizeUrl(url.c_str(), new_url); // Canonicalizing URL

	return ((char*) new_url.getString());

}

int getURLsize(string url){
	CkString canonicalized_url;
	string s;
	int i = 0, size = 0;
	vector<string> tokens;

	canonicalized_url = canonicalizeUrl(url).c_str();
	
	s = getCleanUrl(canonicalized_url.getString());

	if (s.size() <= 0){
		return 0;
	}

	split(s, '/', tokens);


	auto it = begin(tokens);
	while(it != end(tokens)){
		if (tokens[i].size() <= 0){
			tokens.erase(tokens.cbegin()+i);
		} else {
			++it;
			i++;
		}
	}


	size += tokens.size() - 1;

	s = tokens[0];
	tokens.clear();
	split(s, '.', tokens);

	if (tokens[0].compare("www") == 0){
		tokens.erase(tokens.cbegin());
	}

	it = begin(tokens);
	i = 0;
	while(it != end(tokens)){
		if (tokens[i].size() <= 0){
			tokens.erase(tokens.cbegin()+i);
		} else {
			++it;
			i++;	
		}
	}

	size += tokens.size();


	return size;

}


string getNormalizedUrl(string url){
	string delimitation ("www.");

	url = canonicalizeUrl(url);

	// return url;

	size_t found = url.find(delimitation); // Locate the position where "www." starts in the url
	// Test if "www." is within the url
	if (found!=std::string::npos){
		// Teste if "www." starts in the beginning of the url, or after "http://" or "https://"
		if ((!found)||(found >= 7 && found <= 8)){
			url = url.erase(found,delimitation.size());
		}
	}

	if (url.back() == '/'){
		url.pop_back();
	}

	return url;
}

string getDomain(string url){
	CkSpider spider;
	CkString domain;

	spider.GetUrlDomain(url.c_str(), domain);

	return domain.getString();
}