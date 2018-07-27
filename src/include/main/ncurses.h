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

bool ncurses_window_cleanup(WINDOW *win){
  /*
    :TODO: clean up window
    :returns: boolean
  */
  delwin(win);
  endwin();
  refresh();
  return true;
}

bool ncurses_window_border(WINDOW *win){
  /*
    :TODO: draw window border
    :returns: boolean
  */
  box(win, 0, 0);
  wrefresh(win);
  return true;
}

bool ncurses_window_title(WINDOW *win, char *title){
  /*
    :TODO: draw window title
    :win: pointer to window struct
    :title: title of window
    :returns: boolean
  */
  int x, y;
  getmaxyx(win, y, x);
  mvprintw(0, ((x/2) - (strlen(title)/2)), title);
  wrefresh(win);
  return true;
}

bool ncurses_window_clients(WINDOW *win_main,
                            WINDOW *win,
                            int x_pos,
                            int y_pos,
                            int x,
                            int y){
  /*
    :TODO: draw clients window
    :win_main: main window
    :win: clients window struct
    :x_pos: start x position
    :y_pos: start y position
    :x: x width
    :y: y height
  */
  return true;
}

#ifndef NCURSES_MAIN_WIN_COLOR
#define NCURSES_MAIN_WIN_COLOR 1
#endif

bool ncurses_main(){
  /*
    :TODO: initalize main window
    :returns: boolean
  */
  int key;
  char win_main_title[] = "|Swamp RAT|";
  
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

  init_pair(NCURSES_MAIN_WIN_COLOR, COLOR_GREEN, COLOR_BLACK);
  wbkgd(win_main, COLOR_PAIR(NCURSES_MAIN_WIN_COLOR));

  ncurses_window_border(win_main);
  ncurses_window_title(win_main, win_main_title);

  while ((key = getch()) != 27){
    if (key == KEY_RESIZE){
      clear();
      ncurses_window_border(win_main);
      ncurses_window_title(win_main, win_main_title);
    }
  }

  ncurses_window_cleanup(win_main);
  
  return true;
}
