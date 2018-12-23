/*
 MIT License

 Copyright (c) 2018 H. Watanabe

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <ncurses.h>
#include <random>
#include <future>

int px = 40;
const int py = 23;
bool now_playing = true;
bool has_ball = true;
double bx = 0.0, by = 0.0;
double vx = 0.0, vy = 0.0;

const int sx = 10, sy = 2;
const int wx = 60, wy = 5;
bool blocks[wx][wy];

void
draw_all(void) {
  clear();

  if (has_ball) {
    mvprintw(py - 1, px, "*");
  }
  mvprintw(py, px - 2, "-----");
  int x = static_cast<int>(bx);
  int y = static_cast<int>(by);
  if (!has_ball) {
    mvprintw(y, x, "*");
  }

  for (int i = 0; i < wx; i++)
  {
    for (int j = 0; j < wy; j++)
    {
      if(blocks[i][j]) mvprintw(j + sy, i + sx, "+");
    }
  }
  refresh();
}

void
paddle_collision_check(void) {
  if (by < 23.0) return;
  if (bx < px - 2)return;
  if (bx > px + 3)return;
  by = 23;
  double theta = M_PI * ((static_cast<double>(px) - bx + 1.5) / 8.0 + 0.25);
  vx = cos(theta) * 0.5;
  vy = -sin(theta) * 0.5;
}

void block_collision_check(void) {
  if (bx < sx)return;
  if (bx > sx + wx)return;
  if (by < sy)return;
  if (by > sy + wy)return;
  int x = static_cast<int>(bx) - sx;
  int y = static_cast<int>(by) - sy;
  if (!blocks[x][y])return;
  blocks[x][y] = false;
  double dx = bx - x + 0.5;
  double dy = by - y + 0.5;
  if (abs(dx) < abs(dy)) {
    vx = -vx;
  } else {
    vy = -vy;
  }
}

void
move_ball(void){
  if(has_ball)return;
  paddle_collision_check();
  block_collision_check();
  bx += vx;
  by += vy;
  if (bx < 0) {
    bx = 0;
    vx = -vx;
  }
  if (by < 0) {
    by = 0;
    vy = -vy;
  }
  if (bx > 80) {
    bx = 80;
    vx = -vx;
  }
  if (by > 24) {
    by = 24;
    vy = -vy;
    has_ball = true;
  }
}

void
game_loop(void) {
  while (now_playing) {
    move_ball();
    draw_all();
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
  }
}

int
main(void) {
  initscr();
  noecho();
  curs_set(0);
  for (int i = 0; i < wx; i++) {
    for (int j = 0; j < wy; j++) {
      blocks[i][j] = true;
    }
  }

  draw_all();
  auto th_game = std::thread([] {game_loop();});
  std::mt19937 mt;
  std::uniform_real_distribution<double> ud(0.0, 1.0);

  while (true) {
    int ch = getch();
    if (ch == 'q') break;
    if (has_ball && ch == ' ') {
      has_ball = false;
      bx = px;
      by = py - 1.0;
      double theta = (ud(mt) * 0.5 + 0.25) * M_PI;
      vx = cos(theta) * 0.5;
      vy = -sin(theta) * 0.5;
    }

    if(ch == 'l'){
      px += 3;
      if (px < 2)px = 2;
      if (px > 77)px = 77;
    }
    if(ch == 'h'){
      px -= 3;
      if (px < 2)px = 2;
      if (px > 77)px = 77;
    }
  }

  now_playing = false;
  th_game.join();
  endwin();
}
