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
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>
#include "../defs.h"

char *sys_get_user(){
  /*
    :TODO: get current username
    :returns: username
  */
  char *username = malloc(MAX_USERNAME_LEN);
  getlogin_r(username, MAX_USERNAME_LEN);
  return username;
}

char *sys_get_os(){
  /*
    :TODO: get system name
    :returns: system name
  */
  struct utsname p_utsname;
  uname(&p_utsname);
  char *sysname = p_utsname.sysname;
  return sysname;
}

char *sys_get_release(){
  /*
    :TODO: get system release
    :returns: system release
  */
  struct utsname p_utsname;
  uname(&p_utsname);
  char *release = p_utsname.release;
  return release;
}
