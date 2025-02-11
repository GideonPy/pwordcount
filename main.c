#include "pwordcount.h"

int main() {
  char filename[256];
  int continue_program = 1;

  while (continue_program) {
  // Ask user for filename
  printf("Enter the file name: ");
    scanf("%s", filename);

  // Check if the file exsits and is readable
  if (!check_file(filename)) {
    return 1; // Exit if file cannot be loaded
  }

  //Ask the user whether to use multiple cores or single core
  int choice = get_user_choice();
  int num_children = 1; // Declare and intialize num_children

  if (choice == 1) { // single-core mode
      num_children = get_child_count();
    }

  // Load the file and create a process to handle word counting
    create_process(filename, (choice == 2), num_children);

  //Ask the user if they want to process another file
  printf("Would you like to process another file? (y/n): ");
  char response;
  scanf(" %c", &response);

  if (response == 'n' || response == 'N') {
    printf("Exiting program...\n");
    continue_program = 0;
  }
 }
  
  return 0;
}









					 
				      
