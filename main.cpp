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

#define HTML_FILENAME "htmls/html"
#define LIMIT_HTML_FILE_SIZE 500000000 // 500 MB

#define LIMIT_MEM_LOG 500

#define BACKUP_QUEUE_SIZE 50000
#define KEEPING_FROM_BACKUP 2000
#define BACKUP_QUEUE_FILENAME "backup/queue"

const std::chrono::seconds SLEEP_TIME(30);

using namespace std;
using namespace std::chrono;

PriorityQueue urls_queue;
// unordered_map<string, bool> visited_url; // Better off the PriotyQueue, because using threads, there will be less links to be "tested" in the queue
ofstream logs, status_log, backup_queue, html_files[NUM_THREADS];
ifstream reading_backup_queue;
int index_file = 0, log_entrance = 0;
string log_buffer;

mutex urls_queue_mutex, visited_url_mutex, log_mutex, buffer_mutex, status_log_mutex, cout_mutex;

high_resolution_clock::time_point t0;


void crawling(int id, string in_buffer);
string initializing_queue(vector<string> v);
void backingup_queue();
void restoring_backup();

int main(){
	int i;

	vector<string> initial_url = {	"http://jogos.uol.com.br", "http://www.ojogos.com.br", "http://www.papajogos.com.br",
									"http://www.gamevicio.com", "http://g1.globo.com/tecnologia", "http://www.globo.com"	};
	vector<thread> ths;

	string buffer, filename;

	logs.open("logs/log.csv", std::ofstream::out);
	logs << "time from beginning(s),time spent(ms),size(bytes)" << endl;

	status_log.open("logs/status_log.txt", std::ofstream::out);

	backup_queue.open(BACKUP_QUEUE_FILENAME, ios::out);
	backup_queue.close();
	reading_backup_queue.open(BACKUP_QUEUE_FILENAME);

	t0 = high_resolution_clock::now();

	buffer = initializing_queue(initial_url);

	status_log << "Initial queue size: " << urls_queue.getSize() << endl;
	status_log << "Creating threads" << endl;

	for (i = 0; i < NUM_THREADS; i++){
		filename.append(HTML_FILENAME);
		filename.append("-");
		filename.append(to_string(i));
		filename.append("-0");
		// cout << filename << endl;
		html_files[i].open(filename, ios::out | ios::app);		

		ths.push_back(thread(&crawling, i, buffer));

		filename.clear();
		buffer.clear();
	}

	for (auto& th : ths) {
		th.join();
	}

	for (i = 0; i < NUM_THREADS; i++){
		html_files[i].close();
	}

	logs.close();
	status_log.close();
	backup_queue.close();
	reading_backup_queue.close();

	return 0;


	////////////////////////////////////////////////////////
}

void crawling(int id, string in_buffer){
	int i, size_unspired, logging, dequeue_size, vector_size, url_size;
	int file_index = 0, file_size = in_buffer.size();
	double duration, total_duration;
	bool success, havent_slept = true;

	string url, buffer, filename;

	vector<string> local_queue, local_to_queue;

	CkSpider spider;
	CkString ckurl, domain, html;

	high_resolution_clock::time_point t1, t2, tf;

	// myfile.seekp (ios::end);

	spider.put_CacheDir("cache/");
	spider.put_FetchFromCache(true);
	spider.put_UpdateCache(true);

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

			urls_queue_mutex.lock();
			if (urls_queue.getSize() >= BACKUP_QUEUE_SIZE){
				backingup_queue();
			}
			urls_queue_mutex.unlock();

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

				// buffer_mutex.lock();
				// // cout_mutex.lock();
				// // cout << "buffer" << " mutex locked" << endl;
				// // cout_mutex.unlock();
				buffer.append("||| ");
				buffer.append(url);
				buffer.append(" | ");
				buffer.append(" |||");
				buffer.append(html.getString());

				html_files[id] << buffer;

				file_size+=buffer.size();

				if (file_size > LIMIT_HTML_FILE_SIZE){
					html_files[id].close();
					file_size = 0;
					file_index++;
					filename.append(HTML_FILENAME);
					filename.append("-");
					filename.append(to_string(id));
					filename.append("-");
					filename.append(to_string(file_index));
					// html_files[i].open(HTML_FILENAME+"-"+to_string(i)+"-0", ios::out | ios::app);
		
					// html_files[id].open(HTML_FILENAME+"-"+to_string(file_index)+"-"+to_string(id), ios::out | ios::app);
					html_files[i].open(filename, ios::out | ios::app);
				// 	html_files << buffer;
				// 	html_files.close();
				// 	buffer.clear();
				// 	index_file++;
				}

				// // cout_mutex.lock();
				// // cout << "buffer" << " mutex unlocked" << endl;
				// // cout_mutex.unlock();

				// buffer_mutex.unlock();

				size_unspired = spider.get_NumUnspidered();

				for (i = 0; i < size_unspired; i++){
					spider.GetUnspideredUrl(0, ckurl);
					url = getNormalizedUrl(ckurl.getString());
					spider.SkipUnspidered(0);

					url_size = getURLsize(url);
					if (url_size > 0 && url_size <= LIMIT_SIZE_URL){
						// visited_url_mutex.lock();
						// cout_mutex.lock();
						// cout << "visited_url" << " mutex locked" << endl;
						// cout_mutex.unlock();
						// if (!visited_url[url]){
							local_to_queue.push_back(url);
							// visited_url[url] =  true;
						// } 
						// cout_mutex.lock();
						// cout << "visited_url" << " mutex unlocked" << endl;
						// cout_mutex.unlock();
						// visited_url_mutex.unlock();
					}
				}

				size_unspired = spider.get_NumOutboundLinks();

				for (i = 0; i < size_unspired; i++){
					spider.GetOutboundLink(i, ckurl);
					url = getNormalizedUrl(ckurl.getString());

					url_size = getURLsize(url);
					if (url_size > 0 && url_size <= LIMIT_SIZE_URL && isBrDomain(url)){
						// visited_url_mutex.lock();
						// cout_mutex.lock();
						// cout << "visited_url" << " mutex locked" << endl;
						// cout_mutex.unlock();
						// if (!visited_url[url]){
							local_to_queue.push_back(url);
							// visited_url[url] =  true;
						// }
						// cout_mutex.lock();
						// cout << "visited_url" << " mutex unlocked" << endl;
						// cout_mutex.unlock();
						// visited_url_mutex.unlock();
					}
				}

				spider.ClearOutboundLinks();

				// if(local_to_queue.size() >= SIZE_LOCAL_QUEUE || urls_queue.empty()){
				if(local_to_queue.size() >= SIZE_LOCAL_QUEUE){

					if (urls_queue.getSize() <= BACKUP_QUEUE_SIZE){
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

					local_to_queue.clear();
				}

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

	spider.put_CacheDir("cache/");
	spider.put_FetchFromCache(true);
	spider.put_UpdateCache(true);

	for (i = 0; i < v.size(); i++){
		urls_queue.queueURL(getNormalizedUrl(v[i]));
		// visited_url[getNormalizedUrl(v[i])] = true;
	}

	loop_control = 0;

	while(loop_control < INITIAL_COLLECT){
	

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
			buffer.append(" |||");
			buffer.append(html.getString());

			// html_files[0] << buffer;

			// if (buffer.size() > LIMIT_HTML_FILE_SIZE){
			// 	html_files.open(HTML_FILENAME+to_string(index_file));
			// 	html_files << buffer;
			// 	html_files.close();
			// 	buffer.clear();
			// 	index_file++;
			// }

			size_unspired = spider.get_NumUnspidered();
			for (i = 0; i < size_unspired; i++){
				spider.GetUnspideredUrl(0, ckurl);
				url = getNormalizedUrl(ckurl.getString());
				// cout << ckurl.getString() << " " << url << endl;
				spider.SkipUnspidered(0);

				url_size = getURLsize(url);
				if (url_size > 0 && url_size <= LIMIT_SIZE_URL){
					// if (!visited_url[url]){
						urls_queue.queueURL(url);
						// visited_url[url] =  true;
					// }
				}

			}

			size_unspired = spider.get_NumOutboundLinks();

			for (i = 0; i < size_unspired; i++){
				spider.GetOutboundLink(i, ckurl);
				url = getNormalizedUrl(ckurl.getString());

				url_size = getURLsize(url);
				if (url_size > 0 && url_size <= LIMIT_SIZE_URL && isBrDomain(url)){
					// if (!visited_url[url]){
						urls_queue.queueURL(url);
						// visited_url[url] =  true;
					// }
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

	// html_files[0].close();
	return buffer;
}

void backingup_queue(){
	int i, loop_size;
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

	// free(urls_queue);

	// urls_queue = new PriorityQueue();

	loop_size = v.size();
	for (i = 0; i < loop_size; i++){
		urls_queue.queueURL(v.front());
		v.erase(v.cbegin());
	}

	// v.clear();

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