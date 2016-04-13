#include <iostream>
#include <fstream>
#include <chrono> // C++11 Time lib
#include <unordered_map> // C++11 Hash Table
#include <thread> // C++1 Threads
#include <mutex> // C++11 mutex
#include "PriorityQueue.h"

#define NUM_THREADS 20
#define LIMIT_SIZE_URL 6
#define INITIAL_COLLECT 20

#define THREAD_QUEUE_SIZE 20
#define SIZE_LOCAL_QUEUE 100

#define HTML_FILENAME "htmls/html_files"
#define LIMIT_HTML_FILE_SIZE 500000000 // 300 MB

#define LIMIT_MEM_LOG 1000

#define BACKUP_QUEUE_SIZE 50000
#define KEEPING_FROM_BACKUP 5000
#define BACKUP_QUEUE_FILENAME "backup/queue"

const std::chrono::seconds SLEEP_TIME(30);

using namespace std;
using namespace std::chrono;

PriorityQueue urls_queue;
unordered_map<string, bool> queued_url; // Better off the PriotyQueue, because using threads, there will be less links to be "tested" in the queue
ofstream logs, status_log, backup_queue, html_files[NUM_THREADS];
ifstream reading_backup_queue;
int index_file = 0, log_entrance = 0;
string log_buffer;

mutex urls_queue_mutex, queued_url_mutex, log_mutex, status_log_mutex, cout_mutex;

high_resolution_clock::time_point t0;


void crawling(int id, string buffer);
string initializing_queue(vector<string> v);
void backingup_queue();
void restoring_backup();

int main(){
	int i;

	string buffer, filename;

	vector<string> initial_url = {	"http://jogos.uol.com.br", "http://www.ojogos.com.br", "http://www.papajogos.com.br",
									"http://www.gamevicio.com", "http://g1.globo.com/tecnologia", "http://globo.com"	};
	vector<thread> ths;

	// cout << "here";

	logs.open("logs/log.csv", std::ofstream::out);
	logs << "time from beginning(s),time spent(ms),size(bytes)" << endl;

	status_log.open("logs/status_log.txt", std::ofstream::out);

	// backup_queue.open(BACKUP_QUEUE_FILENAME, ios::out);
	// backup_queue.close();
	// reading_backup_queue.open(BACKUP_QUEUE_FILENAME);

	buffer.append("|||");

	t0 = high_resolution_clock::now();

	buffer = initializing_queue(initial_url);

	status_log << "Initial queue size: " << urls_queue.getSize() << endl;
	status_log << "Creating threads" << endl;

	for (i = 0; i < NUM_THREADS; i++){
		// Opening files
		filename.append(HTML_FILENAME);
		filename.append("-");
		filename.append(to_string(i));
		filename.append("-0");
		html_files[i].open(filename, ios::out);
		html_files[i] << "|||";
		html_files[i].close();
		filename.clear();

		// Creating threads
		ths.push_back(thread(&crawling, i, buffer));
		buffer.clear();
	}

	for (auto& th : ths) {
		th.join();
	}

	for(i = 0; i < NUM_THREADS; i++){
		html_files[i].close();
	}

	logs.close();
	status_log.close();
	backup_queue.close();
	reading_backup_queue.close();

	return 0;


	////////////////////////////////////////////////////////
}

void crawling(int id, string buffer){
	int i, size_unspired, logging, dequeue_size, vector_size, url_size;
	int file_index, file_size = buffer.size();
	double duration, total_duration;
	bool success, havent_slept = true;

	string url, filename;

	vector<string> local_queue, local_to_queue;

	CkSpider spider;
	CkString ckurl, domain, html;

	high_resolution_clock::time_point t1, t2, tf;


	filename.append(HTML_FILENAME);
	filename.append("-");
	filename.append(to_string(id));
	filename.append("-0");
	html_files[i].open(filename, ios::out | ios::app);
	filename.clear();

	// Initializing local queue
	urls_queue_mutex.lock();
	dequeue_size = (urls_queue.getSize() > THREAD_QUEUE_SIZE)? THREAD_QUEUE_SIZE : urls_queue.getSize(); 
	for (i=0; i<dequeue_size; i++){
		local_queue.push_back(urls_queue.dequeueURL());
	}
	urls_queue_mutex.unlock();

	// Updating status log
	status_log_mutex.lock();
	t2 = high_resolution_clock::now();

	total_duration = duration_cast<seconds>( t2 - t0 ).count();
	status_log << "Queue size: " << urls_queue.getSize() << " (" << total_duration << " s)" << endl;
	status_log_mutex.unlock();

	while (!urls_queue.empty() || !local_queue.empty() || havent_slept ){
		if (!urls_queue.empty() || !local_queue.empty()){
			havent_slept = true;
			// Checking if local queue is empty
			if (local_queue.empty()){
				local_queue.shrink_to_fit();
				urls_queue_mutex.lock();
				dequeue_size = (urls_queue.getSize() > THREAD_QUEUE_SIZE)? THREAD_QUEUE_SIZE : urls_queue.getSize(); 
				for (i = 0; i < dequeue_size; i++){
					local_queue.push_back(urls_queue.dequeueURL());
				}
				urls_queue_mutex.unlock();

				// Updating status log
				status_log_mutex.lock();
				t2 = high_resolution_clock::now();

				total_duration = duration_cast<seconds>( t2 - t0 ).count();
				status_log << "Queue size: " << urls_queue.getSize() << " (" << total_duration << " s)" << endl;
				status_log_mutex.unlock();
			}

			// urls_queue_mutex.lock();
			// if (urls_queue.getSize() >= BACKUP_QUEUE_SIZE){
			// 	backingup_queue();
			// }
			// urls_queue_mutex.unlock();

			t1 = high_resolution_clock::now();

			// urls_queue_mutex.lock();
			// url = urls_queue.dequeueURL();
			url = local_queue[0];
			local_queue.erase(local_queue.begin());
			// urls_queue_mutex.unlock();

			ckurl = url.c_str();

			spider.GetUrlDomain(ckurl.getString(), domain);

			spider.Initialize(domain.getString());

			spider.AddUnspidered(ckurl.getString());

			success = spider.CrawlNext();

			if (success) { 
				// logging += "Evaluating " + url.getUrl() + "\n";
				// logging = url.getUrl();

				spider.get_LastHtml(html);

				logging = html.getSizeUtf8();

				buffer.append(" ");
				buffer.append(url);
				buffer.append(" | ");
				buffer.append(html.getString());
				buffer.append(" |||");

				file_size+=buffer.size();

				html_files[id] << buffer;

				buffer.clear();
				buffer.shrink_to_fit();

				if (file_size >= LIMIT_HTML_FILE_SIZE){
					html_files[id].close();
					file_size = 0;
					file_index++;
					filename.append(HTML_FILENAME);
					filename.append("-");
					filename.append(to_string(i));
					filename.append("-");
					filename.append(to_string(file_index));
					html_files[i].open(filename, ios::out | ios::app);
				}

				size_unspired = spider.get_NumUnspidered();

				for (i = 0; i < size_unspired; i++){
					spider.GetUnspideredUrl(0, ckurl);
					url = getNormalizedUrl(ckurl.getString());
					spider.SkipUnspidered(0);

					url_size = getURLsize(url);
					if (url_size > 0 && url_size <= LIMIT_SIZE_URL){
						// queued_url_mutex.lock();
						// cout_mutex.lock();
						// cout << "queued_url" << " mutex locked" << endl;
						// cout_mutex.unlock();
						if (!queued_url[url]){
							local_to_queue.push_back(url);
							// queued_url[url] =  true;
						} 
						// cout_mutex.lock();
						// cout << "queued_url" << " mutex unlocked" << endl;
						// cout_mutex.unlock();
						// queued_url_mutex.unlock();
					}
				}

				size_unspired = spider.get_NumOutboundLinks();

				for (i = 0; i < size_unspired; i++){
					spider.GetOutboundLink(i, ckurl);
					url = getNormalizedUrl(ckurl.getString());

					url_size = getURLsize(url);
					if (url_size > 0 && url_size <= LIMIT_SIZE_URL && isBrDomain(url)){
						// queued_url_mutex.lock();
						// cout_mutex.lock();
						// cout << "queued_url" << " mutex locked" << endl;
						// cout_mutex.unlock();
						if (!queued_url[url]){
							local_to_queue.push_back(url);
							// queued_url.erase(url);
						}
						// cout_mutex.lock();
						// cout << "queued_url" << " mutex unlocked" << endl;
						// cout_mutex.unlock();
						// queued_url_mutex.unlock();
					}
				}

				if(local_to_queue.size() >= SIZE_LOCAL_QUEUE || urls_queue.empty()){
					if (urls_queue.getSize() <= BACKUP_QUEUE_SIZE){
						vector_size = local_to_queue.size();
						queued_url_mutex.lock();
						urls_queue_mutex.lock();
						// cout_mutex.lock();
						// cout << "urls_queue" << " mutex locked" << endl;
						// cout_mutex.unlock();
						for (i = 0; i < vector_size; i++){
							url = local_to_queue.back();
							if(!queued_url[url]){
								urls_queue.queueURL(url);
								queued_url[url] = true;
							}
							local_to_queue.pop_back();
						}
						// cout_mutex.lock();
						// cout << "urls_queue" << " mutex unlocked" << endl;
						// cout_mutex.unlock();
						urls_queue_mutex.unlock();
						queued_url_mutex.unlock();
					}
					local_to_queue.clear();
					local_to_queue.shrink_to_fit();
				}


				spider.ClearOutboundLinks();

				t2 = high_resolution_clock::now();

				duration = duration_cast<milliseconds>( t2 - t1 ).count();
				total_duration = duration_cast<seconds>( t2 - t0 ).count();

				log_mutex.lock();
				// cout_mutex.lock();
				// cout << "log" << " mutex locked" << endl;
				// cout_mutex.unlock();
				
				// logs << duration << "," << logging << endl;

				log_buffer.append(to_string(total_duration));
				log_buffer.append(",");
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
		} else {
			havent_slept = false;
			cout << "Sleeping" << endl;
			urls_queue_mutex.lock();
			restoring_backup();
			urls_queue_mutex.unlock();
		}

		spider.ClearSpideredUrls();

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

string initializing_queue(vector<string> v){
	int i, size_unspired, logging, loop_control, url_size;
	double duration, total_duration;
	bool success;

	string url, buffer;

	CkSpider spider;
	CkString ckurl, domain, html;

	high_resolution_clock::time_point t1, t2;

	for (i = 0; i < v.size(); i++){
		urls_queue.queueURL(getNormalizedUrl(v[i]));
		// cout << getNormalizedUrl(v[i]) << endl;
		// queued_url[getNormalizedUrl(v[i])] = true;
	}

	loop_control = 0;

	while(loop_control < INITIAL_COLLECT && !urls_queue.empty()){
	

		t1 = high_resolution_clock::now();

		url = urls_queue.dequeueURL();

		ckurl = url.c_str();

		spider.GetUrlDomain(ckurl.getString(), domain);

		spider.Initialize(domain.getString());

		spider.AddUnspidered(ckurl.getString());

		success = spider.CrawlNext();

		if (success) { 
			spider.get_LastHtml(html);

			logging = html.getSizeUtf8();

			buffer.append(" ");
			buffer.append(url);
			buffer.append(" | ");
			buffer.append(html.getString());
			buffer.append(" |||");

			size_unspired = spider.get_NumUnspidered();
			for (i = 0; i < size_unspired; i++){
				spider.GetUnspideredUrl(0, ckurl);
				url = getNormalizedUrl(ckurl.getString());
				// cout << ckurl.getString() << " " << url << endl;
				spider.SkipUnspidered(0);

				url_size = getURLsize(url);
				if (url_size > 0 && url_size <= LIMIT_SIZE_URL){
					if (!queued_url[url]){
						urls_queue.queueURL(url);
						queued_url[url] =  true;
					}
				}

			}

			size_unspired = spider.get_NumOutboundLinks();

			for (i = 0; i < size_unspired; i++){
				spider.GetOutboundLink(i, ckurl);
				url = getNormalizedUrl(ckurl.getString());

				url_size = getURLsize(url);
				if (url_size > 0 && url_size <= LIMIT_SIZE_URL && isBrDomain(url)){
					if (!queued_url[url]){
						urls_queue.queueURL(url);
						queued_url[url] =  true;
					}
				}

			}

			spider.ClearOutboundLinks();

			t2 = high_resolution_clock::now();

			duration = duration_cast<milliseconds>( t2 - t1 ).count();

			total_duration = duration_cast<seconds>( t2 - t0 ).count();

			logs << total_duration << "," << duration << "," << logging << endl;
		}

		loop_control++;
	}

	return buffer;
}

void backingup_queue(){
	int i;
	double total_duration;
	vector<string> v;

	high_resolution_clock::time_point t1, t2;

	backup_queue.open(BACKUP_QUEUE_FILENAME, ios::out | ios::app);

	status_log_mutex.lock();
	t1 = high_resolution_clock::now();

	total_duration = duration_cast<seconds>( t1 - t0 ).count();
	status_log << endl << "Backing up queue (" << total_duration << " s)" << endl;
	status_log_mutex.unlock();

	for (i = 0; i < KEEPING_FROM_BACKUP; i++){
		v.push_back(urls_queue.dequeueURL());
	}
	while(urls_queue.getSize() > 0){
		// output.append(urls_queue.dequeueURL());
		// output.append("\n");
		backup_queue << urls_queue.dequeueURL() << endl;
	}
	for (i = 0; i < v.size(); i++){
		urls_queue.queueURL(v[i]);
	}

	v.clear();
	v.shrink_to_fit();

	status_log_mutex.lock();
	t2 = high_resolution_clock::now();

	total_duration = duration_cast<seconds>( t2 - t1 ).count();
	status_log << "Done backing up queue (" << total_duration << " s)" << endl << endl;
	status_log_mutex.unlock();

	backup_queue.close();
}

void restoring_backup(){
	int i = 0;
	double total_duration;
	string url;
	high_resolution_clock::time_point t1, t2;

	status_log_mutex.lock();
	t1 = high_resolution_clock::now();

	total_duration = duration_cast<seconds>( t1 - t0 ).count();
	status_log << endl << "Restoring queue (" << total_duration << " s)" << endl;
	status_log_mutex.unlock();

	while(getline(reading_backup_queue,url) && i < KEEPING_FROM_BACKUP && urls_queue.getSize() < BACKUP_QUEUE_SIZE){
		urls_queue.queueURL(url);
		i++;
	}

	status_log_mutex.lock();
	t2 = high_resolution_clock::now();

	total_duration = duration_cast<seconds>( t2 - t1 ).count();
	status_log << "Done restoring queue (" << total_duration << " s)" << endl << endl;
	status_log_mutex.unlock();
}