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
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#ifndef NET_CLIENT_SLEEP
#define NET_CLIENT_SLEEP 5
#endif

#ifndef NET_CLIENT_RESPONSE_MAX
#define NET_CLIENT_RESPONSE_MAX 1024
#endif

bool net_client(char *host, int port){
  char response[NET_CLIENT_RESPONSE_MAX];
  struct sockaddr_in server;

  memset(&server, 0, sizeof(server));
  server.sin_family      = AF_INET;
  server.sin_addr.s_addr = inet_addr(host);
  server.sin_port        = htons(port);

  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0){
    fprintf(stderr, "[x] failed to create socket\n");
    return false;
  }

  while(true){
    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0){
      fprintf(stderr, "[x] failed to connect to server\n");
    } else{
      printf("[+] connected to server\n");
      break;
    }
    sleep(NET_CLIENT_SLEEP);
  }

  char data[] = "hello";

  while(true){
    if (send(sock_fd, data, sizeof(data), 0) < 0){
      fprintf(stderr, "[x] failed to send data to server\n");
      sleep(NET_CLIENT_SLEEP);
      continue;
    }
    if (recv(sock_fd, response, sizeof(response), 0) < 0){
      fprintf(stderr, "[x] failed to receive server data\n");
      sleep(NET_CLIENT_SLEEP);
      continue;
    } else{
      printf("%s\n", response);
    }
    sleep(NET_CLIENT_SLEEP);
  }
  close(sock_fd);
  return true;
}
