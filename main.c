#include "pwordcount.h"

int main(int argc, char *argv[]) {
  //Check if user provided a file name
  if (argc !=2) {
    printf("Usage:%s <filename>\n", argv[0]);
    return 1;
  }

  const char *filename = argv[1];

  //check if the file exists
  if (!check_file(filename)) {
    return 1; // Exit if file cannot be opened
  }

  // Load the file (for now just confirming successful open)
  create_process(filename);

  return 0;
}









					 
				      
