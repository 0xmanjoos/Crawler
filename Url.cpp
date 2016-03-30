#include "Url.h"

// Constructors

Url::Url(){
	this->size = 0;

	// CkString url;

	// url = "https://ufmg.br";
	// cout << "Avaliating " << url.getString() << endl;
	// cout << "URL size: " << this->getURLsize(url) << endl << endl;

	// url = "http://www.ufmg.br";
	// cout << "Avaliating " << url.getString() << endl;
	// cout << "URL size: " << this->getURLsize(url) << endl << endl;

	// url = "www.ufmg.br";
	// cout << "Avaliating " << url.getString() << endl;
	// cout << "URL size: " << this->getURLsize(url) << endl << endl;

	// url = "http://dcc.ufmg.br/~nivio";
	// cout << "Avaliating " << url.getString() << endl;
	// cout << "URL size: " << this->getURLsize(url) << endl << endl;

	// url = "http://dcc.ufmg.br/~nivio/teaching";
	// cout << "Avaliating " << url.getString() << endl;
	// cout << "URL size: " << this->getURLsize(url) << endl << endl;

	// url = "http://dcc.ufmg.br/~nivio/br/teaching-ri-16.php";
	// cout << "Avaliating " << url.getString() << endl;
	// cout << "URL size: " << this->getURLsize(url) << endl << endl;

	// url = "";
	// cout << "Avaliating " << url.getString() << endl;
	// cout << "URL size: " << this->getURLsize(url) << endl << endl;
}

Url::Url(char* url){
	this->url = url;
	this->size = this->getURLsize(url);
}

// Destructor

Url::~Url(){
	// this->size = 0;
	// delete(&list);
	// del(list); // ?
}

int Url::getURLsize(CkString url){
	CkString canonicalized_url, domain_url, clean_url, path_url, aux_ckstr;
	string aux_str;
	CkStringArray *aux;
	int domain_size = 0, size = 0;
	CkSpider spider;

	spider.CanonicalizeUrl(url.getString(), canonicalized_url); // Canonicalizing URL
	
	// printf("\tURL: %s\n", url.getString());
	printf("\tCanonicalized URL: %s\n", canonicalized_url.getString());

	clean_url = this->cleaningURL(url.getString());
	cout << "\tClean URL: " << clean_url.getString() << endl;

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

// Remove "http[s]?://" from url
char* Url::cleaningURL(string url){
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


char* Url::getUrl(){
	return this->url;
}

int Url::getSize(){
	int size = this->size;
	return size;
}

void Url::setUrl(char* url){
	this->url = url;
	this->size = this->getURLsize(url);
}

void Url::setUrl(const char* url){
	size_t len = strlen(url);
	this->url = new char[len+1];
	strncpy(this->url, url, len);
	this->url[len] = '\0';
	this->size = this->getURLsize(url);
}

char* Url::getUrl() const {
	return this->url;
}

int Url::getSize() const {
	return size;
}