#include "Url.h"

// Splits a string "s" in "c"
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

// Checks if there is ".br" in domain
bool isBrDomain(string url){
	string domain = getDomain(url);
	string delimitation (".br");

	if (domain.back() == '/'){
		domain.pop_back();
	}

	size_t found = domain.find(delimitation);

	if (found == (domain.size()-delimitation.size())){
		return true;
	}

	delimitation = "http://";

	found = domain.find(delimitation);

	if (found!=std::string::npos){
		if (!found){
			domain = domain.erase(found,delimitation.size());
		}
	}

	delimitation = "br.";

	found = domain.find(delimitation);

	if (found!=std::string::npos){
		return true;
	}

	return false;


}

// Normalizes URL, removing "www." and last "/"
string getNormalizedUrl(string url){
	string delimitation ("www.");

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


// Gets URL's domain
string getDomain(string url){
	CkSpider spider;
	CkString domain;
	string real_domain;

	spider.GetUrlDomain(url.c_str(), domain);

	real_domain = domain.getString();

	return real_domain;
}

// Calculates URL size
int getURLsize(string url){
	CkString canonicalized_url;
	string s;
	int i = 0, size = 0;
	vector<string> tokens;

	// Remove "http[s]?://" from url
	s = getCleanUrl(url);

	// If url is empty, invalid url, returns 0
	if (s.size() <= 0){
		return 0;
	}

	if (s.back() != '/'){
		s.append("/");
	}

	/* Couting "/" */

	// Splits url in "/"
	split(s, '/', tokens);

	// Removes any empty token
	auto it = begin(tokens);
	while(it != end(tokens)){
		if (tokens[i].size() <= 0){
			tokens.erase(tokens.cbegin()+i);
		} else {
			++it;
			i++;
		}
	}

	// Number of tokens means number os depth in path.
	//-1 because first part of URL (before first /) will also be analyzed
	size += tokens.size() - 1;

	// Gets first part of URL (before first /)
	if (!tokens.empty()){
		s = tokens[0];
	} else {
		s = "";
	}

	tokens.clear();

	/* Couting "." */

	// Splits on "."
	split(s, '.', tokens);


	// Removes "www"
	if(!tokens.empty()){
		if (tokens[0].compare("www") == 0){
			tokens.erase(tokens.cbegin());
		}
	}

	// Removes any empty token
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

	// Number of tokens is number of "." of first part of URL (before first /)
	size += tokens.size();

	return size;

}