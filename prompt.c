#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "prompt.h"

static void green(void) {
  printf("\033[0;32m");
}
static void reset(void) {
  printf("\033[0m");
}
void show_prompt(void){
    green();
    char path[100000];
    getcwd(path, sizeof(path));
    printf ("%s",path);
    reset();
    printf ("$ ");
    fflush (stdout);
}


