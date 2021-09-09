#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "prompt.h"

static void green(void) {
  printf("\033[0;32m");
}
static void blue(void) {
  printf("\033[0;34m");
}
static void reset(void) {
  printf("\033[0m");
}
void show_prompt(void){
    green();
    char path[1000],*usr;
    usr = getlogin();
    getcwd(path, sizeof(path));
    printf ("%s",usr);
    reset();
    printf(":");
    blue();
    printf("%s", path);
    reset();
    printf ("$ ");
    fflush (stdout);
}


