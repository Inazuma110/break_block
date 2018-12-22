#include <ncurses.h>

int
main(void) {
  initscr();
  noecho();
  curs_set(4);
  mvprintw(12, 30, "Hello World!");
  while (true) {
    int ch = getch();
    if (ch == 'q') break;
  }
  endwin();
}
