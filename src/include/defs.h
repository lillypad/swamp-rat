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

#ifndef TCP_PORT_MAX
#define TCP_PORT_MAX 65535
#endif

#ifndef TCP_PORT_MIN
#define TCP_PORT_MIN 1
#endif

#ifndef MAX_DOMAIN_LEN
#define MAX_DOMAIN_LEN 63
#endif

#ifndef MAX_USERNAME_LEN
#define MAX_USERNAME_LEN 32
#endif

#ifndef CONFIG_DEFINED
typedef struct{
  int xor_key;
  char host[MAX_DOMAIN_LEN];
  int host_port;
} config;
#define CONFIG_DEFINED
#endif

#ifndef PACKET_SHELL_REVERSE_TCP_DEFINED
typedef struct{
  config header;
  int shell_type;
  int shell_async;
} packet_shell_reverse_tcp;
#define PACKET_SHELL_REVERSE_TCP_DEFINED
#endif

#ifndef PACKET_CALLBACK_DEFINED
typedef struct{
  config header;
  char username[MAX_USERNAME_LEN];
  int command;
} packet_callback;
#define PACKET_CALLBACK_DEFINED
#endif
