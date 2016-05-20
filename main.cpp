#include "Define.h"

// File to wait when queue is empty
const std::chrono::seconds SLEEP_TIME(5);
// Time backup and restore thread sleep
const std::chrono::minutes BACKUP_SLEEP_TIME(5);

using namespace std;
using namespace std::chrono;

PriorityQueue urls_queue; // Priority Queue 
unordered_map<string, bool> queued_url; // URLs in queue (or that have been in queue)
unordered_map<string, high_resolution_clock::time_point> last_access; // Time of last access to domain
ofstream logs, status_log, backup_queue, html_files[NUM_THREADS]; // Files
ifstream reading_backup_queue; // Restore queue file
int log_entrance = 0; // Log entraces counter
string log_buffer;

mutex urls_queue_mutex, queued_url_mutex, log_mutex, status_log_mutex, last_access_mutex; // Mutexes

high_resolution_clock::time_point t0;


// Actual crawling
void crawling(int id);
// Queue's backup
void backingup_queue();
// Queue restoring
void restoring_backup();

string cleanHTML(string html);

int main(){
	int i;
	string filename;

	// Some other links where added because I was getting a lot of fail access
	vector<string> initial_url = {	"http://jogos.uol.com.br", "http://www.ojogos.com.br", "http://www.papajogos.com.br",
									"http://www.gamevicio.com", "http://g1.globo.com/tecnologia", "http://globo.com", 
									"http://www.gamesbrasil.com.br", "http://br.ign.com", "http://www.comboinfinito.com.br/",
									"http://www.brasilgamer.com.br", "http://gizmodo.uol.com.br", "http://www.uol.com.br",
									"http://www.ig.com.br", "http://br.yahoo.com/"};

	vector<thread> ths;
	thread backup(backingup_queue);//, restore;


	// Time program started
	t0 = high_resolution_clock::now();

	// Initializing queue
	for (string v : initial_url){
		urls_queue.push(getNormalizedUrl(v));
		queued_url[getNormalizedUrl(v)] = true;
	}

	// Initializing log
	logs.open(LOG_FILENAME, ios::out);
	logs << "time from beginning(s),time spent(ms),size(bytes)" << endl;

	status_log.open(STATUS_LOG_FILENAME, ios::out);

	backup_queue.open(BACKUP_QUEUE_FILENAME, ios::out);
	backup_queue.close();
	reading_backup_queue.open(BACKUP_QUEUE_FILENAME);

	// restore = thread(restoring_backup);

	status_log << "Initial queue size: " << urls_queue.size() << endl;
	status_log << "Creating threads" << endl;

	// Detaching threads, they will run in background
	backup.detach();
	// restore.detach();

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
		ths.push_back(thread(&crawling, i));
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

	ths.clear();

	return 0;

}

void crawling(int id){
	int i, size_unspired, logging, vector_size, url_size;
	int file_index, file_size = 0;
	double duration, total_duration;
	bool success, politeness_control;

	string url, filename, buffer;

	vector<string> local_to_queue;

	CkSpider spider;
	CkString ckurl, domain, html;

	high_resolution_clock::time_point t1, t2, tf, tla;

	unordered_map<std::string,high_resolution_clock::time_point>::const_iterator out;

	
	// Reopening file to append content
	filename.append(HTML_FILENAME);
	filename.append("-");
	filename.append(to_string(id));
	filename.append("-");
	filename.append(to_string(file_size));
	html_files[id].open(filename, ios::out | ios::app);
	filename.clear();

	// Main loop
	while (true){

		// Iteration start
		t1 = high_resolution_clock::now();

		// Taking queue's root
		urls_queue_mutex.lock();
		
		url = urls_queue.pop();

		urls_queue_mutex.unlock();

		// If queue is empty, it returns an empty string, therefore it's necessary to check returns size.
		if (url.size() > 0){
			// Recieved a valid URL

			ckurl = url.c_str();

			// Get URL's domain
			spider.GetUrlDomain(ckurl.getString(), domain);

			// Checks last time URL's domain was accessed
			// out = last_access.find(domain.getString());
			// if (out != last_access.end()){

			// 	politeness_control = true;

			// 	// Loop is necessary because other thread may access that domain while this one is waiting
			// 	while (politeness_control) {
			// 		// Time now
			// 		t2 = high_resolution_clock::now();

			// 		last_access_mutex.lock();

			// 		// Time domain was last accessed
			// 		tla = last_access[domain.getString()];

			// 		// Time elapsed
			// 		total_duration = duration_cast<seconds>(t2 - tla).count();

			// 		politeness_control = total_duration < POLITENESS_TIME;

			// 		if (!politeness_control){
			// 			// In case it has elapsed more than POLITENESS_TIME, update last access so other thread do not try to access this domain
			// 			last_access[domain.getString()] = t2;
			// 		}

			// 		last_access_mutex.unlock();

			// 		if (politeness_control){
			// 			// In case it has elapsed less than POLITENESS_TIME, sleep the difference
			// 			status_log_mutex.lock();

			// 			tf = high_resolution_clock::now();

			// 			duration = duration_cast<seconds>( tf - t0 ).count();

			// 			status_log << "Thread " << id << " is having its politeness sleep. (" << duration << " s)" << endl;
			// 			status_log_mutex.unlock();

			// 			std::chrono::seconds POLITENESS_SLEEP_TIME((int)(POLITENESS_TIME - total_duration));
			// 			this_thread::sleep_for(std::chrono::seconds(POLITENESS_SLEEP_TIME));
			// 		}
			// 	}

			// }

			// t2 = high_resolution_clock::now();

			// last_access_mutex.lock();
			// // Updating time of last access
			// last_access[domain.getString()] = t2;
			// last_access_mutex.unlock();

			spider.Initialize(domain.getString());

			spider.AddUnspidered(ckurl.getString());

			success = spider.CrawlNext();

			if (success) { 
				spider.get_LastHtml(html);

				// Getting HTML's size, in bytes
				logging = html.getSizeUtf8();

				buffer.append(" ");
				buffer.append(url);
				buffer.append(" | ");
				buffer.append(cleanHTML(html.getString()));
				buffer.append(" |||");

				file_size+=buffer.size();

				html_files[id] << buffer;

				if (logging <= 0 ){
					status_log_mutex.lock();

					status_log << "Unable to download " << url << " " << spider.get_NumUnspidered() << " " << spider.get_NumOutboundLinks() << endl;

					status_log_mutex.unlock();
				}

				buffer.clear();
				buffer.shrink_to_fit();

				if (file_size >= LIMIT_HTML_FILE_SIZE){
					// If file is bigger than limit, close file and open a new one
					html_files[id].close();
					file_size = 0;
					file_index++;
					filename.append(HTML_FILENAME);
					filename.append("-");
					filename.append(to_string(id));
					filename.append("-");
					filename.append(to_string(file_index));
					html_files[id].open(filename, ios::out | ios::app);
					buffer.append("|||");
					filename.clear();
				}

				// Saving unspired links to local queue
				size_unspired = spider.get_NumUnspidered();
				for (i = 0; i < size_unspired; i++){
					spider.GetUnspideredUrl(0, ckurl);
					url = getNormalizedUrl(ckurl.getString());
					spider.SkipUnspidered(0);

					url_size = getURLsize(url);
					if (url_size > 0 && url_size <= LIMIT_SIZE_URL){
						if (!queued_url[url] && isBrDomain(url)){
							local_to_queue.push_back(url);
						} 
					}
				}

				// Saving outbound links to local queue
				size_unspired = spider.get_NumOutboundLinks();
				for (i = 0; i < size_unspired; i++){
					spider.GetOutboundLink(i, ckurl);
					url = getNormalizedUrl(ckurl.getString());

					url_size = getURLsize(url);
					if (url_size > 0 && url_size <= LIMIT_SIZE_URL){
						if (!queued_url[url] && isBrDomain(url)){
							local_to_queue.push_back(url);
						}
					}
				}

				spider.ClearOutboundLinks();

				/*	After having both unspired and outbound links
					push all of them to priority queue
				*/

				vector_size = local_to_queue.size();

				urls_queue_mutex.lock();
				queued_url_mutex.lock();

				for (i = 0; i < vector_size; i++){
					url = local_to_queue.back();
					if(!queued_url[url]){
						urls_queue.push(url);
						queued_url[url] = true;
					}
					local_to_queue.pop_back();
				}

				queued_url_mutex.unlock();
				urls_queue_mutex.unlock();

				local_to_queue.clear();
				local_to_queue.shrink_to_fit();
				
				// Saving log
				log_mutex.lock();

				t2 = high_resolution_clock::now();

				duration = duration_cast<milliseconds>( t2 - t1 ).count();
				total_duration = duration_cast<seconds>( t2 - t0 ).count();

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

				log_mutex.unlock();

				status_log_mutex.lock();

				tf = high_resolution_clock::now();

				duration = duration_cast<seconds>( tf - t0 ).count();

				status_log << "Queue size " << urls_queue.size() << " (" << duration << " s)" << endl;
				status_log_mutex.unlock();

			}  else {
				// crawlnext failed to load URL
				status_log_mutex.lock();

				status_log << "Fail to access " << url << " " << spider.get_NumUnspidered() << " " << spider.get_NumOutboundLinks() << endl;

				status_log_mutex.unlock();

			}

			spider.ClearFailedUrls();
		} else {
			// Queue returned and URL with size 0, which means queue is empty
			// Thread sleeps for a while so queue may fill
			status_log_mutex.lock();

			tf = high_resolution_clock::now();

			duration = duration_cast<seconds>( tf - t0 ).count();

			status_log << "Thread " << id << " is sleeping. (" << duration << " s)" << endl;
			status_log_mutex.unlock();

			this_thread::sleep_for(SLEEP_TIME);
		}

		spider.ClearSpideredUrls();
	}

	status_log_mutex.lock();

	tf = high_resolution_clock::now();

	duration = duration_cast<seconds>( tf - t0 ).count();

	status_log << "Thread " << i << " is dead after " << duration << " s of execution." << endl;
	status_log_mutex.unlock();

}

void backingup_queue(){
	unsigned int i, size_to_keep;
	double total_duration;
	vector<string> backup;
	string url;

	high_resolution_clock::time_point t1, t2;

	while(true){
		urls_queue_mutex.lock();

		if (urls_queue.size() >= BACKUP_QUEUE_SIZE){

			backup_queue.open(BACKUP_QUEUE_FILENAME, ios::out | ios::app);

			status_log_mutex.lock();
			t1 = high_resolution_clock::now();

			total_duration = duration_cast<seconds>( t1 - t0 ).count();
			status_log << endl << "Backing up queue (" << total_duration << " s)" << endl;
			status_log_mutex.unlock();

			// Checks if queue has more than KEEPING_FROM_BACKUP elemens
			size_to_keep = (urls_queue.size() >= KEEPING_FROM_BACKUP) ? KEEPING_FROM_BACKUP : urls_queue.size();

			// Saving URLs
			for (i = 0; i < size_to_keep; i++){
				backup.push_back(urls_queue.getFromVector(i));
			}

			queued_url_mutex.lock();

			// Writing URLs into file
			// while(!urls_queue.empty()){
			// 	url = urls_queue.popFromVector();
			// 	backup_queue << url << endl;
			// }

			urls_queue.clear();

			// Restoring saved URLs
			for (i = 0; i < backup.size(); i++){
				urls_queue.push(backup[i]);
			}

			queued_url_mutex.unlock();

			backup.clear();
			backup.shrink_to_fit();

			status_log_mutex.lock();
			t2 = high_resolution_clock::now();

			total_duration = duration_cast<seconds>( t2 - t1 ).count();
			status_log << "Done backing up queue (total duration: " << total_duration << " s)" << endl << endl;
			status_log_mutex.unlock();

			backup_queue.close();
		}

		urls_queue_mutex.unlock();

		this_thread::sleep_for(BACKUP_SLEEP_TIME);

	}
}

void restoring_backup(){
	unsigned int i = 0;
	double total_duration;
	string url;
	high_resolution_clock::time_point t1, t2;

	this_thread::sleep_for(BACKUP_SLEEP_TIME);

	while(true){
		if (urls_queue.size() <= MIN_TO_KEEP_IN_QUEUE){
			status_log_mutex.lock();
			t1 = high_resolution_clock::now();

			total_duration = duration_cast<seconds>( t1 - t0 ).count();
			status_log << endl << "Restoring queue (" << total_duration << " s)" << endl;
			status_log_mutex.unlock();

			urls_queue_mutex.lock();

			// Restoring URLs from file
			while(getline(reading_backup_queue,url) && i < KEEPING_FROM_BACKUP && urls_queue.size() < BACKUP_QUEUE_SIZE){
				urls_queue.push(url);
				i++;
			}

			urls_queue_mutex.unlock();

			status_log_mutex.lock();
			t2 = high_resolution_clock::now();

			total_duration = duration_cast<seconds>( t2 - t1 ).count();
			status_log << "Done restoring queue (" << total_duration << " s)" << endl << endl;
			status_log_mutex.unlock();
		}

		this_thread::sleep_for(BACKUP_SLEEP_TIME);
	}
}

string cleanHTML(string html){
	string new_html = html;
	size_t found = new_html.find("|");

	while(found != std::string::npos){
		new_html.replace(found,1,"*");

		found = new_html.find("|");
	}

	return new_html;
}