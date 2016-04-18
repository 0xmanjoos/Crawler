#ifndef DEFINE_H
#define DEFINE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include <memory>
#include <chrono> // C++11 Time lib
#include <unordered_map> // C++11 Hash Table
#include <thread> // C++1 Threads
#include <mutex> // C++11 mutex
#include "PriorityQueue.h"
#include "chilkat/include/CkSpider.h"
#include "chilkat/include/CkString.h"
#include "chilkat/include/CkStringArray.h"


// Defines the time that has to be between two accesses to the same domain
#define POLITENESS_TIME 30.0 // seconds

// Number of threads
#define NUM_THREADS 100
// Biggest acceptable URL size, bigger than that will be ignored
#define LIMIT_SIZE_URL 10

// Log's files
#define LOG_FILENAME "logs/log.csv"
#define STATUS_LOG_FILENAME "logs/status_log.txt"

// HTML's base name file
#define HTML_FILENAME "htmls/html_files"
// Size to close a file
#define LIMIT_HTML_FILE_SIZE 500000000 // 500 MB

// Number of log entries kept in memory before been written
#define LIMIT_MEM_LOG 500

// Threshold of queue's size, when backup thread wakes up and queue is bigger than that, backup will proceed
#define BACKUP_QUEUE_SIZE 150000
// Entries kept in queue during backup / Same value to be retrieved during restore process
#define KEEPING_FROM_BACKUP 30000
// // Threshold of queue's size, when restoring thread wakes up and queue is smaller than that, restore will proceed
#define MIN_TO_KEEP_IN_QUEUE 1000
// Queue's backup filename
#define BACKUP_QUEUE_FILENAME "backup/queue"


#endif