#include <stdio.h>
#include <string.h>
#include "snake_utils.h"
#include "state.h"

int main(int argc, char* argv[]) {
  char* in_filename = NULL;
  char* out_filename = NULL;
  game_state_t* state = NULL;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 && i < argc - 1) {
      in_filename = argv[i + 1];
      i++;
      continue;
    }
    if (strcmp(argv[i], "-o") == 0 && i < argc - 1) {
      out_filename = argv[i + 1];
      i++;
      continue;
    }
    fprintf(stderr, "Usage: %s [-i filename] [-o filename]\n", argv[0]);
    return 1;
  }

  // Do not modify anything above this line.

  /* Task 7 */

  // Read board from file, or create default board if no input filename was given
  if (in_filename != NULL) {
    // TODO: load the board from in_filename into state...
    // TODO: ...then call initialize_snakes on the state you made
  } else {
    // TODO: create the default state in state
  }

  // TODO: Update state. Use the deterministic_food function
  // (already implemented in state_utils.h) to add food.

  // Write updated board to file, or print to stdout if no output filename was given
  if (out_filename != NULL) {
    // TODO: save the board to out_filename
  } else {
    // TODO: print the board to stdout
  }

  // TODO: free any allocated memory

  return 0;
}
