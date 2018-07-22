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
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include "../defs.h"

#ifndef SYS_INFO
#define SYS_MAX_SYSNAME_LEN 32
#define SYS_MAX_MACHINE_LEN 32
#define SYS_MAX_RELEASE_LEN 64
typedef struct{
  char ip[MAX_DOMAIN_LEN];
  char username[MAX_USERNAME_LEN];
  char nodename[MAX_DOMAIN_LEN];
  char sysname[SYS_MAX_SYSNAME_LEN];
  char release[SYS_MAX_RELEASE_LEN];
  char machine[SYS_MAX_MACHINE_LEN];
  int cpu_usage;
} sys_info_t;
#define SYS_INFO
#endif

struct sys_memory_t {
  char *memory;
  size_t size;
};

static size_t sys_curl_memory_callback(void *contents,
                                       size_t size,
                                       size_t nmemb,
                                       void *userp){
  /*
    :TODO: write response body data
    :contents:
    :size:
    :nmemb:
    :uerp:
    :returns: real size
  */
  size_t realsize = size * nmemb;
  struct sys_memory_t *mem = (struct sys_memory_t *)userp;
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    fprintf(stderr, "[x] not enough memory (realloc returned NULL)\n");
    return 0;
  }
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}

bool sys_public_ip(char *public_ip, size_t public_ip_size){
  /*
    :TODO: get public ip address
    :public_ip: pointer to store public ip address
    :public_ip_size: max size of ip address
    :returns: boolean
  */
  CURL *curl_handle;
  CURLcode response_code;
  struct sys_memory_t response_body;
  char user_agent[] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36";

  // setup response body
  response_body.memory = malloc(1);
  response_body.size = 0;

  // curl init
  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();

  // set curl options
  curl_easy_setopt(curl_handle, CURLOPT_URL, "https://icanhazip.com/");
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, sys_curl_memory_callback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&response_body);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, user_agent);

  // perform request
  response_code = curl_easy_perform(curl_handle);

  // check http response code
  if (response_code != CURLE_OK){
    fprintf(stderr, "[x] failed to get public ip address with status %d\n", response_code);
    return false;
  } else{
    if (strlen(response_body.memory) < public_ip_size){
      strncpy(public_ip, response_body.memory, strlen(response_body.memory)-1);
    } else{
      fprintf(stderr, "[x] response body size greater than public ip size\n");
      return false;
    }
  }
  // cleanup
  curl_easy_cleanup(curl_handle);
  free(response_body.memory);
  curl_global_cleanup();
  return true;
}

bool sys_host2ip(char *host, char* ip, size_t ip_size){
  /*
    :TODO: host to ip
    :host: host domain
    :ip: pointer to ip
    :returns: ip address
  */
  struct hostent *he;
  struct in_addr **addr_list;
  int i;
  if ((he = gethostbyname(host)) == NULL){
    herror("gethostbyname");
    return false;
  }
  addr_list = (struct in_addr **) he->h_addr_list;
  for(i = 0; addr_list[i] != NULL; i++){
    strncpy(ip , inet_ntoa(*addr_list[i]), ip_size);
    return true;
  }
  return false;
}

char *sys_username(){
  /*
    :TODO: get current username
    :username: pointer to store username
    :returns: boolean
  */
  char *username = malloc(MAX_USERNAME_LEN);
  getlogin_r(username, MAX_USERNAME_LEN);
  return username;
}


#ifndef SYS_UNAME_DEFINED
#define SYS_UNAME_SYSNAME    0
#define SYS_UNAME_NODENAME   1
#define SYS_UNAME_RELEASE    2
#define SYS_UNAME_VERSION    3
#define SYS_UNAME_MACHINE    4
#define SYS_UNAME_DEFINED
#endif

#ifndef SYS_SYSINFO
typedef struct{
  char ip[MAX_DOMAIN_LEN];
  char username[MAX_USERNAME_LEN];
  char architechure[25];
  float load_average;
} sys_sysinfo_t;
#define SYS_SYSINFO
#endif

struct utsname *sys_uname_create(){
  /*
    :TODO: get system username;
    ;returns: (char *) sytem username
  */
  struct utsname *p_uname = malloc(sizeof(struct utsname));
  uname(p_uname);
  return p_uname;
}

char *sys_uname(int sys_uname_type){
  /*
    :TODO: get uname data;
    :sys_uname_type: (int) preprocessor defined uname options
    :returns: (char *) uname data;
  */
  struct utsname *p_uname = sys_uname_create();
  switch(sys_uname_type){
  case SYS_UNAME_SYSNAME:
    return p_uname->sysname;
  case SYS_UNAME_NODENAME:
    return p_uname->nodename;
  case SYS_UNAME_RELEASE:
    return p_uname->release;
  case SYS_UNAME_VERSION:
    return p_uname->version;
  case SYS_UNAME_MACHINE:
    return p_uname->machine;
  }
  return NULL;
}

int sys_load_average(){
  /*
    :TODO: get system load average
    :returns: (int) load average
  */
  char loadavg[1024];
  float load_average;
  int loadavg_fd = open("/proc/loadavg", O_RDONLY);
  if (loadavg_fd < 0){
    fprintf(stderr, "error: error reading load average\n");
    return -1;
  } else{
    read(loadavg_fd, loadavg, sizeof(loadavg) - 1);
    sscanf(loadavg, "%f", &load_average);
    close(loadavg_fd);
    return (int)(load_average * 100);
    return load_average;
  }
}


