#include <iostream>
// #include "chilkat/CkSpider.h"
#include "PriorityQueue.h"
// #include <unistd.h>
#include <chrono> // C++11 Time lib
#include <unordered_map> // C++11 Hash Table

using namespace std;
using namespace std::chrono;

int main(){
	int i, size_unspired;
	bool success;

	CkSpider spider;
	CkString ckurl, domain;

	Url url;
	PriorityQueue queue;
	
	unordered_map<string, int> visited_url; // Better off the PriotyQueue, because using threads, there will be less links to be "tested" in the queue

	high_resolution_clock::time_point t1, t2;
	
	// url.setUrl("http://www.chilkatsoft.com/crawlStart.html");
	url.setUrl("http://www.joelonsoftware.com");
	queue.queueURL(url);

	// visited_url.emplace(url.getUrl(), 1);
	url.setUrl("http://stackoverflow.com/");
	visited_url.emplace("http://stackoverflow.com/", 1);
	// 

	cout << visited_url["nana"] << " " << visited_url[url.getUrl()]  << endl;

	if (!visited_url["http://stackoverflow.com/"]){
		cout << "nana" << endl;
		queue.queueURL(url);
	}

	while (queue.getSize() > 0){
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		url = queue.dequeueURL();
		// if (!visited_url[url.getUrl()]){
		if (true){
			visited_url.emplace(url.getUrl(), 1);
			// cout << url.getCleanUrl() << endl;

			ckurl = url.getUrl().c_str();

			spider.GetUrlDomain(ckurl.getString(), domain);

			spider.Initialize(domain.getString());

			spider.AddUnspidered(ckurl.getString());

			success = spider.CrawlNext();

			if (success) { 
				cout << "Evaluating " << url.getUrl() << endl;

				// cout << spider.get_NumUnspidered() << endl;
				// cout << spider.get_NumOutboundLinks() << endl;

				// char aux;
				size_unspired = spider.get_NumUnspidered();
				for (i = 0; i < size_unspired; i++){
					spider.GetUnspideredUrl(0, ckurl);
					url.setUrl(ckurl);
					spider.SkipUnspidered(0);
					// cout << i << ".\t" << url.getUrl() << endl;
					if (!visited_url[url.getNormalizedUrl()]){
						queue.queueURL(url);
						// visited_url.emplace(url.getUrl(), 1);
						visited_url[url.getNormalizedUrl()] =  1;
						// cout << "Value: " << (int) visited_url[url.getUrl()] << endl;
						// cin >> aux;
					}
				}

				// cout << endl << endl;

				size_unspired = spider.get_NumOutboundLinks();

				for (i = 0; i < size_unspired; i++){
					spider.GetOutboundLink(i, ckurl);
					url.setUrl(ckurl);
					// cout << i << ". " << ckurl.getString() << endl;
					if (!visited_url[url.getNormalizedUrl()]){
						queue.queueURL(url);
						// visited_url.emplace(url.getUrl(), 1);
						visited_url[url.getNormalizedUrl()] =  1;
					}
				}

				// cout << "=============" << endl << endl;

				// cout << endl << endl;

				// i = 0;

				spider.ClearOutboundLinks();
				// cout << spider.get_NumOutboundLinks() << endl;
				// cout << spider.get_NumUnspidered() << endl;
				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>( t2 - t1 ).count();
				cout << "\tElapsed time: " << duration << " ms." << endl << endl;
			}

		}
	}

}

// int main()
//     {
//     //  The Chilkat Spider component/library is free.
//     CkSpider spider;
//     const char *url = "http://www.chilkatsoft.com/crawlStart.html";
//     const char *domain = "www.chilkatsoft.com";
    
//     printf("Started");//point 1

//     //  The spider object crawls a single web site at a time.  As you'll see
//     //  in later examples, you can collect outbound links and use them to
//     //  crawl the web.  For now, we'll simply spider 10 pages of chilkatsoft.com
//     spider.Initialize(domain);

//      printf("Started");//point 2

//     //  Add the 1st URL:
//     spider.AddUnspidered(url);


//     //  Begin crawling the site by calling CrawlNext repeatedly.
//     long i;
//     for (i = 0; i <= 9; i++) {
//         bool success;
//         success = spider.CrawlNext();
//         if (success == true) {
//             //  Show the URL of the page just spidered.
//             printf("%s\n",spider.lastUrl());
//             //  The HTML is available in the LastHtml property
//         }
//         else {
//             //  Did we get an error or are there no more URLs to crawl?
//             if (spider.get_NumUnspidered() == 0) {
//                 printf("No more URLs to spider\n");
//             }
//             else {
//                 printf("%s\n",spider.lastErrorText());
//             }

//         }

//         //  Sleep 1 second before spidering the next URL.
//         spider.SleepMs(1000);
//         printf("End");
//     }

//     printf("\n");
//     }

// int main(){

//     PriorityQueue queue;
//     // Url url;

//     Url url;

//     url.setUrl("https://ufmg.br");
//     cout << "Avaliating " << url.getUrl() << endl;
//     cout << "URL size: " << url.getSize() << endl << endl;
//     queue.queueURL(url);

// 	url.setUrl("http://www.ufmg.br");
//     cout << "Avaliating " << url.getUrl() << endl;
// 	cout << "URL size: " << url.getSize() << endl << endl;
// 	queue.queueURL(url);

// 	url.setUrl("http://dcc.ufmg.br/~nivio/br/teaching-ri-16.php");
// 	cout << "Avaliating " << url.getUrl() << endl;
// 	cout << "URL size: " << url.getSize() << endl << endl;
// 	queue.queueURL(url);

// 	url.setUrl("www.ufmg.br");
// 	cout << "Avaliating " << url.getUrl() << endl;
// 	cout << "URL size: " << url.getSize() << endl << endl;
// 	queue.queueURL(url);

// 	url.setUrl("http://dcc.ufmg.br/~nivio");
// 	cout << "Avaliating " << url.getUrl() << endl;
// 	cout << "URL size: " << url.getSize() << endl << endl;
// 	queue.queueURL(url);

// 	url.setUrl("http://dcc.ufmg.br/~nivio/teaching");
// 	cout << "Avaliating " << url.getUrl() << endl;
// 	cout << "URL size: " << url.getSize() << endl << endl;
// 	queue.queueURL(url);

// 	url.setUrl("");
// 	cout << "Avaliating " << url.getUrl() << endl;
// 	cout << "URL size: " << url.getSize() << endl << endl;
// 	queue.queueURL(url);

// 	cout << "Dequeueing" << endl;
// 	while(queue.getSize()>0){
// 		url = queue.dequeueURL();
// 	}

//     return 0;
// }