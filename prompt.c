#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "prompt.h"

void show_prompt(void){
    char path[100000];
    getcwd(path, sizeof(path));
    printf ("%s> ",path);
    fflush (stdout);
}


