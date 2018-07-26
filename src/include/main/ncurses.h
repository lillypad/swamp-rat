/*-----------------------------------------------------------------------\
| Copyright (C) 2018  Lilly Chalupowski                                  |
|                                                                        |
| This program is free software: you can redistribute it and/or modify   |
| it under the terms of the GNU General Public License as published by   |
| the Free Software Foundation, either version 3 of the License, or      |
| (at your option) any later version.                                    |
|                                                                        |
| This program is distributed in the hope that it will be useful,        |
| but WITHOUT ANY WARRANTY; without even the implied warranty of         |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          |
| GNU General Public License for more details.                           |
|                                                                        |
| You should have received a copy of the GNU General Public License      |
| along with this program.  If not, see <https://www.gnu.org/licenses/>. |
\-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <errno.h>

#ifndef NCURSES_MAIN_WIN_COLOR
#define NCURSES_MAIN_WIN_COLOR 1
#endif

bool ncurses_window_cleanup(WINDOW *win){
  delwin(win);
  endwin();
  refresh();
  return true;
}

bool ncurses_window_border(WINDOW *win){
  int x, y;
  getmaxyx(win, y, x);
  mvwprintw(win, 0, 0, "/");
  mvwprintw(win, y -1, 0, "\\");
  mvwprintw(win, 0, x - 1, "\\");
  mvwprintw(win, y -1 , x -1, "/");
  for (int i = 1; i < (y -1); i++){
    mvwprintw(win, i, 0, "|");
    mvwprintw(win, i, x - 1, "|");
  }
  for (int i = 1; i < (x - 1); i++){
    mvwprintw(win, 0, i, "-");
    mvwprintw(win, y - 1, i, "-");
  }
  return true;
}

#define MAIN_WIN_COLOR 1

bool ncurses_main(){
  /*
    :TODO: initalize main window
  */

  // let ncurses choose right terminal env
  if(putenv("TERM=linux") != 0){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  WINDOW *win_main;
  if ((win_main = initscr()) == NULL){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  if (start_color() == ERR || !has_colors() || !can_change_color()){
    ncurses_window_cleanup(win_main);
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  init_pair(MAIN_WIN_COLOR, COLOR_GREEN, COLOR_BLACK);
  wbkgd(win_main, COLOR_PAIR(MAIN_WIN_COLOR));

  //ncurses_window_border(win_main);
  box(win_main, 0, 0);
  wrefresh(win_main);

  getch();

  ncurses_window_cleanup(win_main);
  
  return true;
}
