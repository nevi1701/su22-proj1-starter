#include "state.h"
#include "snake_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int x, unsigned int y,
                         char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_x(unsigned int cur_x, char c);
static unsigned int get_next_y(unsigned int cur_y, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
    // TODO: Implement this function.
    game_state_t *state = malloc(sizeof(game_state_t));
    if (state == NULL) {
        perror("Malloc failed\n");
    }
    state->num_rows = 18;
    state->num_snakes = 1;
    state->board = (char **)malloc(18 * sizeof(char *));
    if (state->board == NULL) {
        perror("Malloc failed\n");
    }
    for (unsigned int i = 0; i < 18; i++) {
        state->board[i] = (char *)malloc(20 * sizeof(char));
        if (state->board[i] == NULL) {
            perror("Malloc failed\n");
        }
    }
    for (unsigned int i = 0; i < 20; i++) {
        for (unsigned int j = 0; j < 18; j++) {
            set_board_at(state, i, j, '#');
        }
    }
    for (unsigned int i = 1; i < 19; i++) {
        for (unsigned int j = 1; j < 17; j++) {
            set_board_at(state, i, j, ' ');
        }
    }
    state->snakes = malloc(sizeof(snake_t));
    if (state->snakes == NULL) {
        perror("Malloc failed\n");
    }
    state->snakes[0].head_x = 4;
    state->snakes[0].head_y = 2;
    state->snakes[0].live = true;
    state->snakes[0].tail_x = 2;
    state->snakes[0].tail_y = 2;

    set_board_at(state, state->snakes[0].tail_x, state->snakes[0].tail_y, 'd');
    set_board_at(state, state->snakes[0].head_x, state->snakes[0].head_y, 'D');
    set_board_at(state, 9, 2, '*');
    set_board_at(state, 3, 2, '>');

    return state;
}

/* Task 2 */
void free_state(game_state_t *state) {
    // TODO: Implement this function.
    for (unsigned int i = 0; i < state->num_rows; i++) {
        free(state->board[i]);
    }
    free(state->board);
    free(state->snakes);
    free(state);

    return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
    // TODO: Implement this function.
    for (unsigned int i = 0; i < state->num_rows; i++) {
        fprintf(fp, "%s\n", state->board[i]);
    }
    return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
    FILE *f = fopen(filename, "w");
    print_board(state, f);
    fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int x, unsigned int y) {
    return state->board[y][x];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int x, unsigned int y,
                         char ch) {
    state->board[y][x] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
    // TODO: Implement this function.
    if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
        return true;
    }
    return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
    // TODO: Implement this function.
    if (c == 'W' || c == 'A' || c == 'S' || c == 'D') {
        return true;
    }
    return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<>vWASDx"
*/
static bool is_snake(char c) {
    // TODO: Implement this function.
    if (c == 'w' || c == 'a' || c == 's' || c == 'd' || c == '^' || c == '<' ||
        c == '>' || c == 'v' || c == 'W' || c == 'A' || c == 'S' || c == 'D' ||
        c == 'x') {
        return true;
    }

    return false;
}

/*
  Converts a character in the snake's body ("^<>v")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
    // TODO: Implement this function.
    if (c == '^') {
        return 'w';
    }
    if (c == '<') {
        return 'a';
    }
    if (c == 'v') {
        return 's';
    }
    if (c == '>') {
        return 'd';
    } else {
        return c;
    }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<>v").
*/
static char head_to_body(char c) {
    // TODO: Implement this function.
    if (c == 'D') {
        return '>';
    }
    if (c == 'S') {
        return 'v';
    }
    if (c == 'A') {
        return '<';
    }
    if (c == 'W') {
        return '^';
    } else {
        return c;
    }
}

/*
  Returns cur_x + 1 if c is '>' or 'd' or 'D'.
  Returns cur_x - 1 if c is '<' or 'a' or 'A'.
  Returns cur_x otherwise.
*/
static unsigned int get_next_x(unsigned int cur_x, char c) {
    // TODO: Implement this function.
    if (c == '<' || c == 'a' || c == 'A') {
        return cur_x - 1;
    }
    if (c == '>' || c == 'd' || c == 'D') {
        return cur_x + 1;
    } else {
        return cur_x;
    }
}

/*
  Returns cur_y + 1 if c is '^' or 'w' or 'W'.
  Returns cur_y - 1 if c is 'v' or 's' or 'S'.
  Returns cur_y otherwise.
*/
static unsigned int get_next_y(unsigned int cur_y, char c) {
    // TODO: Implement this function.
    if (c == '^' || c == 'w' || c == 'W') {
        return cur_y - 1;
    }
    if (c == 'v' || c == 's' || c == 'S') {

        return cur_y + 1;
    } else {
        return cur_y;
    }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake
  is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
    // TODO: Implement this function.
    unsigned int head_x = state->snakes[snum].head_x;
    unsigned int head_y = state->snakes[snum].head_y;
    char c = get_board_at(state, head_x, head_y);
    return get_board_at(state, get_next_x(head_x, c), +get_next_y(head_y, c));
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the x and y coordinates of the head

  Note that this function ignores food, walls, and snake bodies when moving the
  head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
    // TODO: Implement this function.
    char c = get_board_at(state, state->snakes[snum].head_x,
                          state->snakes[snum].head_y);
    set_board_at(state, get_next_x(state->snakes[snum].head_x, c),
                 get_next_y(state->snakes[snum].head_y, c), c);
    set_board_at(state, state->snakes[snum].head_x, state->snakes[snum].head_y,
                 head_to_body(c));
    state->snakes[snum].head_x = get_next_x(state->snakes[snum].head_x, c);
    state->snakes[snum].head_y = get_next_y(state->snakes[snum].head_y, c);
    return;
}
/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^v<>) into a tail character (wasd)

  ...in the snake struct: update the x and y coordinates of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
    // TODO: Implement this function.
    char c = get_board_at(state, state->snakes[snum].tail_x,
                          state->snakes[snum].tail_y);
    set_board_at(state, state->snakes[snum].tail_x, state->snakes[snum].tail_y,
                 ' ');
    set_board_at(state, get_next_x(state->snakes[snum].tail_x, c),
                 get_next_y(state->snakes[snum].tail_y, c),
                 body_to_tail(get_board_at(
                     state, get_next_x(state->snakes[snum].tail_x, c),
                     get_next_y(state->snakes[snum].tail_y, c))));
    state->snakes[snum].tail_x = get_next_x(state->snakes[snum].tail_x, c);
    state->snakes[snum].tail_y = get_next_y(state->snakes[snum].tail_y, c);

    return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
    // TODO: Implement this function.
    for (unsigned int i = 0; i < state->num_snakes; i++) {
        if (state->snakes[i].live == true) {
            unsigned int head_x = state->snakes[i].head_x;
            unsigned int head_y = state->snakes[i].head_y;
            char n = next_square(state, i);
            if (n == '#' || is_snake(n) == true) {
                state->snakes[i].live = false;
                set_board_at(state, head_x, head_y, 'x');
                return;
            }
            if (n == '*') {
                update_head(state, i);
                add_food(state);
            } else {
                update_head(state, i);
                update_tail(state, i);
            }
        }
    }
    return;
}

/* Task 5 */
game_state_t *load_board(char *filename) {
    // TODO: Implement this function.
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("Cant open for reading\n");
        return NULL;
    }
    char *buffer = malloc(sizeof(char) * 1000);
    unsigned int i = 0;
    while (fgets(buffer, 1000, f)) {
        i++;
    }
    rewind(f);
    game_state_t *state = malloc(sizeof(game_state_t));
    if (state == NULL) {
        perror("Malloc failed\n");
    }
    state->board = malloc(sizeof(char *) * i);
    if (state->board == NULL) {
        perror("Malloc failed\n");
    }
    unsigned int k = 0;
    while (fgets(buffer, 1000, f)) {
        buffer[strlen(buffer) - 1] = '\0';
        state->board[k] = malloc(sizeof(char) * strlen(buffer));
        if (state->board[k] == NULL) {
            perror("Malloc failed\n");
        }
        strcpy(state->board[k], buffer);
        k++;
    }
    state->num_rows = i;

    free(buffer);
    fclose(f);
    return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail coordinates filled in,
  trace through the board to find the head coordinates, and
  fill in the head coordinates in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
    // TODO: Implement this function.
    state->snakes[snum].head_x = state->snakes[snum].tail_x;
    state->snakes[snum].head_y = state->snakes[snum].tail_y;
    while (is_snake(next_square(state, snum))) {
        char c = get_board_at(state, state->snakes[snum].head_x,
                              state->snakes[snum].head_y);
        if (is_head(c) == true) {
            return;
        }

        state->snakes[snum].head_x = get_next_x(state->snakes[snum].head_x, c);
        state->snakes[snum].head_y = get_next_y(state->snakes[snum].head_y, c);
    }
    return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
    // TODO: Implement this function.
    state->snakes = malloc(sizeof(snake_t));
    for (unsigned int i = 0; i < state->num_rows; i++) {
        for (unsigned int j = 0; j < strlen(state->board[i]); j++) {
            if (is_tail(get_board_at(state, j, i))) {
                state->snakes = realloc(state->snakes, (state->num_snakes + 1) *
                                                           sizeof(snake_t));
                state->snakes[state->num_snakes].tail_x = j;
                state->snakes[state->num_snakes].tail_y = i;
                state->snakes[state->num_snakes].live = true;
                find_head(state, state->num_snakes);
                state->num_snakes++;
            }
        }
    }
    return state;
}
