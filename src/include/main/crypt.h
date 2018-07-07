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
