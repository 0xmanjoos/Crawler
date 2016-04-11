#include "Url.h"

// Remove "http[s]?://" from url
char* getCleanUrl(string url){
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

	// Converting string to char*
	char* final_url = new char[new_url.size()+1];
	std::copy(new_url.begin(), new_url.end(), final_url);
	final_url[new_url.size()] = '\0';

	return final_url;
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
	CkString canonicalized_url, domain_url, clean_url, path_url, aux_ckstr;
	string aux_str;
	CkStringArray *aux;
	int domain_size = 0, size = 0;

	canonicalized_url = canonicalizeUrl(url).c_str();
	
	// printf("\tURL: %s\n", url.getString());
	// printf("\tCanonicalized URL: %s\n", canonicalized_url.getString());

	clean_url = getCleanUrl((char*) canonicalized_url.getString());
	// cout << "\tClean URL: " << clean_url.getString() << endl;

	// Check if there is something after "http[s]?://""
	if (clean_url.getNumChars() <= 0){
		return 0;
	}

	// 
	// /* Separating path */
	// aux_str = string(clean_url.getString());
	// aux_str = aux_str.erase(0,string(domain_url).size()+1);

	// cout << "\tPath: " << aux_str << endl;
	// 

	// Counting path's size (www.ufmg.br -> size = 3)	
	aux = clean_url.split('/', true, true, false);
	size += aux->get_Count()-1; // -1 because it is ignoring the domain, which will be considered later

	// Counting domain's size (www.ufmg.br -> size = 3)
	aux->StrAt(0, aux_ckstr);
	aux = aux_ckstr.split('.', true, true, false);
	size += aux->get_Count();

	// printf("\tURL size: %d\n\n", size);


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