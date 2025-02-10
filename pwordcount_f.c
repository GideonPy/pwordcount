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
  int pipe1[2]; // Pipe for sending file content to child
  int pipe2[2]; // Pipe for sending word count back to parent
  pid_t pid;
  char buffer[BUFFER_SIZE];

  // Create pipes
  if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
    perror("Pipe failed");
    exit(1);
  }

  // Fork child process
  pid = fork();
  if (pid < 0) {
    perror("Fork failed");
    exit(1);
  }

  if (pid > 0) {
    //Parent process: Reads and sends data through the pipe
    close(pipe1[0]); // Close unused read end of pipe1
    close(pipe2[1]); // Close write end of pipe2

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
      perror("Error openinig file");
      exit(1);
    }

    printf("Parent: Reading file and sending data to child...\n");
    while (fgets(buffer,BUFFER_SIZE, file) != NULL) {
      write(pipe1[1], buffer, strlen(buffer)); // write to pipe1
    }

    fclose(file);
    close(pipe1[1]); // Close write end after writing

    // Read word count result from child through pipe2
    int word_count;
    read(pipe2[0], &word_count, sizeof(word_count));
    printf("Parent: The total number of words is %d.\n", word_count);
    close(pipe2[0]); // Close read endo of pipe2
  }else {
    // Child proces: Reads data from the pipe, counts , and sends data back to parent
    close(pipe1[1]); // Close  write end of pipe1
    close(pipe2[0]); // Close read end of pipe2

    ssize_t bytesRead;
    int total_words= 0;
    
    printf("Child: Receiving file content from parent and counting words...\n");
    while ((bytesRead = read(pipe1[0], buffer, BUFFER_SIZE -1)) > 0) {
      buffer[bytesRead] = '\0'; // Null-terminate string
      total_words += count_words(buffer); // Count words in chunk
    }

    close(pipe1[0]); // Close read end after reading

    // Send word count back to parent
    write(pipe2[1], &total_words, sizeof(total_words));
    close(pipe2[1]); // Close write end after writing
  }
}
	    
			      
