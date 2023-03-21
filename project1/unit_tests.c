#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Necessary due to static functions in state.c
#include "state.c"

char* COLOR_GREEN = "";
char* COLOR_RESET = "";

bool assert_true(char* msg, bool actual) {
  if (!actual) {
    printf("Assertion error: expected %s to be true, but got false\n", msg);
    return false;
  } else {
    return true;
  }
}

bool assert_equals_char(char* msg, char expected, char actual) {
  if (expected != actual) {
    printf("Assertion error: expected %s to be %c but got %c\n", msg, expected, actual);
    return false;
  } else {
    return true;
  }
}

bool assert_equals_int(char* msg, int expected, int actual) {
  if (expected != actual) {
    printf("Assertion error: expected %s to be %d but got %d\n", msg, expected, actual);
    return false;
  } else {
    return true;
  }
}

bool assert_map_equals(game_state_t* state, int x, int y, char expected) {
  char actual = get_board_at(state, x, y);
  if (expected != actual) {
    printf("Assertion error: at (%d, %d), expected %c but got %c\n", x, y, expected, actual);
    return false;
  } else {
    return true;
  }
}

// Note: This function only works for states created from create_default_state.
bool assert_state_equals(game_state_t* expected, game_state_t* actual) {
  // Check that width and height are equal
  if (!assert_equals_int("board width", expected->x_size, actual->x_size)) {
    return false;
  }
  if (!assert_equals_int("board height", expected->y_size, actual->y_size)) {
    return false;
  }

  // Check that boards are equal
  for (int x = 0; x < 14; x++) {
    for (int y = 0; y < 10; y++) {
      char expected_char = get_board_at(expected, x, y);
      char actual_char = get_board_at(actual, x, y);
      if (expected_char != actual_char) {
        printf("Assertion error: at (%d, %d), expected %c but got %c\n", x, y, expected_char, actual_char);
        return false;
      }
    }
  }

  // Check that num_snakes are equal
  if (!assert_equals_int("number of snakes", expected->num_snakes, actual->num_snakes)) {
    return false;
  }

  // Check that snake coordinates are equal
  // Note: This code only works for structs with one snake, so you probably won't be able to use it
  // in your implementation that supports multiple snakes.
  if (!assert_equals_int("x-coordinate of snake tail", expected->snakes->tail_x, actual->snakes->tail_x)) {
    return false;
  }
  if (!assert_equals_int("y-coordinate of snake tail", expected->snakes->tail_y, actual->snakes->tail_y)) {
    return false;
  }
  if (!assert_equals_int("x-coordinate of snake head", expected->snakes->head_x, actual->snakes->head_x)) {
    return false;
  }
  if (!assert_equals_int("y-coordinate of snake head", expected->snakes->head_y, actual->snakes->head_y)) {
    return false;
  }

  // Check that both snakes are alive or dead
  if (!assert_true("snake is alive", expected->snakes->live == actual->snakes->live)) {
    return false;
  }

  return true;
}

bool test_is_tail() {
  // TODO: Implement this if you'd like!
  printf("%s\n", "test_is_tail doesn't have any tests.");
  return true;
}

bool test_is_snake() {
  // TODO: Implement this if you'd like!
  printf("%s\n", "test_is_snake doesn't have any tests.");
  return true;
}

bool test_body_to_tail() {
  // TODO: Implement this if you'd like!
  printf("%s\n", "test_body_to_tail doesn't have any tests.");
  return true;
}

bool test_incr_x() {
  // TODO: Implement this if you'd like!
  printf("%s\n", "test_incr_x doesn't have any tests.");
  return true;
}

bool test_incr_y() {
  // TODO: Implement this if you'd like!
  printf("%s\n", "test_incr_y doesn't have any tests.");
  return true;
}

bool test_create_default_state() {
  game_state_t* state = create_default_state();

  if (state == NULL) {
    printf("%s\n", "create_default_state is not implemented, skipping...");
    return false;
  }

  if (!assert_equals_int("board width", 14, state->x_size)) {
    return false;
  }

  if (!assert_equals_int("board height", 10, state->y_size)) {
    return false;
  }

  for (int x = 0; x < 14; x++) {
    for (int y = 0; y < 10; y++) {
      if (x == 0 || y == 0 || x == 13 || y == 9) {
        if (!assert_map_equals(state, x, y, '#')) {
          return false;
        }
      } else if (x == 9 && y == 2) {
        if (!assert_map_equals(state, x, y, '*')) {
          return false;
        }
      } else if (x == 4 && y == 4) {
        if (!assert_map_equals(state, x, y, 'd')) {
          return false;
        }
      } else if (x == 5 && y == 4) {
        if (!assert_map_equals(state, x, y, '>')) {
          return false;
        }
      } else {
        if (!assert_map_equals(state, x, y, ' ')) {
          return false;
        }
      }
    }
  }

  if (!assert_equals_int("number of snakes", 1, state->num_snakes)) {
    return false;
  }

  if (!assert_equals_int("x-coordinate of snake tail", 4, state->snakes->tail_x)) {
    return false;
  }
  if (!assert_equals_int("y-coordinate of snake tail", 4, state->snakes->tail_y)) {
    return false;
  }
  if (!assert_equals_int("x-coordinate of snake head", 5, state->snakes->head_x)) {
    return false;
  }
  if (!assert_equals_int("y-coordinate of snake head", 4, state->snakes->head_y)) {
    return false;
  }
  if (!assert_true("snake is alive", state->snakes->live)) {
    return false;
  }

  return true;
}


bool test_free_state() {
  game_state_t* state = create_default_state();
  free_state(state);

  printf("%s\n", "Make sure that no Valgrind errors are printed!");

  return true;
}


bool test_print_board() {
  char* expected =
    "##############\n"
    "#            #\n"
    "#        *   #\n"
    "#            #\n"
    "#   d>       #\n"
    "#            #\n"
    "#            #\n"
    "#            #\n"
    "#            #\n"
    "##############\n";

  game_state_t* state = create_default_state();
  save_board(state, "unit-test-out.snk");

  // We hardcoded a filesize here since we know the default board is 150 characters.
  // You shouldn't hardcode sizes in your own implementation.
  char actual[151];

  FILE* f = fopen("unit-test-out.snk", "r");
  fread(actual, 150, 1, f);
  fclose(f);
  actual[150] = '\0';

  // check that file ends in a newline
  if (actual[strlen(actual) - 1] != '\n') {
    printf("%s\n", "Assertion error: Make sure your printed board ends in a newline.");
    return false;
  }

  if (strcmp(expected, actual) != 0) {
    printf("%s\n", "Your printed board doesn't match the expected output. See unit-test-out.snk for what you printed.");
    return false;
  }

  return true;
}

bool test_helpers() {
  if (!test_is_tail()) {
    printf("%s\n", "test_is_tail failed.");
    return false;
  }
  if (!test_is_snake()) {
    printf("%s\n", "test_is_snake failed.");
    return false;
  }
  if (!test_body_to_tail()) {
    printf("%s\n", "test_body_to_tail failed.");
    return false;
  }
  if (!test_incr_x()) {
    printf("%s\n", "test_incr_x failed.");
    return false;
  }
  if (!test_incr_y()) {
    printf("%s\n", "test_incr_y failed.");
    return false;
  }

  return true;
}

bool test_next_square_board_1() {
  /*
  Board 1 (default):
  ##############
  #            #
  #        *   #
  #            #
  #   d>       #
  #            #
  #            #
  #            #
  #            #
  ##############
  */

  // set up board
  game_state_t* actual_state = create_default_state();
  save_board(actual_state, "unit-test-out.snk");

  // the next square for the snake should be ' '
  if (!assert_equals_char("next_square on board 1", ' ', next_square(actual_state, 0))) {
    return false;
  }

  // check that board has not changed
  game_state_t* expected_state = create_default_state();
  if (!assert_state_equals(expected_state, actual_state)) {
    printf("%s\n", "Error: next_square should not modify board");
    return false;
  }
  return true;
}


bool test_next_square_board_2() {
  /*
  Board 2:
  ##############
  #            #
  #        *   #
  #            #
  #   d>*      #
  #            #
  #            #
  #            #
  #            #
  ##############
  */

  game_state_t* state = create_default_state();

  // set up board
  set_board_at(state, 6, 4, '*');
  save_board(state, "unit-test-out.snk");

  // the next square for the snake should be '*'
  return assert_equals_char("next_square on board 2", '*', next_square(state, 0));
}


bool test_next_square_board_3() {
  /*
  Board 3:
  ##############
  #            #
  #        *   #
  #            #
  #   d>x      #
  #            #
  #            #
  #            #
  #            #
  ##############
  */

  game_state_t* state = create_default_state();

  // set up board
  set_board_at(state, 6, 4, 'x');
  save_board(state, "unit-test-out.snk");

  // the next square for the snake should be 'x'
  return assert_equals_char("next_square on board 3", 'x', next_square(state, 0));
}

bool test_next_square_board_4() {
  /*
  Board 4:
  ##############
  #            #
  #        *   #
  #     #      #
  #   d>^      #
  #            #
  #            #
  #            #
  #            #
  ##############
  */

  game_state_t* state = create_default_state();

  // set up board
  set_board_at(state, 6, 4, '^');
  set_board_at(state, 6, 3, '#');
  state->snakes->head_x = 6;
  state->snakes->head_y = 4;
  save_board(state, "unit-test-out.snk");

  // the next square for the snake should be '#'
  return assert_equals_char("next_square on board 4", '#', next_square(state, 0));
}

bool test_next_square_board_5() {
  /*
  Board 5:
  ##############
  #            #
  #        *   #
  #            #
  #   d>v      #
  #     v      #
  #     v      #
  #            #
  #            #
  ##############
  */

  game_state_t* state = create_default_state();

  // set up board
  set_board_at(state, 6, 4, 'v');
  set_board_at(state, 6, 5, 'v');
  set_board_at(state, 6, 6, 'v');
  state->snakes->head_x = 6;
  state->snakes->head_y = 6;
  save_board(state, "unit-test-out.snk");

  // the next square for the snake should be ' '
  return assert_equals_char("next_square on board 5", ' ', next_square(state, 0));
}

bool test_next_square_board_6() {
  /*
  Board 6:
  ##############
  #            #
  #        *   #
  #            #
  #   d>v      #
  #     v      #
  #     v      #
  #     #      #
  #            #
  ##############
  */

  game_state_t* state = create_default_state();

  // set up board
  set_board_at(state, 6, 4, 'v');
  set_board_at(state, 6, 5, 'v');
  set_board_at(state, 6, 6, 'v');
  set_board_at(state, 6, 7, '#');
  state->snakes->head_x = 6;
  state->snakes->head_y = 6;
  save_board(state, "unit-test-out.snk");

  // the next square for the snake should be '#'
  return assert_equals_char("next_square on board 6", '#', next_square(state, 0));
}

bool test_next_square() {
  if (!test_next_square_board_1()) {
    printf("%s\n", "test_next_square_board_1 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_next_square_board_2()) {
    printf("%s\n", "test_next_square_board_2 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_next_square_board_3()) {
    printf("%s\n", "test_next_square_board_3 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_next_square_board_4()) {
    printf("%s\n", "test_next_square_board_4 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_next_square_board_5()) {
    printf("%s\n", "test_next_square_board_5 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_next_square_board_6()) {
    printf("%s\n", "test_next_square_board_6 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  return true;
}

bool test_update_head_board_1() {
  /*
  Board 1 (default):
  ##############        ##############
  #            #        #            #
  #        *   #        #        *   #
  #            #        #            #
  #   d>       # -----> #   d>>      #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  ##############        ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 6, 4, '>');
  expected->snakes->head_x = 6;
  expected->snakes->head_y = 4;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  save_board(actual, "unit-test-in.snk");

  update_head(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_head_board_2() {
  /*
  Board 2:
  ##############        ##############
  #            #        #            #
  #        *   #        #        *   #
  #            #        #     ^      #
  #   d>^      # -----> #   d>^      #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  ##############        ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 6, 4, '^');
  set_board_at(expected, 6, 3, '^');
  expected->snakes->head_x = 6;
  expected->snakes->head_y = 3;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 6, 4, '^');
  actual->snakes->head_x = 6;
  actual->snakes->head_y = 4;
  save_board(actual, "unit-test-in.snk");

  update_head(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_head_board_3() {
  /*
  Board 3:
  ##############        ##############
  #            #        #            #
  #        *   #        #        *   #
  #     <      #        #    <<      #
  #   d>^      # -----> #   d>^      #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  ##############        ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 6, 4, '^');
  set_board_at(expected, 6, 3, '<');
  set_board_at(expected, 5, 3, '<');
  expected->snakes->head_x = 5;
  expected->snakes->head_y = 3;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 6, 4, '^');
  set_board_at(actual, 6, 3, '<');
  actual->snakes->head_x = 6;
  actual->snakes->head_y = 3;
  save_board(actual, "unit-test-in.snk");

  update_head(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}


bool test_update_head() {
  if (!test_update_head_board_1()) {
    printf("%s\n", "test_update_head_board_1 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_head_board_2()) {
    printf("%s\n", "test_update_head_board_2 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_head_board_3()) {
    printf("%s\n", "test_update_head_board_3 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  return true;
}

bool test_update_tail_board_1() {
  /*
  Board 1:
  ##############        ##############
  #            #        #            #
  #        *   #        #        *   #
  #            #        #            #
  #   d>>      # -----> #    d>      #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  ##############        ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 4, ' ');
  set_board_at(expected, 5, 4, 'd');
  set_board_at(expected, 6, 4, '>');
  expected->snakes->head_x = 6;
  expected->snakes->head_y = 4;
  expected->snakes->tail_x = 5;
  expected->snakes->tail_y = 4;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 6, 4, '>');
  actual->snakes->head_x = 6;
  actual->snakes->head_y = 4;
  save_board(actual, "unit-test-in.snk");

  update_tail(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_tail_board_2() {
  /*
  Board 2:
  ##############        ##############
  #            #        #            #
  #        *   #        #        *   #
  #     ^      #        #     ^      #
  #    d^      # -----> #     w      #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  ##############        ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 4, ' ');
  set_board_at(expected, 5, 4, ' ');
  set_board_at(expected, 6, 4, 'w');
  set_board_at(expected, 6, 3, '^');
  expected->snakes->head_x = 6;
  expected->snakes->head_y = 3;
  expected->snakes->tail_x = 6;
  expected->snakes->tail_y = 4;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 4, 4, ' ');
  set_board_at(actual, 5, 4, 'd');
  set_board_at(actual, 6, 4, '^');
  set_board_at(actual, 6, 3, '^');
  actual->snakes->head_x = 6;
  actual->snakes->head_y = 3;
  actual->snakes->tail_x = 5;
  actual->snakes->tail_y = 4;
  save_board(actual, "unit-test-in.snk");

  update_tail(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_tail_board_3() {
  /*
  Board 3:
  ##############        ##############
  #            #        #            #
  #        *   #        #        *   #
  #    v<      #        #    va      #
  #     w      # -----> #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  ##############        ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 4, ' ');
  set_board_at(expected, 5, 4, ' ');
  set_board_at(expected, 6, 3, 'a');
  set_board_at(expected, 5, 3, 'v');
  expected->snakes->head_x = 5;
  expected->snakes->head_y = 3;
  expected->snakes->tail_x = 6;
  expected->snakes->tail_y = 3;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 4, 4, ' ');
  set_board_at(actual, 5, 4, ' ');
  set_board_at(actual, 6, 4, 'w');
  set_board_at(actual, 6, 3, '<');
  set_board_at(actual, 5, 3, 'v');
  actual->snakes->head_x = 5;
  actual->snakes->head_y = 3;
  actual->snakes->tail_x = 6;
  actual->snakes->tail_y = 4;
  save_board(actual, "unit-test-in.snk");

  update_tail(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}


bool test_update_tail() {
  if (!test_update_tail_board_1()) {
    printf("%s\n", "test_update_tail_board_1 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_tail_board_2()) {
    printf("%s\n", "test_update_tail_board_2 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_tail_board_3()) {
    printf("%s\n", "test_update_tail_board_3 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  return true;
}

bool test_update_state_board_1() {
  /*
  Board 1 (default):
  ##############        ##############
  #            #        #            #
  #        *   #        #        *   #
  #            #        #            #
  #   d>       # -----> #    d>      #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  ##############        ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 4, ' ');
  set_board_at(expected, 5, 4, 'd');
  set_board_at(expected, 6, 4, '>');
  expected->snakes->head_x = 6;
  expected->snakes->head_y = 4;
  expected->snakes->tail_x = 5;
  expected->snakes->tail_y = 4;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  save_board(actual, "unit-test-in.snk");

  update_state(actual, corner_food);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_state_board_2() {
  /*
  Board 2:
  ##############        ##############
  #            #        #*           #
  #        *   #        #        *   #
  #            #        #            #
  #   d>*      # -----> #   d>>      #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  ##############        ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 6, 4, '>');
  set_board_at(expected, 1, 1, '*');
  expected->snakes->head_x = 6;
  expected->snakes->head_y = 4;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 6, 4, '*');
  save_board(actual, "unit-test-in.snk");

  update_state(actual, corner_food);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_state_board_3() {
  /*
  Board 3:
  ##############        ##############
  #^           #        #x           #
  #^       *   #        #^       *   #
  #w           #        #w           #
  #            # -----> #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  ##############        ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 4, ' ');
  set_board_at(expected, 5, 4, ' ');
  set_board_at(expected, 1, 3, 'w');
  set_board_at(expected, 1, 2, '^');
  set_board_at(expected, 1, 1, 'x');
  expected->snakes->head_x = 1;
  expected->snakes->head_y = 1;
  expected->snakes->tail_x = 1;
  expected->snakes->tail_y = 3;
  expected->snakes->live = false;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 4, 4, ' ');
  set_board_at(actual, 5, 4, ' ');
  set_board_at(actual, 1, 3, 'w');
  set_board_at(actual, 1, 2, '^');
  set_board_at(actual, 1, 1, '^');
  actual->snakes->head_x = 1;
  actual->snakes->head_y = 1;
  actual->snakes->tail_x = 1;
  actual->snakes->tail_y = 3;
  save_board(actual, "unit-test-in.snk");

  update_state(actual, corner_food);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_state_board_4() {
  /*
  Board 4:
  ##############        ##############
  #            #        #            #
  #>v      *   #        #>v      *   #
  #w<          #        #wx          #
  #            # -----> #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  #            #        #            #
  ##############        ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 4, ' ');
  set_board_at(expected, 5, 4, ' ');
  set_board_at(expected, 1, 3, 'w');
  set_board_at(expected, 2, 2, 'v');
  set_board_at(expected, 2, 3, 'x');
  expected->snakes->head_x = 2;
  expected->snakes->head_y = 3;
  expected->snakes->tail_x = 1;
  expected->snakes->tail_y = 3;
  expected->snakes->live = false;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 4, 4, ' ');
  set_board_at(actual, 5, 4, ' ');
  set_board_at(actual, 1, 3, 'w');
  set_board_at(actual, 2, 2, 'v');
  set_board_at(actual, 2, 3, '<');
  actual->snakes->head_x = 2;
  actual->snakes->head_y = 3;
  actual->snakes->tail_x = 1;
  actual->snakes->tail_y = 3;
  save_board(actual, "unit-test-in.snk");

  update_state(actual, corner_food);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_state() {
  if (!test_update_state_board_1()) {
    printf("%s\n", "test_update_state_board_1 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_state_board_2()) {
    printf("%s\n", "test_update_state_board_2 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_state_board_3()) {
    printf("%s\n", "test_update_state_board_3 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_state_board_4()) {
    printf("%s\n", "test_update_state_board_4 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  return true;
}

bool test_load_board_1() {
  /*
  Board 1 (default):
  ##############
  #            #
  #        *   #
  #            #
  #   d>       #
  #            #
  #            #
  #            #
  #            #
  ##############
  */

  game_state_t* state = load_board("tests/1-simple-in.snk");

  if (state == NULL) {
    printf("%s\n", "load_board is not implemented, skipping...");
    return false;
  }

  if (!assert_equals_int("board width", 14, state->x_size)) {
    return false;
  }

  if (!assert_equals_int("board height", 10, state->y_size)) {
    return false;
  }

  char* expected = "##############\n#            #\n#        *   #\n#            #\n#   d>       #\n#            #\n#            #\n#            #\n#            #\n##############\n";

  save_board(state, "unit-test-out.snk");

  FILE* f = fopen("unit-test-out.snk", "r");
  fseek(f, 0, SEEK_END);
  long f_len = ftell(f);
  if (strlen(expected) != f_len) {
    printf("%s\n", "Your loaded board doesn't match the expected output. See unit-test-out.snk for what you loaded.");
    fclose(f);
    return false;
  }
  rewind(f);
  char* actual = malloc(sizeof(char) * (f_len + 1));
  if (actual == NULL) {
    printf("%s\n", "Failed to allocate memory for file.");
    return false;
  }
  fread(actual, f_len, 1, f);
  actual[f_len] = '\0';
  fclose(f);

  if (strcmp(expected, actual) != 0) {
    printf("%s\n", "Your loaded board doesn't match the expected output. See unit-test-out.snk for what you loaded.");
    return false;
  }
  free(actual);

  return true;
}

bool test_load_board_2() {
  /*
  Board 2 (small):
  #####
  #   #
  # ^ #
  # w #
  #####
  */

  game_state_t* state = load_board("tests/6-small-in.snk");

  if (state == NULL) {
    printf("%s\n", "load_board is not implemented, skipping...");
    return false;
  }

  if (!assert_equals_int("board width", state->x_size, 5)) {
    return false;
  }

  if (!assert_equals_int("board height", state->y_size, 5)) {
    return false;
  }

  char* expected = "#####\n#   #\n# ^ #\n# w #\n#####\n";

  save_board(state, "unit-test-out.snk");

  // We hardcoded a filesize here since we know the default board is 150 characters.
  // You shouldn't hardcode sizes in your own implementation.
  char actual[31];

  FILE* f = fopen("unit-test-out.snk", "r");
  fread(actual, 30, 1, f);
  fclose(f);
  actual[30] = '\0';

  if (strcmp(expected, actual) != 0) {
    printf("%s\n", "Your loaded board doesn't match the expected output. See unit-test-out.snk for what you loaded.");
    return false;
  }

  return true;
}

bool test_load_board() {
  if (!test_load_board_1()) {
    printf("%s\n", "test_load_board_1 failed. Check tests/1-simple-in.snk for a diagram of the board.");
    return false;
  }

  if (!test_load_board_2()) {
    printf("%s\n", "test_load_board_2 failed. Check tests/6-small-in.snk for a diagram of the board.");
    return false;
  }

  return true;
}


bool test_find_head_board_1() {
  /*
  Board 1:
  ##############
  #            #
  #        *   #
  #            #
  #   d>v      #
  #     v      #
  #  ^  v      #
  #  ^<<<      #
  #            #
  ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 6, 4, 'v');
  set_board_at(expected, 6, 5, 'v');
  set_board_at(expected, 6, 6, 'v');
  set_board_at(expected, 6, 7, '<');
  set_board_at(expected, 5, 7, '<');
  set_board_at(expected, 4, 7, '<');
  set_board_at(expected, 3, 7, '^');
  set_board_at(expected, 3, 6, '^');
  expected->snakes->head_x = 3;
  expected->snakes->head_y = 6;
  save_board(expected, "unit-test-out.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 6, 4, 'v');
  set_board_at(actual, 6, 5, 'v');
  set_board_at(actual, 6, 6, 'v');
  set_board_at(actual, 6, 7, '<');
  set_board_at(actual, 5, 7, '<');
  set_board_at(actual, 4, 7, '<');
  set_board_at(actual, 3, 7, '^');
  set_board_at(actual, 3, 6, '^');

  find_head(actual, 0);

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}


bool test_find_head() {
  if (!test_find_head_board_1()) {
    printf("%s\n", "test_find_head_board_1 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  return true;
}

bool test_initialize_snakes_board_1() {
  /*
  Board 1 (default):
  ##############
  #            #
  #        *   #
  #            #
  #   d>       #
  #            #
  #            #
  #            #
  #            #
  ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  save_board(expected, "unit-test-out.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  free(actual->snakes);
  actual->num_snakes = 0;

  actual = initialize_snakes(actual);

  if (actual == NULL) {
    printf("%s\n", "initialize_snakes is not implemented, skipping...");
    return false;
  }

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_initialize_snakes_board_2() {
  /*
  Board 2:
  ##############
  #            #
  #        *   #
  #            #
  #   d>v      #
  #     v      #
  #  ^  v      #
  #  ^<<<      #
  #            #
  ##############
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 6, 4, 'v');
  set_board_at(expected, 6, 5, 'v');
  set_board_at(expected, 6, 6, 'v');
  set_board_at(expected, 6, 7, '<');
  set_board_at(expected, 5, 7, '<');
  set_board_at(expected, 4, 7, '<');
  set_board_at(expected, 3, 7, '^');
  set_board_at(expected, 3, 6, '^');
  expected->snakes->head_x = 3;
  expected->snakes->head_y = 6;
  save_board(expected, "unit-test-out.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 6, 4, 'v');
  set_board_at(actual, 6, 5, 'v');
  set_board_at(actual, 6, 6, 'v');
  set_board_at(actual, 6, 7, '<');
  set_board_at(actual, 5, 7, '<');
  set_board_at(actual, 4, 7, '<');
  set_board_at(actual, 3, 7, '^');
  set_board_at(actual, 3, 6, '^');
  free(actual->snakes);
  actual->num_snakes = 0;

  actual = initialize_snakes(actual);

  if (actual == NULL) {
    printf("%s\n", "initialize_snakes is not implemented, skipping...");
    return false;
  }

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_initialize_snakes() {
  if (!test_initialize_snakes_board_1()) {
    printf("%s\n", "test_initialize_snakes_board_1 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_initialize_snakes_board_2()) {
    printf("%s\n", "test_initialize_snakes_board_2 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  return true;
}

void init_colors() {
  if (getenv("NO_COLOR") != NULL) {
    return;
  }

  char* term = getenv("TERM");
  if (term == NULL || strstr(term, "xterm") == NULL) {
    return;
  }

  COLOR_GREEN = "\033[0;32m";
  COLOR_RESET = "\033[0m";
}

bool test_and_print(char* label, bool (*run_test)()) {
  printf("\nTesting %s...\n", label);
  bool result = run_test();
  if (result) {
    printf("%sAll %s tests passed!%s\n", COLOR_GREEN, label, COLOR_RESET);
  } else {
    printf("Not all %s tests passed.\n", label);
  }
  return result;
}

/*
  Feel free to comment out tests.
*/
int main(int argc, char* argv[]) {
  bool MEMCHECK_MODE = false;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-m") == 0) {
      MEMCHECK_MODE = true;
      continue;
    }
    fprintf(stderr, "Usage: %s [-m]\n", argv[0]);
    return 1;
  }

  init_colors();

  printf("%s\n", "Reminder: These tests are not comprehensive, and passing them does not guarantee that your implementation is working.");

  if (MEMCHECK_MODE) {
    printf("\nTesting free_state...\n");
    if (!test_free_state()) {
      return 0;
    }
  } else {
    if (!test_and_print("create_default_state", test_create_default_state)) {
      return 0;
    }
    if (!test_and_print("print_board", test_print_board)) {
      return 0;
    }
    if (!test_and_print("helpers", test_helpers)) {
      return 0;
    }
    if (!test_and_print("next_square", test_next_square)) {
      return 0;
    }
    if (!test_and_print("update_head", test_update_head)) {
      return 0;
    }
    if (!test_and_print("update_tail", test_update_tail)) {
      return 0;
    }
    if (!test_and_print("update_state", test_update_state)) {
      return 0;
    }
    if (!test_and_print("load_board", test_load_board)) {
      return 0;
    }
    if (!test_and_print("find_head", test_find_head)) {
      return 0;
    }
    if (!test_and_print("initialize_snakes", test_initialize_snakes)) {
      return 0;
    }
  }
}
