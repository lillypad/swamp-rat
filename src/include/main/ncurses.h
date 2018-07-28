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
#define CTRLD 	4

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

char *choices[] = {
  "Choice 1",
  "Choice 2",
  "Choice 3",
  "Choice 4",
  "Exit",
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

bool ncurses_test(){
    ITEM **my_items;
	int c;				
	MENU *my_menu;
    WINDOW *my_menu_win, *win_main;
    int n_choices;

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

    // set main window color
    init_pair(NCURSES_MAIN_WIN_COLOR, COLOR_GREEN, COLOR_BLACK);
    wbkgd(win_main, COLOR_PAIR(NCURSES_MAIN_WIN_COLOR));
    
    cbreak();
    noecho();
	keypad(stdscr, true);
	init_pair(1, COLOR_GREEN, COLOR_BLACK);

    n_choices = ARRAY_SIZE(choices);
    my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
    for(int i = 0; i < n_choices -1; ++i){
      my_items[i] = new_item(itoa(i+1), choices[i]);
    }

	my_menu = new_menu((ITEM **)my_items);

    my_menu_win = newwin(10, 40, 4, 4);
    wbkgd(my_menu_win, COLOR_PAIR(NCURSES_MAIN_WIN_COLOR));
    keypad(my_menu_win, TRUE);
     
    set_menu_win(my_menu, my_menu_win);
    set_menu_sub(my_menu, derwin(my_menu_win, 6, 38, 3, 1));

    set_menu_mark(my_menu, " -> ");
    
    box(my_menu_win, 0, 0);
    ncurses_print_menu_title(my_menu_win, 1, 0, 40, "Swamp RAT Victims");
    mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
	mvwhline(my_menu_win, 2, 1, ACS_HLINE, 38);
	mvwaddch(my_menu_win, 2, 39, ACS_RTEE);
	refresh();
        
	/* Post the menu */
	post_menu(my_menu);
	wrefresh(my_menu_win);

	while((c = wgetch(my_menu_win)) != KEY_F(1)){
      switch(c){
      case KEY_DOWN:
        menu_driver(my_menu, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(my_menu, REQ_UP_ITEM);
        break;
      }
      wrefresh(my_menu_win);
	}	

    unpost_menu(my_menu);
    free_menu(my_menu);
    for(int i = 0; i < n_choices; ++i){
                free_item(my_items[i]);
    }
	endwin();
  return true;
}

bool ncurses_main(){
  /*
    :TODO: initalize main window
    :returns: boolean
  */
  int key;
  WINDOW *win_main;
  char win_main_title[] = "|Swamp RAT|";
  
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
    wrefresh(win_main);
  }

  // cleanup
  ncurses_window_cleanup(win_main);
  
  return true;
}
