#include "state.h"
#include "snake_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_ROW (10)
#define BOARD_COL (14)

const char SNAKE_TAIL[4] = {'w', 'a', 's', 'd'};
const char SNAKE_BODY[5] = {'^', '<', 'v', '>', 'x'};

/* Helper function definitions */
static char get_board_at(game_state_t *state, int x, int y);
static void set_board_at(game_state_t *state, int x, int y, char ch);
static bool is_tail(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static int incr_x(char c);
static int incr_y(char c);
static void find_head(game_state_t *state, int snum);
static char next_square(game_state_t *state, int snum);
static void update_tail(game_state_t *state, int snum);
static void update_head(game_state_t *state, int snum);

/* Helper function to get a character from the board (already implemented for
 * you). */
static char get_board_at(game_state_t *state, int x, int y) {
    return state->board[y][x];
}

/* Helper function to set a character on the board (already implemented for
 * you). */
static void set_board_at(game_state_t *state, int x, int y, char ch) {
    state->board[y][x] = ch;
}

/* Task 1 */
game_state_t *create_default_state() {
    game_state_t *default_state = (game_state_t *)malloc(sizeof(game_state_t));

    /* Set board properties and create wall */
    default_state->x_size = BOARD_COL;
    default_state->y_size = BOARD_ROW;
    default_state->board = (char **)malloc(sizeof(char *) * BOARD_ROW);
    for (int i = 0; i < BOARD_ROW; ++i) {
        default_state->board[i] = (char *)malloc(sizeof(char) * BOARD_COL);
        if (i == 0 || i == BOARD_ROW - 1) {
            memset(default_state->board[i], '#', sizeof(char) * BOARD_COL);
        } else {
            memset(default_state->board[i], ' ', sizeof(char) * BOARD_COL);
            // overwrite the first and last element
            memset(default_state->board[i], '#', sizeof(char));
            memset(default_state->board[i] + BOARD_COL - 1, '#', sizeof(char));
        }
    }

    /* Create fruit */
    set_board_at(default_state, 9, 2, '*');

    /* Create snake state */
    default_state->num_snakes = 1;
    default_state->snakes = (snake_t *)malloc(sizeof(snake_t));
    default_state->snakes->head_x = 5;
    default_state->snakes->head_y = 4;
    default_state->snakes->tail_x = 4;
    default_state->snakes->tail_y = 4;
    default_state->snakes->live = true;
    set_board_at(default_state, default_state->snakes->head_x,
                 default_state->snakes->head_y, '>');
    set_board_at(default_state, default_state->snakes->tail_x,
                 default_state->snakes->tail_y, 'd');

    return default_state;
}

/* Task 2 */
void free_state(game_state_t *state) {
    if (state) {
        /* free snakes struct */
        for (int i = 0; i < state->num_snakes; ++i) {
            if (state->snakes + i) {
                free(state->snakes + i);
            }
        }
        /* free board struct */
        for (int i = 0; i < BOARD_ROW; i++) {
            if (state->board[i]) {
                free(state->board[i]);
            }
        }
        if (state->board) {
            free(state->board);
        }
        /* free game state struct */
        free(state);
    }

    return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
    for (int i = 0; i < BOARD_ROW; ++i) {
        fprintf(fp, "%s\n", state->board[i]);
    }

    return;
}

/* Saves the current state into filename (already implemented for you). */
void save_board(game_state_t *state, char *filename) {
    FILE *f = fopen(filename, "w");
    print_board(state, f);
    fclose(f);
}

/* Task 4.1 */
static bool is_tail(char c) {
    if (c == SNAKE_TAIL[TAIL_UP] || c == SNAKE_TAIL[TAIL_LEFT] ||
        c == SNAKE_TAIL[TAIL_RIGHT] || c == SNAKE_TAIL[TAIL_DOWN]) {
        return true;
    } else {
        return false;
    }
}

static bool is_snake(char c) {
    if (is_tail(c) || /* check tail and body*/
        c == SNAKE_BODY[BODY_UP] || c == SNAKE_BODY[BODY_LEFT] ||
        c == SNAKE_BODY[BODY_RIGHT] || c == SNAKE_BODY[BODY_DOWN] ||
        c == SNAKE_BODY[BODY_DEAD]) {
        return true;
    } else {
        return false;
    }
}

/* Call check function first */
static char body_to_tail(char c) {
    if (c == SNAKE_BODY[BODY_UP]) {
        return SNAKE_TAIL[TAIL_UP];
    } else if (c == SNAKE_BODY[BODY_LEFT]) {
        return SNAKE_TAIL[TAIL_LEFT];
    } else if (c == SNAKE_BODY[BODY_RIGHT]) {
        return SNAKE_TAIL[TAIL_RIGHT];
    } else if (c == SNAKE_BODY[BODY_DOWN]) {
        return SNAKE_TAIL[TAIL_DOWN];
    } else {
        return '?';
    }
}

static int incr_x(char c) {
    if (c == SNAKE_BODY[BODY_RIGHT] || c == SNAKE_TAIL[TAIL_RIGHT]) {
        return 1; // right
    } else if (c == SNAKE_BODY[BODY_LEFT] || c == SNAKE_TAIL[TAIL_LEFT]) {
        return -1; // left
    }
    return 0; // others
}

static int incr_y(char c) {
    if (c == SNAKE_BODY[BODY_DOWN] || c == SNAKE_TAIL[TAIL_DOWN]) {
        return 1; // right
    } else if (c == SNAKE_BODY[BODY_UP] || c == SNAKE_TAIL[TAIL_UP]) {
        return -1; // left
    }
    return 0; // others
}

/* Task 4.2 */
static char next_square(game_state_t *state, int snum) {
    /* snum [0, num_snakes - 1] */
    if (snum >= state->num_snakes || snum < 0) {
        printf("snake id incorrect, called by %s at %d\n", __func__, __LINE__);
        return '?';
    }

    if (!state->snakes[snum].live) {
        printf("snake has dead, called by %s at %d\n", __func__, __LINE__);
        return '?';
    }
    /* Get head direction */
    char head_dir = get_board_at(state, state->snakes[snum].head_x,
                                 state->snakes[snum].head_y);
    int dx = incr_x(head_dir);
    int dy = incr_y(head_dir);

    /* Update */
    if (dx != 0) {
        return get_board_at(state,
                            state->snakes[snum].head_x + incr_x(head_dir),
                            state->snakes[snum].head_y);
    } else if (dy != 0) {
        return get_board_at(state, state->snakes[snum].head_x,
                            state->snakes[snum].head_y + incr_y(head_dir));
    }

    return '?';
}

/* Task 4.3 */
static void update_head(game_state_t *state, int snum) {
    if (snum >= state->num_snakes || snum < 0) {
        printf("snake id incorrect, called by %s at %d\n", __func__, __LINE__);
        return;
    }

    if (!state->snakes[snum].live) {
        printf("snake has dead, called by %s at %d\n", __func__, __LINE__);
        return;
    }

    /* Get head direction */
    int x = state->snakes[snum].head_x;
    int y = state->snakes[snum].head_y;
    char head_dir = get_board_at(state, x, y);
    int dx = incr_x(head_dir);
    int dy = incr_y(head_dir);

    if (dx != 0) {
        set_board_at(state, x + dx, y, head_dir);
        state->snakes[snum].head_x += dx;
    } else if (dy != 0) {
        set_board_at(state, x, y + dy, head_dir);
        state->snakes[snum].head_y += dy;
    }
}

/* Task 4.4 */
static void update_tail(game_state_t *state, int snum) {
    if (snum >= state->num_snakes || snum < 0) {
        printf("snake id incorrect, called by %s at %d\n", __func__, __LINE__);
        return;
    }

    if (!state->snakes[snum].live) {
        printf("snake has dead, called by %s at %d\n", __func__, __LINE__);
        return;
    }

    /* Get head direction */
    int x = state->snakes[snum].tail_x;
    int y = state->snakes[snum].tail_y;
    char tail_dir = get_board_at(state, x, y); 
    set_board_at(state, x, y, ' ');

    int dx = incr_x(tail_dir);
    int dy = incr_y(tail_dir);

    if (dx != 0) {
        char next_tail = get_board_at(state, x + dx, y);
        set_board_at(state, x + dx, y, body_to_tail(next_tail));
        state->snakes[snum].tail_x += dx;
    } else if (dy != 0) {
        char next_tail = get_board_at(state, x, y + dy);
        set_board_at(state, x, y + dy, body_to_tail(next_tail));
        state->snakes[snum].tail_y += dy;
    }
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
    // TODO: Implement this function.
    return;
}

/* Task 5 */
game_state_t *load_board(char *filename) {
    // TODO: Implement this function.
    return NULL;
}

/* Task 6.1 */
static void find_head(game_state_t *state, int snum) {
    // TODO: Implement this function.
    return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
    // TODO: Implement this function.
    return NULL;
}
