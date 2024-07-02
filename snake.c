#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int INITIAL_SNAKE_SIZE = 10;
const int INITIAL_SNAKE_POSITION_X = 10;
const int INITIAL_SNAKE_POSITION_Y = 10;

typedef enum {
  LEFT,
  RIGHT,
  UP,
  DOWN,
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
  return game;
}

void slow_down(int milliseconds) {
  struct timespec req, rem;
  req.tv_sec = milliseconds / 1000;
  req.tv_nsec = (milliseconds % 1000) * 1000000L;
  nanosleep(&req, &rem);
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
  Game *game = init_game(rows, cols);
  if (game == NULL) {
    endwin();
    return 1;
  }

  while (game->running) {
    draw_game(game);
    int pressed = getch();
    switch (pressed) {
    case KEY_UP:
      game->snake->direction = UP;
    case KEY_LEFT:
      game->snake->direction = LEFT;
    case KEY_RIGHT:
      game->snake->direction = RIGHT;
    case KEY_DOWN:
      game->snake->direction = DOWN;
    }
    slow_down(200);
  }

  endwin();
  return 0;
}
