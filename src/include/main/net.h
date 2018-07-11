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

#ifndef NET_START_SERVER_ARGS
typedef struct{
  int port;
} net_start_server_args_t;
#define NET_START_SERVER_ARGS
#endif

bool net_start_server(int port){
  /*
    :TODO: start listening server
    :port: (int) server listening port
    :returns: (bool)
  */
  if (port < TCP_PORT_MIN || port > TCP_PORT_MAX){
    fprintf(stderr, "error: server port is invalid\n");
    return false;
  }
  int server_fd, client_fd, err;
  struct sockaddr_in server, client;
  char data[NET_PACKET_MAX_SIZE];

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0){
    fprintf(stderr, "error: server error creating socket\n");
    return false;
  }
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  int opt_val = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);
  err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
  if (err < 0){
    fprintf(stderr, "error: bind to socket failed\n");
    return false;
  }
  err = listen(server_fd, 128);
  if (err < 0){
    fprintf(stderr, "error: listen to socket failed\n");
    return false;
  }
  while (true){
    socklen_t client_len = sizeof(client);
    client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);

    if (client_fd < 0){
      fprintf(stderr, "error: failed to connect to client\n");
      return false;
    }

    while (true){
      int read = recv(client_fd, data, NET_PACKET_MAX_SIZE, 0);
      if (!read){
        break;
      }
      if (read < 0){
        fprintf(stderr, "error: client read failed\n");
        return false;
      }
      err = send(client_fd, data, read, 0);
      if (err < 0){
        fprintf(stderr, "error: client write failed\n");
        return false;
      }
    }
  }
}
