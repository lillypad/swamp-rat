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
#include <ncurses.h>
#include <form.h>
#include <menu.h>
#include <stdbool.h>
#include <errno.h>
#include "../common.h"


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

int y_margin_form = 4;
int x_margin_form = 8;

bool ncurses_forms_command_select(WINDOW *win_main, WINDOW *win_menu, char *uuid){
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
  cbreak();
  noecho();
  wclear(win_menu);
  menu = new_menu((ITEM **)item_options);
  getmaxyx(win_main, y, x);
  wresize(win_menu,
          (y - y_margin_form),
          (x - x_margin_form));
  keypad(win_menu, TRUE);
  set_menu_win(menu, win_menu);
  set_menu_sub(menu, derwin(win_menu, y - (y_margin_form * 2), x - (x_margin_form * 2), 3, 1));
  set_menu_mark(menu, " -> ");
  set_menu_format(menu, 5, 1);
  box(win_menu, 0, 0);
  mvwprintw(win_menu, 1, (x / 2) - x_margin_form - 10, "%s", uuid);
  mvwaddch(win_menu, 2, 0, ACS_LTEE);
  mvwhline(win_menu, 2, 1, ACS_HLINE, (x - x_margin_form) - 2);
  mvwaddch(win_menu, 2, (x - x_margin_form) - 1, ACS_RTEE);
  post_menu(menu);
  refresh();
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
    if (key == 10 || key == KEY_ENTER){
      break;
    }
    wrefresh(win_menu);
  }
  unpost_menu(menu);
  free_menu(menu);
  for(int i = 0; i < n_choices; ++i){
    free_item(item_options[i]);
  }
  return true;
}
