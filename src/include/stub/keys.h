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

#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <linux/limits.h>

#ifndef KEYS_ASYNC
#define KEYS_ASYNC_FALSE 0
#define KEYS_ASYNC_TRUE  1
#define KEYS_ASYNC
#endif

#ifndef KEYS_GET_ALL_ARGS
typedef struct{
  char keyboard_dev[PATH_MAX];
} keys_get_all_args_t;
#define KEYS_GET_ALL_ARGS
#endif

int keys_get_fd(char *keyboard_dev){
  /*
    :TODO: get keyboard file descriptor from device path
    :keyboard_dev: (char *) device path
    :returns: (int) keyboard file descriptor
  */
  int keyboard_fd = open(keyboard_dev, O_RDONLY);
  if (keyboard_fd < 0){
    fprintf(stderr,
            "error: unable to get file descriptor for %s\n",
            keyboard_dev);
    return -1;
  } else{
    return keyboard_fd;
  }
}

void *keys_get_all_main(void *args){
  /*
    :TODO: start capturing all keys even w/o x11
    :returns: boolean
  */
  keys_get_all_args_t *p_keys_get_all_args = args;
  char *keyboard_dev = p_keys_get_all_args->keyboard_dev;
  int keyboard_fd = keys_get_fd(keyboard_dev);
  if (keyboard_fd < 0){
    return false;
  }
  struct input_event ev;
  while (1){
    read(keyboard_fd, &ev, sizeof(struct input_event));
    if(ev.type == 1){
      printf("key %i state %i\n", ev.code, ev.value);
    }
  }
  return NULL;
}

bool key_get_all(char *keyboard_dev, int async){
  keys_get_all_args_t *p_keys_get_all_args = malloc(sizeof(keys_get_all_args_t));
  p_keys_get_all_args->keyboard_dev = keyboard_dev;
  if (async == KEYS_ASYNC_TRUE){
    pthread_t keys_get_all_thread;
    pthread_create(&keys_get_all_thread,
                   NULL,
                   keys_get_all_main,
                   p_keys_get_all_args);
    if (async == KEYS_ASYNC_FALSE){
      pthread_join(keys_get_all_thread, NULL);
      free(p_keys_get_all_args);
    }
  }
  return true;
}
