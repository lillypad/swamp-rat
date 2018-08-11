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
#include <stdbool.h>
#include <argp.h>
#include "include/main/ncurses.h"
//#include "include/main/net.h"

const char *program_version = "swamp-rat 0.9b";

const char *program_bug_address = "lillypadgirl86@gmail.com";

static char program_doc[] = "swamp-rat --- A Linux Rat in C";

static char program_args_doc[] = "-p 4444";

static struct argp_option program_options[] = {
  {"port", 'p', 0, 0,  "listen / config port"},
  {"host", 'h', 0, 0, "config host"},
  {"version", 'v', 0, 0, "display version"},
  {"config", 'c', 0, 0, "write config"},
  { 0 }
};

struct arguments{
  char *args[2];
  int port;
};

static error_t parse_opt(int key,
                         char *arg,
                         struct argp_state *state){
  struct arguments *arguments = state->input;
  switch(key){
  case 'p':
    arguments->port = atoi(arg);
  case 'v':
    printf("%s\n", program_version);
    exit(EXIT_SUCCESS);
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {
  program_options,
  parse_opt,
  program_args_doc,
  program_doc
};

int main(int argc, char **argv){
  struct arguments arguments;
  arguments.port = 4444;
  argp_parse(&argp, argc, argv, 0, 0, &arguments);
  ncurses_main(4444);
  return EXIT_FAILURE;
}
