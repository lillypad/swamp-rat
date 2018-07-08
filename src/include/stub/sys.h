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


#ifndef SYS_UNAME_DEFINED
#define SYS_UNAME_SYSNAME    0
#define SYS_UNAME_NODENAME   1
#define SYS_UNAME_RELEASE    2
#define SYS_UNAME_VERSION    3
#define SYS_UNAME_MACHINE    4
#define SYS_UNAME_DOMAINNAME 5
#define SYS_UNAME_DEFINED
#endif

struct utsname *sys_uname_create(){
  struct utsname *p_uname = malloc(sizeof(struct utsname));
  uname(p_uname);
  return p_uname;
}

char *sys_uname(int sys_uname_type){
  struct utsname *p_uname = sys_uname_create();
  switch(sys_uname_type){
  case SYS_UNAME_SYSNAME:
    return p_uname->sysname;
  case SYS_UNAME_NODENAME:
    return p_uname->nodename;
  case SYS_UNAME_RELEASE:
    return p_uname->release;
  case SYS_UNAME_VERSION:
    return p_uname->version;
  case SYS_UNAME_MACHINE:
    return p_uname->machine;
  }
  return NULL;
}
