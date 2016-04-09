#include <iostream>
#include <fstream>
#include <chrono> // C++11 Time lib
#include <unordered_map> // C++11 Hash Table
#include <thread> // C++1 Threads
#include <mutex> // C++11 mutex
#include "PriorityQueue.h"

#define NUM_THREADS 30
#define FILENAME "htmls/html_files"
#define LIMIT_FILE_SIZE 300000000 // 300 MB
#define LIMIT_SIZE_URL 6
#define INITIAL_COLLECT 20
#define THREAD_QUEUE_SIZE 20
#define LIMIT_MEM_LOG 1000
#define SIZE_LOCAL_QUEUE 100

using namespace std;
using namespace std::chrono;

PriorityQueue urls_queue;
unordered_map<string, bool> visited_url; // Better off the PriotyQueue, because using threads, there will be less links to be "tested" in the queue
ofstream logs, html_files, status_log;
int index_file = 0, log_entrance = 0;
string buffer, log_buffer;

mutex urls_queue_mutex, visited_url_mutex, log_mutex, buffer_mutex, status_log_mutex, cout_mutex;

high_resolution_clock::time_point t0;


void crawling(int id);
void initializing_queue(vector<string> v);

int main(){
	int i;
	Url url;

	vector<string> initial_url = {	"http://jogos.uol.com.br", "http://www.ojogos.com.br", "http://www.papajogos.com.br",
									"http://www.gamevicio.com", "http://g1.globo.com/tecnologia"	};
	vector<thread> ths;

	logs.open("logs/log.csv", std::ofstream::out);
	logs << "time(ms),size(bytes)" << endl;

	status_log.open("logs/status_log.txt", std::ofstream::out);

	buffer.append("|||");

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
	int i, size_unspired, logging, dequeue_size, vector_size;
	double duration;
	bool success;
	vector<Url> local_queue, local_to_queue;
	// string logging;

	CkSpider spider;
	CkString ckurl, domain, html;

	high_resolution_clock::time_point t1, t2, tf;

	// Initializing local queue
	urls_queue_mutex.lock();
	dequeue_size = (urls_queue.getSize() > THREAD_QUEUE_SIZE)? THREAD_QUEUE_SIZE : urls_queue.getSize(); 
	for (i=0; i<dequeue_size; i++){
		local_queue.push_back(urls_queue.dequeueURL());
	}
	urls_queue_mutex.unlock();

	// Updating status log
	status_log_mutex.lock();
	status_log << "Queue size: " << urls_queue.getSize() << endl;
	status_log_mutex.unlock();

	while (urls_queue.getSize()>0 || local_queue.size() > 0 ){

		// Checking if local queue is empty
		if (local_queue.empty()){
			urls_queue_mutex.lock();
			dequeue_size = (urls_queue.getSize() > THREAD_QUEUE_SIZE)? THREAD_QUEUE_SIZE : urls_queue.getSize(); 
			for (i = 0; i < dequeue_size; i++){
				local_queue.push_back(urls_queue.dequeueURL());
			}
			urls_queue_mutex.unlock();

			// Updating status log
			status_log_mutex.lock();
			status_log << "Queue size: " << urls_queue.getSize() << endl;
			status_log_mutex.unlock();
		}

		Url url;

		t1 = high_resolution_clock::now();

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
			// cout_mutex.lock();
			// cout << "buffer" << " mutex locked" << endl;
			// cout_mutex.unlock();
			buffer.append(" ");
			buffer.append(url.getUrl().c_str());
			buffer.append(" | ");
			buffer.append(" |||");
			buffer.append(html.getString());

			if (buffer.size() > LIMIT_FILE_SIZE){
				html_files.open(FILENAME+to_string(index_file));
				html_files << buffer;
				html_files.close();
				buffer.clear();
				index_file++;
			}

			// cout_mutex.lock();
			// cout << "buffer" << " mutex unlocked" << endl;
			// cout_mutex.unlock();

			buffer_mutex.unlock();

			size_unspired = spider.get_NumUnspidered();

			for (i = 0; i < size_unspired; i++){
				spider.GetUnspideredUrl(0, ckurl);
				url.setUrl(ckurl);
				spider.SkipUnspidered(0);

				if (url.getSize() <= LIMIT_SIZE_URL && url.getSize() > 0){
					visited_url_mutex.lock();
					// cout_mutex.lock();
					// cout << "visited_url" << " mutex locked" << endl;
					// cout_mutex.unlock();
					if (!visited_url[url.getNormalizedUrl()]){
						local_to_queue.push_back(url);
						visited_url[url.getNormalizedUrl()] =  true;
					} 
					// cout_mutex.lock();
					// cout << "visited_url" << " mutex unlocked" << endl;
					// cout_mutex.unlock();
					visited_url_mutex.unlock();
				}
			}

			size_unspired = spider.get_NumOutboundLinks();

			for (i = 0; i < size_unspired; i++){
				spider.GetOutboundLink(i, ckurl);
				url.setUrl(ckurl);

				if (url.getSize() <= LIMIT_SIZE_URL && url.isBrDomain() && url.getSize() > 0){
					visited_url_mutex.lock();
					// cout_mutex.lock();
					// cout << "visited_url" << " mutex locked" << endl;
					// cout_mutex.unlock();
					if (!visited_url[url.getNormalizedUrl()]){
						local_to_queue.push_back(url);
						visited_url[url.getNormalizedUrl()] =  true;
					}
					// cout_mutex.lock();
					// cout << "visited_url" << " mutex unlocked" << endl;
					// cout_mutex.unlock();
					visited_url_mutex.unlock();
				}
			}

			if(local_to_queue.size() >= SIZE_LOCAL_QUEUE){
				urls_queue_mutex.lock();
				// cout_mutex.lock();
				// cout << "urls_queue" << " mutex locked" << endl;
				// cout_mutex.unlock();
				vector_size = local_to_queue.size();
				for (i = 0; i < vector_size; i++){
					urls_queue.queueURL(local_to_queue.back());
					local_to_queue.pop_back();
				}
				// cout_mutex.lock();
				// cout << "urls_queue" << " mutex unlocked" << endl;
				// cout_mutex.unlock();
				urls_queue_mutex.unlock();
			}


			spider.ClearOutboundLinks();

			t2 = high_resolution_clock::now();

			duration = duration_cast<milliseconds>( t2 - t1 ).count();

			log_mutex.lock();
			// cout_mutex.lock();
			// cout << "log" << " mutex locked" << endl;
			// cout_mutex.unlock();
			
			// logs << duration << "," << logging << endl;

			log_buffer.append(to_string(duration));
			log_buffer.append(",");
			log_buffer.append(to_string(logging));
			log_buffer.append("\n");

			log_entrance++;

			if (log_entrance >= LIMIT_MEM_LOG){
				logs << log_buffer;
				log_buffer.clear();

			}

			// cout_mutex.lock();
			// cout << "log" << " mutex unlocked" << endl;
			// cout_mutex.unlock();
			log_mutex.unlock();
		}
	}

	status_log_mutex.lock();
	// cout_mutex.lock();
	// cout << "status_log" << " mutex locked" << endl;
	// cout_mutex.unlock();

	tf = high_resolution_clock::now();

	duration = duration_cast<milliseconds>( tf - t0 ).count();

	status_log << "Thread " << i << " is dead after " << duration << " ms of execution." << endl;
	// cout_mutex.lock();
	// cout << "status_log" << " mutex unlocked" << endl;
	// cout_mutex.unlock();
	status_log_mutex.unlock();

}

void initializing_queue(vector<string> v){
	int i, size_unspired, logging, loop_control;
	double duration;
	bool success;

	Url url;

	CkSpider spider;
	CkString ckurl, domain, html;

	high_resolution_clock::time_point t1, t2;

	for (i = 0; i < v.size(); i++){
		url.setUrl(v[i]);
		urls_queue.queueURL(url);
		visited_url[url.getNormalizedUrl()] = true;
	}

	loop_control = 0;

	while(loop_control < INITIAL_COLLECT){
	

		t1 = high_resolution_clock::now();

		url = urls_queue.dequeueURL();

		ckurl = url.getUrl().c_str();

		spider.GetUrlDomain(ckurl.getString(), domain);

		spider.Initialize(domain.getString());

		spider.AddUnspidered(ckurl.getString());

		success = spider.CrawlNext();

	

		if (success) { 
			spider.get_LastHtml(html);

			logging = html.getSizeUtf8();

			buffer.append(" ");
			buffer.append(url.getUrl());
			buffer.append(" | ");
			buffer.append(" |||");
			buffer.append(html.getString());

			if (buffer.size() > LIMIT_FILE_SIZE){
				html_files.open(FILENAME+to_string(index_file));
				html_files << buffer;
				html_files.close();
				buffer.clear();
				index_file++;
			}

			size_unspired = spider.get_NumUnspidered();
			for (i = 0; i < size_unspired; i++){
				spider.GetUnspideredUrl(0, ckurl);
				url.setUrl(ckurl);
				spider.SkipUnspidered(0);

				if (url.getSize() <= LIMIT_SIZE_URL && url.getSize() > 0){
					if (!visited_url[url.getNormalizedUrl()]){
						urls_queue.queueURL(url);
						visited_url[url.getNormalizedUrl()] =  true;
					}
				}

			}

			size_unspired = spider.get_NumOutboundLinks();

			for (i = 0; i < size_unspired; i++){
				spider.GetOutboundLink(i, ckurl);
				url.setUrl(ckurl);

				if (url.getSize() <= LIMIT_SIZE_URL && url.isBrDomain() && url.getSize() > 0){
					if (!visited_url[url.getNormalizedUrl()]){
						urls_queue.queueURL(url);
						visited_url[url.getNormalizedUrl()] =  true;
					}
				}

			}

			spider.ClearOutboundLinks();

			t2 = high_resolution_clock::now();

			duration = duration_cast<milliseconds>( t2 - t1 ).count();

			logs << duration << "," << logging << endl;
		}

		loop_control++;
	}
}