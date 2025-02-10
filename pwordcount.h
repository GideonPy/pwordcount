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

#define LARGE_FILE_SIZE (1024*1024) // 1MB threshold


// Functions
int check_file(char *filename);
long get_file_size(const char *filename);
void load_file(const char *filename);
void create_process(const char *filename);
int count_words(const char *text);

#endif
