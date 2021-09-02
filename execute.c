#include <sys/syscall.h>
#include "command.h"
#include "execute.h"
#include "builtin.h"

void execute_pipeline(pipeline apipe){
    if(builtin_is_internal(apipe)){
        builtin_exec(apipe);
    }
}
