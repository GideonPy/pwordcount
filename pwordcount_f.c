#include "pwordcount.h"
#include <errno.h> // error handling

#define BUFFER_SIZE 1024 // Buffer for reading file 

int check_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error: Cannot open file '%s'.\n", filename);
    perror("Error opening file"); //Detailed error message
    return 0; // file not found
  }
  fclose(file);
  return 1; // file exsists
}

// Function to get file size
long get_file_size(const char *filename) {
  struct stat st;
  if (stat(filename, &st) == 0) {
    return st.st_size;
  }
  perror("Error getting file size");
  return -1;
}

//Function to count words in a given string
int count_words(const char *text) {
  int count = 0;
  int in_word = 0;

  while (*text) {
    if (isspace((unsigned char)*text)) {
      in_word = 0; // Find space, word ended
    } else if (!in_word) {
      in_word = 1; // Find new word
      count ++;
    }
    text++;
  }
  return count;
}

void load_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error: Cannot open file '%s',\n", filename);
    return;
  }
  printf("file '%s' loaded successfully.\n", filename);
  fclose(file);
}

void create_process(const char *filename) {
  long file_size = get_file_size(filename);
  if (file_size < 0) {
    printf("Error: Unable to determine file size.\n");
    return;
  }

  //Determine number of child processes
  int num_processes = 1;
  if (file_size > LARGE_FILE_SIZE) {
    num_processes = 2; // Increase processes for large files.
  }

  printf("File Size: %ld bytes. Using %d child process(es).\n", file_size, num_processes);

  int fd1[2], fd2[2]; // Pipes
  pid_t pids[num_processes];

  // Create pipes
  if (pipe(fd1) == -1 || pipe(fd2) == -1) {
    perror("Pipe failed");
    exit(1);
  }

  // Fork multiple child processes
  for (int i = 0; i < num_processes; i++) {
    if ((pids[i] = fork()) < 0) {
      perror("Fork failed");
      exit(1);
    }

    if (pids[i] == 0) { // child process
      close(fd1[1]); // Close write end of input pipe
      close(fd2[0]); // Close read end of output pipe

      char buffer[1024];
      int total_words = 0;

      //Read data from parent
      ssize_t bytesRead;
      while (( bytesRead = read(fd1[0], buffer, sizeof(buffer) - 1)) > 0) {
	buffer[bytesRead] = '\0';
	total_words += count_words(buffer);
      }

      close(fd1[0]); // Close input pipe after reading

      printf("Child %d: Counted %d words.\n", getpid(), total_words);

      // Send word count back to parent
      write(fd2[1], &total_words, sizeof(int));
      close(fd2[1]);

      exit(0);
    }
  }

  // Parent process: Read file and distrubute data
  close(fd1[0]); // Close read end of input pipe
  close(fd2[1]); // Close write end of output pipe

  FILE *file = fopen(filename,"r");
  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }

  char buffer[1024];
  while(fgets(buffer, sizeof(buffer), file) != NULL) {
    write(fd1[1], buffer, strlen(buffer));
  }

  fclose(file);
  close(fd1[1]); // Close write end after writing

  // Collect word counts from children
  int total_word_count = 0, child_word_count;
  for (int i = 0; i < num_processes; i++) {
    read(fd2[0], &child_word_count, sizeof(int));

    // debug information
    printf("Parent: Received %d words from child %d\n", child_word_count, pids[i]);
    
    total_word_count += child_word_count;
  }

  close(fd2[0]); // Close read end of output pipe

  // Print final word count
  printf("Total word count: %d\n", total_word_count);
}

			      
