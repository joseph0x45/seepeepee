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
  }
}

void set_next_direction(Snake *snake, int key_press) {
  switch (key_press) {
  case KEY_UP:
    if (snake->direction == DOWN) {
      break;
    }
    snake->direction = UP;
    break;
  case KEY_DOWN:
    if (snake->direction == UP) {
      break;
    }
    snake->direction = DOWN;
    break;
  case KEY_LEFT:
    if (snake->direction == RIGHT) {
      break;
    }
    snake->direction = LEFT;
    break;
  case KEY_RIGHT:
    if (snake->direction == LEFT) {
      break;
    }
    snake->direction = RIGHT;
    break;
  }
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
  }
}

void free_game(Game *game) {
  if (game->snake->body != NULL) {
    free(game->snake->body);
  }
  if (game->snake != NULL) {
    for (int i = 0; i < game->snake->size; i++) {
      if (game->snake->body[i] != NULL) {
        free(game->snake->body[i]);
      }
    }
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
  Point *p1 = malloc(sizeof(Point));
  p1->X = INITIAL_SNAKE_POSITION_X - 1;
  p1->Y = INITIAL_SNAKE_POSITION_Y;
  game->snake->body[1] = p1;
  game->snake->direction = RIGHT;
  return game;
}

void move_snake(Snake *snake) {
  for (int i = snake->size - 1; i > 0; i--) {
    if (snake->body[i] == NULL) {
      continue;
    }
    snake->body[i]->X = snake->body[i - 1]->X;
    snake->body[i]->Y = snake->body[i - 1]->Y;
  }
  translate_point(snake->body[0], snake->direction);
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
  nodelay(stdscr, TRUE);
  Game *game = init_game(rows, cols);
  if (game == NULL) {
    endwin();
    return 1;
  }

  while (game->running) {
    draw_game(game);
    // check if user lost
    if (game->snake->body[0]->X <= 0 || game->snake->body[0]->Y <= 0) {
      game->running = 0;
    }
    int pressed = getch();
    set_next_direction(game->snake, pressed);
    move_snake(game->snake);
    refresh();
    slow_down(200);
    clear();
  }
  endwin();
  printf("You lost\n");
  return 0;
}
