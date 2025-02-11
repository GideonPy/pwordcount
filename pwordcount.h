#ifndef PWORDCOUNT_H
#define PWORDCOUNT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For fork(), pipe()
#include <sys/types.h>
#include <sys/stat.h> // File size checking
#include <fcntl.h> // File control options
#include <string.h>
#include <ctype.h> // For isspace()
#include <sys/sysinfo.h> // For getting CPU core count
#include <time.h> // For measuring execution time

#define LARGE_FILE_SIZE (1024*1024) // 1MB threshold
#define LOG_FILE "pwordcount.log" // Log file name



// Functions
int check_file(char *filename);
long get_file_size(const char *filename);
void load_file(const char *filename);
void create_process(const char *filename, int use_multiple_cores);
int count_words(const char *text);
int get_user_choice(); // User function to get core choice.
void log_result(const char *filename, int word_count, double exec_time, int num_processes);

#endif
