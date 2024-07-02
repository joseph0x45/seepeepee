#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int INITIAL_SNAKE_SIZE = 10;
const int INITIAL_SNAKE_POSITION_X = 5;
const int INITIAL_SNAKE_POSITION_Y = 5;

typedef enum {
  LEFT,
  RIGHT,
  UP,
  DOWN,
  INVALID,
} Directions;

typedef struct {
  int X;
  int Y;
} Point;

typedef struct {
  Point **body;
  size_t size;
  Directions direction;
} Snake;

typedef struct {
  Snake *snake;
  Point food;
  int running;
} Game;

void translate_point(Point *point, Directions direction) {
  switch (direction) {
  case LEFT:
    point->X--;
    break;
  case RIGHT:
    point->X++;
    break;
  case UP:
    point->Y--;
    break;
  case DOWN:
    point->Y++;
    break;
  case INVALID:
    return;
  }
}

Directions key_press_to_direction(int key_press) {
  switch (key_press) {
  case KEY_UP:
    return UP;
  case KEY_DOWN:
    return DOWN;
  case KEY_LEFT:
    return LEFT;
  case KEY_RIGHT:
    return RIGHT;
  }
  return INVALID;
}

const char *direction_to_str(Directions d) {
  switch (d) {
  case UP:
    return "up";
  case DOWN:
    return "down";
  case LEFT:
    return "left";
  case RIGHT:
    return "right";
  case INVALID:
    return "invalid";
  }
}

int directions_opposed(Directions d1, Directions d2) {
  int left_right = (d1 == LEFT && d2 == RIGHT) || (d1 == RIGHT && d2 == LEFT);
  int up_down = (d1 == UP && d2 == DOWN) || (d1 == DOWN && d2 == UP);
  if (left_right || up_down) {
    return 1;
  }
  return 0;
}

void free_game(Game *game) {
  if (game->snake->body != NULL) {
    free(game->snake->body);
  }
  if (game->snake != NULL) {
    free(game->snake);
  }
  if (game != NULL) {
    free(game);
  }
}

Point get_random_point(int maxY, int maxX) {
  Point point;
  point.Y = rand() % maxY;
  point.X = rand() % maxX;
  return point;
}

Game *init_game(int maxX, int maxY) {
  Game *game = malloc(sizeof(Game));
  if (game == NULL) {
    perror("Failed to allocate memory for game");
    return NULL;
  }
  game->snake = malloc(sizeof(Snake));
  if (game->snake == NULL) {
    perror("Failed to allocate memory for snake");
    free_game(game);
    return NULL;
  }
  game->snake->body = malloc(sizeof(Point *) * INITIAL_SNAKE_SIZE);
  if (game->snake->body == NULL) {
    perror("Failed to allocate memory for snake body");
    free_game(game);
    return NULL;
  }
  game->snake->size = INITIAL_SNAKE_SIZE;
  game->running = 1;
  game->food = get_random_point(maxY, maxX);
  Point *snake_initial_position = malloc(sizeof(Point));
  snake_initial_position->X = INITIAL_SNAKE_POSITION_X;
  snake_initial_position->Y = INITIAL_SNAKE_POSITION_Y;
  if (snake_initial_position == NULL) {
    perror("Failed to allocate memory for initial snake position");
    free_game(game);
    return NULL;
  }
  game->snake->body[0] = snake_initial_position;
  game->snake->direction = RIGHT;
  return game;
}

void move_snake(Snake *snake, Directions direction) {
  if (direction == INVALID) {
    return;
  }
  if (directions_opposed(snake->direction, direction) == 1) {
    return;
  }
  snake->direction = direction;
  for (int i = 0; i < snake->size; i++) {
    Point *p = snake->body[i];
    if (p == NULL) {
      break;
    }
    translate_point(p, snake->direction);
  }
}

void slow_down(int milliseconds) {
  struct timespec req, rem;
  req.tv_sec = milliseconds / 1000;
  req.tv_nsec = (milliseconds % 1000) * 1000000L;
  nanosleep(&req, &rem);
}

void print_current_game_stats(Game *game) {
  // print current snake direction
  mvprintw(1, 1, "%s", direction_to_str(game->snake->direction));
  // print snake head
  mvprintw(2, 2, "X %d; Y %d", game->snake->body[0]->X,
           game->snake->body[0]->Y);
}

void draw_game(Game *game) {
  mvprintw(game->food.Y, game->food.X, "o");
  for (int i = 0; i < game->snake->size; i++) {
    Point *point = game->snake->body[i];
    if (point == NULL) {
      break;
    }
    mvprintw(point->Y, point->X, "#");
  }
  print_current_game_stats(game);
  refresh();
}

int main(int argc, char **argv) {
  int rows, cols;
  srand(time(NULL));
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, true);
  getmaxyx(stdscr, cols, rows);
  nodelay(stdscr, TRUE);
  Game *game = init_game(rows, cols);
  if (game == NULL) {
    endwin();
    return 1;
  }

  while (game->running) {
    draw_game(game);
    int pressed = getch();
    if (pressed == ERR) {
      printw("no input");
      refresh();
      pressed = game->snake->direction;
    }
    move_snake(game->snake, key_press_to_direction(pressed));
    refresh();
    slow_down(200);
    clear();
  }

  endwin();
  return 0;
}
