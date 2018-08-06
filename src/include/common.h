#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *itoa(int x){
  /*
    :TODO: integer to (char *)
    :x: integer
    :returns: (char *)
  */
  char* buffer = malloc(sizeof(char) * sizeof(int) * 4 + 1);
  if (buffer){
    sprintf(buffer, "%d", x);
  }
  return buffer;
}
