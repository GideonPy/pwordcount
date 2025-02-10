#ifndef PWORDCOUNT_H
#define PWORDCOUNT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For fork(), pipe()
#include <sys/types.h>
#include <string.h>
#include <ctype.h> // For isspace()


// Functions
int check_file(const char *filename);
void load_file(const char *filename);
void create_process(const char *filename);
int count_words(const char *text);

#endif
