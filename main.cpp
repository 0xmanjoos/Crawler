#include <iostream>
#include <stdlib.h>
// #include "chilkat/CkSpider.h"
#include "PriorityQueue.h"
#include "Url.h"
#include <unistd.h>

using namespace std;


int main(){
	CkSpider spider;
	CkString ckurl, domain;
	Url url;
	// string url;
	bool success;
	int i, size_unspired;
	PriorityQueue queue;
	vector<Url> waitlist;
	//- Url url;

	ckurl = "http://www.joelonsoftware.com/";

	spider.GetUrlDomain(ckurl.getString(), domain);

	spider.Initialize(domain.getString());

	spider.AddUnspidered(ckurl.getString());

	success = spider.CrawlNext();

	if (success) { 

		cout << spider.get_NumUnspidered() << endl;
		cout << spider.get_NumOutboundLinks() << endl;

		size_unspired = spider.get_NumUnspidered();
		for (i = 0; i < size_unspired; i++){
			spider.GetUnspideredUrl(0, ckurl);
			url.setUrl(ckurl);

			spider.SkipUnspidered(0);

			cout << i << ".\t" << url.getUrl() << endl;
			queue.queueURL(url);
		}

		cout << endl << endl;

		size_unspired = spider.get_NumOutboundLinks();

		for (i = 0; i < size_unspired; i++){
			spider.GetOutboundLink(i, ckurl);
			url.setUrl(ckurl);
			cout << i << ". " << ckurl.getString() << endl;
			queue.queueURL(url);
		}

		cout << "=============" << endl << endl;

		cout << endl << endl;

		i = 0;
		while (queue.getSize() > 0){
			url = queue.dequeueURL();
			cout << i << ". " << url.getUrl() << endl;
			i++;
		}

		spider.ClearOutboundLinks();
		cout << spider.get_NumOutboundLinks() << endl;
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