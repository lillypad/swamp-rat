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
#include "../defs.h"

bool net_start_server(int port){
  if (port < TCP_PORT_MIN || port > TCP_PORT_MAX){
    fprintf(stderr, "error: server port is invalid\n");
    return false;
  }
  int sock_fd;
  struct sockaddr_in server;
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
  return true;
}
