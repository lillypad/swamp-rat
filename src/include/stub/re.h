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
#include <sys/ptrace.h>

bool re_ptrace(){
  /*
    :TODO: check for debugger
    :returns: boolean
  */
  if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1){
    return true;
  } else{
    return false;
  }
}

#ifndef RE_KERNEL_MODULE_NAME_MAX_SIZE
#define RE_KERNEL_MODULE_NAME_MAX_SIZE 32
#endif

#ifndef RE_BASH_COMMAND_MAX_LEN
#define RE_BASH_COMMAND_MAX_LEN 256
#endif

bool re_kernel_module(char *kernel_module){
  /*
    :TODO: check if kernel module is loaded
    :returns: boolean
  */
  if (strlen(kernel_module) + 16 > RE_BASH_COMMAND_MAX_LEN){
    fprintf(stderr, "[x] kernel module name length exceeds limitations\n");
    return false;
  }
  char command[RE_BASH_COMMAND_MAX_LEN];
  sprintf(command, "grep -Po '^%s\x20' /proc/modules", kernel_module);
  FILE *fd = popen(command, "r");
  if (fd == NULL){
    fprintf(stderr, "[x] failed to read modules");
    return false;
  }
  char buff[RE_KERNEL_MODULE_NAME_MAX_SIZE];
  int ret = fread(buff, 1, sizeof(buff), fd);
  if (ret > 0){
    return true;
  } else{
    return false;
  }
}
