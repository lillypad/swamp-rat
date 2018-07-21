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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include "../defs.h"

int hostname_to_ip(char *host, char* ip){
  struct hostent *he;
  struct in_addr **addr_list;
  int i;
  if ((he = gethostbyname(host)) == NULL){
    herror("gethostbyname");
    return 1;
  }
  addr_list = (struct in_addr **) he->h_addr_list;
  for(i = 0; addr_list[i] != NULL; i++){
    strcpy(ip , inet_ntoa(*addr_list[i]));
    return 0;
  }
  return 1;
}

bool sys_public_ip(char *public_ip){
  int sock_fd, port;
  struct sockaddr_in server;
  char ip[MAX_DOMAIN_LEN];
  char *domain;
  char *request;

  port = 80;
  domain = "icanhazip.com";
  hostname_to_ip(domain, ip);
  request = "GET / HTTP/1.1\r\nHost: icanhazip.com\r\n\r\n";

  memset(&server, 0, sizeof(server));
  server.sin_family      = AF_INET;
  server.sin_addr.s_addr = inet_addr(ip);
  server.sin_port        = htons(port);

  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    fprintf(stderr, "[x] failed to create socket\n");
    return false;
  } else{
    printf("[+] created socket\n");
  }

  if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0){
    fprintf(stderr, "[x] failed to connect to icanhazip.com\n");
    return false;
  } else{
    printf("[+] connected to icanhazip.com\n");
  }

  if (recv(sock_fd, public_ip, MAX_DOMAIN_LEN, 0) < 0){
    fprintf(stderr, "[x] failed to receive public ip data");
    return false;
  } else{
    printf("[+] received public ip data\n");
  }

  printf("Public IP is: %s", public_ip);

  close(sock_fd);
  
  return true;
}

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
#define SYS_UNAME_DEFINED
#endif

#ifndef SYS_SYSINFO
typedef struct{
  char ip[MAX_DOMAIN_LEN];
  char username[MAX_USERNAME_LEN];
  char architechure[25];
  float load_average;
} sys_sysinfo_t;
#define SYS_SYSINFO
#endif

struct utsname *sys_uname_create(){
  /*
    :TODO: get system username;
    ;returns: (char *) sytem username
  */
  struct utsname *p_uname = malloc(sizeof(struct utsname));
  uname(p_uname);
  return p_uname;
}

char *sys_uname(int sys_uname_type){
  /*
    :TODO: get uname data;
    :sys_uname_type: (int) preprocessor defined uname options
    :returns: (char *) uname data;
  */
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

int sys_load_average(){
  /*
    :TODO: get system load average
    :returns: (int) load average
  */
  char loadavg[1024];
  float load_average;
  int loadavg_fd = open("/proc/loadavg", O_RDONLY);
  if (loadavg_fd < 0){
    fprintf(stderr, "error: error reading load average\n");
    return -1;
  } else{
    read(loadavg_fd, loadavg, sizeof(loadavg) - 1);
    sscanf(loadavg, "%f", &load_average);
    close(loadavg_fd);
    return (int)(load_average * 100);
    return load_average;
  }
}


