#include <stdint.h>
#include <stdio.h>
#include "snake_utils.h"
#include "state.h"

uint32_t det_rand(uint32_t* state) {
  if (*state == 0) {
    *state = 1;
  }
  if (*state & 1) {
    *state = *state >> 1 ^ 0x80000057;
  } else {
    *state = *state >> 1;
  }
  return* state;
}

uint32_t seed = 1;

int deterministic_food(game_state_t* state) {
  unsigned int x = det_rand(&seed) % state->x_size;
  unsigned int y = det_rand(&seed) % state->y_size;
  while (state->board[y][x] != ' ') {
    x = det_rand(&seed) % state->x_size;
    y = det_rand(&seed) % state->y_size;
  }
  state->board[y][x] = '*';

  return 1;
}

int corner_food(game_state_t* state) {
  state->board[1][1] = '*';
  return 1;
}

void redirect_snake(game_state_t* state, char newhead) {
  unsigned int x;
  unsigned int y;
  x = state->snakes->head_x;
  y = state->snakes->head_y;

  if (!state->snakes->live) {
    return;
  }

  if (newhead == 'w') {
    state->board[y][x] = '^';
  } else if (newhead == 'a') {
    state->board[y][x] = '<';
  } else if (newhead == 's') {
    state->board[y][x] = 'v';
  } else if (newhead == 'd') {
    state->board[y][x] = '>';
  }
}

uint32_t snake_seed = 1;

void random_turn(game_state_t* state, int snum) {
  snake_t* snake = &(state->snakes[snum]);
  char cur_head = state->board[snake->head_y][snake->head_x];
  char* heads = "<v>^";
  int i;
  for (i = 0; i < 4; ++i) {
    if (heads[i] == cur_head) break;
  }
  if (det_rand(&snake_seed) % 2 == 0) {
    i += 1;
  } else {
    i -= 1;
  }
  i = i % 4;

  state->board[snake->head_y][snake->head_x] = heads[i];
}
