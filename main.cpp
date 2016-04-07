#include <iostream>
#include <fstream>
#include <chrono> // C++11 Time lib
#include <unordered_map> // C++11 Hash Table
#include <thread> // C++1 Threads
#include <mutex> // C++11 mutex
#include "PriorityQueue.h"

#define NUM_THREADS 20
#define FILENAME "htmls/html_files"
#define LIMIT_FILE_SIZE 300000000 // 300 MB
#define LIMIT_SIZE_URL 6
#define INITIAL_COLLECT 20
#define THREAD_QUEUE_SIZE 20

using namespace std;
using namespace std::chrono;

PriorityQueue urls_queue;
unordered_map<string, bool> visited_url; // Better off the PriotyQueue, because using threads, there will be less links to be "tested" in the queue
ofstream logs, html_files, status_log;
int index_file = 0;
CkString buffer;

mutex urls_queue_mutex, visited_url_mutex, log_mutex, buffer_mutex, status_log_mutex;

high_resolution_clock::time_point t0;


void crawling(int id);
void initializing_queue(vector<string> v);

int main(){
	int i;
	Url url;

	vector<string> initial_url = {"http://www.uol.com.br"};
	vector<thread> ths;

	logs.open("logs/log.csv", std::ofstream::out);
	logs << "time(ms),size(bytes)" << endl;

	status_log.open("logs/status_log.txt", std::ofstream::out);

	initializing_queue(initial_url);

	status_log << "Initial queue size: " << urls_queue.getSize() << endl;
	status_log << "Creating threads" << endl;

	t0 = high_resolution_clock::now();

	for (i = 0; i < NUM_THREADS; i++){
		ths.push_back(thread(&crawling, i));
	}

	for (auto& th : ths) {
		th.join();
	}

	logs.close();
	status_log.close();

	return 0;


	////////////////////////////////////////////////////////
}

void crawling(int id){
	int i, size_unspired, logging, dequeue_size;
	bool success;
	vector<Url> local_queue;
	// string logging;

	CkSpider spider;
	CkString ckurl, domain, html;

	high_resolution_clock::time_point t1, t2;

	urls_queue_mutex.lock();
	dequeue_size = (urls_queue.getSize() > THREAD_QUEUE_SIZE)? THREAD_QUEUE_SIZE : urls_queue.getSize(); 
	for (i=0; i<dequeue_size; i++){
		local_queue.push_back(urls_queue.dequeueURL());
	}
	status_log_mutex.lock();
	status_log << "Queue size: " << urls_queue.getSize() << endl;
	status_log_mutex.unlock();
	urls_queue_mutex.unlock();

	while (urls_queue.getSize()>0 || local_queue.size() > 0 ){
	// while (true){
		if (local_queue.empty()){
			urls_queue_mutex.lock();
			dequeue_size = (urls_queue.getSize() > THREAD_QUEUE_SIZE)? THREAD_QUEUE_SIZE : urls_queue.getSize(); 
			for (i = 0; i<dequeue_size; i++){
				local_queue.push_back(urls_queue.dequeueURL());
			}
			status_log_mutex.lock();
			status_log << "Queue size: " << urls_queue.getSize() << endl;
			status_log_mutex.unlock();
			urls_queue_mutex.unlock();
		}

		Url url;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		// urls_queue_mutex.lock();
		// url = urls_queue.dequeueURL();
		url = local_queue[0];
		local_queue.erase(local_queue.begin());
		// urls_queue_mutex.unlock();

		ckurl = url.getUrl().c_str();

		spider.GetUrlDomain(ckurl.getString(), domain);

		spider.Initialize(domain.getString());

		spider.AddUnspidered(ckurl.getString());

		success = spider.CrawlNext();

		if (success) { 
			// logging += "Evaluating " + url.getUrl() + "\n";
			// logging = url.getUrl();

			spider.get_LastHtml(html);

			logging = html.getSizeUtf8();

			buffer_mutex.lock();
			buffer.appendUtf8("/\\/\\ ");
			buffer.appendUtf8(url.getUrl().c_str());
			buffer.appendUtf8("\n\n");
			buffer.appendStr(html);

			if (buffer.getSizeUtf8() > LIMIT_FILE_SIZE){
				html_files.open(FILENAME+to_string(index_file));
				html_files << buffer.getString();
				buffer.clear();
				index_file++;
				html_files.close();
			}

			buffer_mutex.unlock();

			size_unspired = spider.get_NumUnspidered();
			for (i = 0; i < size_unspired; i++){
				spider.GetUnspideredUrl(0, ckurl);
				url.setUrl(ckurl);
				spider.SkipUnspidered(0);
				// f << i << ".\t" << url.getUrl() << endl;
				if (url.getSize() <= LIMIT_SIZE_URL){
					visited_url_mutex.lock();
					if (!visited_url[url.getNormalizedUrl()]){
						urls_queue_mutex.lock();
						urls_queue.queueURL(url);
						urls_queue_mutex.unlock();
						// visited_url.emplace(url.getUrl(), 1);
						visited_url[url.getNormalizedUrl()] =  true;
						// f << "Value: " << (int) visited_url[url.getUrl()] << endl;
						// cin >> aux;
					}
					visited_url_mutex.unlock();
				}
			}

			size_unspired = spider.get_NumOutboundLinks();

			for (i = 0; i < size_unspired; i++){
				spider.GetOutboundLink(i, ckurl);
				url.setUrl(ckurl);
				// f << i << ". " << ckurl.getString() << endl;
				visited_url_mutex.lock();
				if (url.getSize() <= LIMIT_SIZE_URL){
					if (!visited_url[url.getNormalizedUrl()]){
						urls_queue_mutex.lock();
						urls_queue.queueURL(url);
						urls_queue_mutex.unlock();
						// visited_url.emplace(url.getUrl(), 1);
						visited_url[url.getNormalizedUrl()] =  true;
					}
					visited_url_mutex.unlock();
				}
			}
			spider.ClearOutboundLinks();

			high_resolution_clock::time_point t2 = high_resolution_clock::now();

			auto duration = duration_cast<milliseconds>( t2 - t1 ).count();

			// if (url.getUrl() != ""){
				log_mutex.lock();
				// cout << "Evaluating " << logging << endl << "\tElapsed time: " << duration << " ms." << endl;
				logs << duration << "," << logging << endl;
				log_mutex.unlock();
			// }
		}
			
		// logging += "\tElapsed time: " + to_string(duration) + " ms." + "\n";

	}
	status_log_mutex.lock();

	high_resolution_clock::time_point tf = high_resolution_clock::now();

	auto duration = duration_cast<milliseconds>( tf - t0 ).count();

	status_log << "Thread " << to_string(i) << " is dead after " << duration << " ms of execution." << endl;
	status_log_mutex.unlock();
	// f.close();
}

void initializing_queue(vector<string> v){
	int i, size_unspired, logging, loop_control;
	bool success;
	// string logging;

	Url url;

	CkSpider spider;
	CkString ckurl, domain, html;

	for (i = 0; i < v.size(); i++){
		url.setUrl(v[i]);
		urls_queue.queueURL(url);
		visited_url[url.getNormalizedUrl()] = true;
	}

	// Url url;
	loop_control = 0;

	while(loop_control < INITIAL_COLLECT){
		// cout << i << " " << INITIAL_COLLECT <<   endl;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		url = urls_queue.dequeueURL();

		ckurl = url.getUrl().c_str();

		spider.GetUrlDomain(ckurl.getString(), domain);

		spider.Initialize(domain.getString());

		spider.AddUnspidered(ckurl.getString());

		success = spider.CrawlNext();

		if (success) { 
			// logging = url.getUrl();

			spider.get_LastHtml(html);

			logging = html.getSizeUtf8();

			buffer.appendUtf8("/\\/\\ ");
			buffer.appendUtf8(url.getUrl().c_str());
			buffer.appendUtf8("\n\n");
			buffer.appendStr(html);

			if (buffer.getSizeUtf8() > LIMIT_FILE_SIZE){
				html_files.open(FILENAME+to_string(index_file));
				html_files << buffer.getString();
				buffer.clear();
				index_file++;
				html_files.close();
			}

			size_unspired = spider.get_NumUnspidered();
			for (i = 0; i < size_unspired; i++){
				spider.GetUnspideredUrl(0, ckurl);
				url.setUrl(ckurl);
				spider.SkipUnspidered(0);
				// f << i << ".\t" << url.getUrl() << endl;
				if (url.getSize() <= LIMIT_SIZE_URL && url.isBrDomain()){
					if (!visited_url[url.getNormalizedUrl()]){
						urls_queue.queueURL(url);
						// visited_url.emplace(url.getUrl(), 1);
						visited_url[url.getNormalizedUrl()] =  true;
						// f << "Value: " << (int) visited_url[url.getUrl()] << endl;
						// cin >> aux;
					}
				}
			}

			size_unspired = spider.get_NumOutboundLinks();

			for (i = 0; i < size_unspired; i++){
				spider.GetOutboundLink(i, ckurl);
				url.setUrl(ckurl);
				// f << i << ". " << ckurl.getString() << endl;
				if (url.getSize() <= LIMIT_SIZE_URL && url.isBrDomain()){
					if (!visited_url[url.getNormalizedUrl()]){
						urls_queue.queueURL(url);
						// visited_url.emplace(url.getUrl(), 1);
						visited_url[url.getNormalizedUrl()] =  true;
					}
				}
			}

			spider.ClearOutboundLinks();

			high_resolution_clock::time_point t2 = high_resolution_clock::now();

			auto duration = duration_cast<milliseconds>( t2 - t1 ).count();

			logs << duration << "," << logging << endl;
		}

		loop_control++;
	}
}