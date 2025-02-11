#include "pwordcount.h"
#include <errno.h> // error handling

#define BUFFER_SIZE 1024 // Buffer for reading file

void get_filename(char *filename) {
  printf("Enter a new file name (or type 'exit' to quit): ");
  scanf("%s", filename);
}

int check_file(char *filename) {
  FILE *file;

  while (1) {
    file = fopen(filename, "r");
    if(file != NULL) {
      fclose(file);
      return 1; // File exists and can be opened
    }

    printf("Error: Cannot open file '%s'.\n", filename);
    perror("System Error");

    // Ask user for a new file or exit
    printf("Would you like to enter a different file? (y/n): ");
    char choice;
    scanf(" %c", &choice);

    if (choice == 'n' || choice == 'N') {
      printf("Exiting program...\n");
      exit(1);
    }

    // Get a new filename from user
    get_filename(filename);
  }
}

// Function for user choice
int get_user_choice() {
  int choice;
  printf("Choose processing mode:\n");
  printf("1 - Single-core mode (1 child process)\n");
  printf("2 - Multi-core mode (Use all available cores)\n");
  printf("Enter your choice (1 or 2): ");
  scanf("%d", &choice);

  while (choice != 1 && choice != 2) {
    printf("Invalid choice. Please enter 1 for single-core or 2 for multi-core: ");
    scanf("%d", &choice);
  }
  return choice;
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

void create_process(const char *filename, int use_multiple_cores) {
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start); // Start timing
  
  long file_size = get_file_size(filename);
  if (file_size < 0) {
    printf("Error: Unable to determine file size.\n");
    return;
  }

  //Determine number of child processes based on user choice
  int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
  int num_processes = use_multiple_cores ? num_cores : 1;

  printf("File Size: %ld bytes. Detected %d cores. Using %d child process(es).\n", file_size, num_cores, num_processes);

  int fd1[2], fd2[2]; // Pipes for communication
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

    if (pids[i] == 0) { // Child process
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

  //Measure execution time
  clock_gettime(CLOCK_MONOTONIC, &end);
  double exec_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  // Print final word count
  printf("Total word count: %d\n", total_word_count);
  printf("Execution Time: %.5f seconds\n", exec_time);

  // Log result to file
  log_result(filename, total_word_count, exec_time, num_processes);  
}

void log_result(const char *filename, int word_count, double exec_time, int num_processes) {
  FILE *log_file = fopen(LOG_FILE, "a"); // Open in addend mode
  if (log_file == NULL) {
    perror("Error opening log file");
    return;
  }

  fprintf(log_file, "File: %s | Word Count: %d | Execution Time: %.5f sec | Processes: %d\n",
	  filename, word_count, exec_time, num_processes);

  fclose(log_file);
}
			      
