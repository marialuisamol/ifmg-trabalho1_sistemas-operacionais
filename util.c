#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


#define CHUNK sizeof(char)

void executa(char *comando) 
{
    //SPLIT NO ESPAÇO
    char *args[64];
    char **prox = args;

    char *temp = strtok(comando, " ");

    while (temp != NULL)
    {
      *prox++ = temp;
      //printf("%s,\n", temp); //argumentos
      temp = strtok(NULL, " ");
    }
    *prox = NULL;
    
    //EXECUTA
    execvp(args[0], args);
}

char *getln()
{
    char *line = NULL, *tmp = NULL;
    size_t size = 0, index = 0;
    int ch = EOF;

    while (ch) {
        ch = getc(stdin);

        /* Check if we need to stop. */
        if (ch == EOF || ch == '\n')
            ch = 0;

        /* Check if we need to expand. */
        if (size <= index) {
            size += CHUNK;
            tmp = realloc(line, size);
            if (!tmp) {
                free(line);
                line = NULL;
                break;
            }
            line = tmp;
        }

        /* Actually store the thing. */
        line[index++] = ch;
    }

    return line;
}

void cria_fork(char *comando)
{

  pid_t res;

  res = fork();

  if (res == 0)
  {
    //Filho 
    
    executa(comando);
    
  }
  else if (res > 0)
  {
    //Pai
    wait(NULL);
  }
   
}

void separa_pipe(char* comando)
{
  char *p;

  p = strtok(comando, "|");

  
  while (p)
  {
    //printf("%s\n", p);
    cria_fork(p);
    p = strtok(NULL, "|");
      
  }
  
}

/*        REDIRECIONADORES        */
void redirecionamento_maior(char* saida)
{
  pid_t res = fork();

  if (res == 0)
  {
    /*Filho*/
    FILE* fd = fopen(saida, "rw");
    dup2(fd, STDOUT_FILENO);
  }
  else if(res > 0)
  {
    /*Pai*/
    wait(NULL);
  }
  
}

void redirecionamento_menor(char* entrada)
{
  pid_t res = fork();

  if (res == 0)
  {
    /*Filho*/
    FILE* fd = fopen(entrada, "r");
    dup2(fd, STDIN_FILENO);
  }
   else if(res > 0)
  {
    /*Pai*/
    wait(NULL);
  }
  
  
}
  

/*              PIPES           */
#define READ 1
#define WRITE 0

void pipe_simples(char *comando)//só um pipe
{
  int link_a[2], link_b[2];
  pid_t res_a = fork();
  pid_t res_b = fork();

  pipe(link_a);

  if (res_a == 0)
  {
    /* Filho A*/

    dup2(link_a[WRITE], STDOUT_FILENO);
    close(link_a[WRITE]);
    close(link_a[READ]);

    executa(comando);

  }

  
  if (res_b == 0)
  {
    /* Filho B*/

    dup2(link_b[READ], STDIN_FILENO);
    close(link_b[WRITE]);
    close(link_b[READ]);

    executa(comando);


  }

  else if (res_a > 0 && res_b > 0)
  {
    /* Pai */
    waitpid(res_a, NULL, NULL);
    waitpid(res_b, NULL, NULL);
  }
  
}

void pipe_rec(char* comando)
{
  pid_t *filhos = malloc(2 * sizeof(pid_t));

  filhos[0] = fork(); //filho a
  filhos[1] = fork();//filho b

  int *link[2] = malloc(2 * sizeof(int));

  pipe(link[0]);
  pipe(link[1]);

  if (filhos[0] == 0)
  {
    /* filho A echo abc | grep a*/
    dup2(link[0][WRITE], STDOUT_FILENO);
    close(link[0][WRITE]);
    close(link[0][READ]);

    executa(comando);

  }

  if (filhos[1] == 0)
  {
    /* filho B */
    dup2(link[0][READ], STDIN_FILENO);//lendo de a
    close(link[0][WRITE]);
    close(link[0][READ]);

    dup2(link[1][WRITE], STDOUT_FILENO);//escrevendo em b
    close(link[1][WRITE]);
    close(link[1][READ]);

    executa(comando);

  }


  

}
