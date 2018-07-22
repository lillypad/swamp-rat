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

extern unsigned char* _start;
extern unsigned char* __etext;

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
    fprintf(stderr, "[x] failed to read kernel module list");
    return false;
  }
  char buff[RE_KERNEL_MODULE_NAME_MAX_SIZE];
  memset(buff, 0, sizeof(buff));
  fread(buff, 1, strlen(kernel_module), fd);
  if (strncmp(buff, kernel_module, strlen(kernel_module)) == 0){
    return true;
  } else{
    return false;
  }
}

bool re_kernel_modules(){
  /*
    :TODO: check for kernel modules
    :returns: boolean
  */
  if (re_kernel_module("virtio") == true){
    return true;
  } else if (re_kernel_module("vboxvideo") == true){
    return true;
  } else if (re_kernel_module("vboxguest") == true){
    return true;
  } else if (re_kernel_module("vboxsf") == true){
    return true;
  } else{
    return false;
  }
}

bool re_hypervisor(){
  /*
    :TODO: detect hypervisor
    :returns: boolean
  */
  char hypervisor[] = "hypervisor";
  char command[] = "grep -m 1 -Po 'hypervisor' /proc/cpuinfo";
  char buff[RE_KERNEL_MODULE_NAME_MAX_SIZE];
  FILE *fd = popen(command, "r");
  if (fd == NULL){
    fprintf(stderr, "[x] failed to read cpuinfo");
    return false;
  }
  memset(buff, 0, sizeof(buff));
  fread(buff, 1, strlen(hypervisor), fd);
  if (strncmp(buff, hypervisor, strlen(hypervisor)) == 0){
    return true;
  } else{
    return false;
  }
}

int re_breakpoints() {                                     
  int count = 0;                                            
  char* start = (char*)&_start;                             
  char* end = (char*)&__etext;                              
  volatile unsigned char bppart[1] = { 0x66 };                  
  while(start != end) {                                     
    if(((*(volatile unsigned*)start) & 0xFF) == ((*bppart) + (*bppart))) {     
      ++count;                                          
    }                                                     
    ++start;                                              
  }
  return count;                                             
}
