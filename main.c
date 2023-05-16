#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include <string.h>

int main(int argc, char const *argv[])
{
    char *linha_cmd;
    do
    {
        printf("$");
        linha_cmd = getln();
        inicializar();
        if (strstr(linha_cmd, "|") != NULL)
        {
            separa_pipe(linha_cmd);   
        }
        else
        {
            cria_fork(linha_cmd);
        }
        
        

    } while (strcmp(linha_cmd, "exit") != 0);
    
    return 0;
}
