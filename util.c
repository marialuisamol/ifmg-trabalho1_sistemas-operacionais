#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define READ 1
#define WRITE 0

#define CHUNK sizeof(char)

void executa(char *comando) 
{
    //SPLIT NO ESPAÇO
    char *arg[64];
    int const args = *arg;
    char **prox = arg;

    char *temp = strtok(comando, " ");

    while (temp != NULL)
    {
      *prox++ = temp;
      temp = strtok(NULL, " ");
    }
    *prox = NULL;
    
    //EXECUTA
    execvp(arg[0], args);
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
    int link_a[2], status_a, status_b;
    pipe(link_a);

    pid_t res_a = fork();
    pid_t res_b = fork();


    if (res_a == 0)
    {
      /* Filho A*/

      dup2(link_a[WRITE], STDOUT_FILENO);
      close(link_a[WRITE]);
      close(link_a[READ]);

      executa(p);
    }
    

    p = strtok(NULL, "|");

    if (res_b == 0)
    {
        /* Filho B*/

      dup2(link_a[READ], STDIN_FILENO);      
      close(link_a[READ]);
      close(link_a[WRITE]);
      executa(p);

    }

    else if (res_a > 0)
    {
      /* Pai */
      waitpid(res_a, status_a, NULL);
      waitpid(res_b, status_b, NULL);
    }

    else if (res_a < 0)
    {
      /* Falha em criar o filho A */
    }

    else if (res_b < 0)
    {
      /* Falha em criar o filho A */
    } 
  }
}

// /*        REDIRECIONADORES        */
// void redirecionamento_maior(char* saida)
// {
//   pid_t res = fork();

//   if (res == 0)
//   {
//     /*Filho*/
//     FILE* fd = fopen(saida, "rw");
//     dup2(fd, STDOUT_FILENO);
//   }
//   else if(res > 0)
//   {
//     /*Pai*/
//     wait(NULL);
//   }
  
// }

// void redirecionamento_menor(char* entrada)
// {
//   pid_t res = fork();

//   if (res == 0)
//   {
//     /*Filho*/
//     FILE* fd = fopen(entrada, "r");
//     dup2(fd, STDIN_FILENO);
//   }
//    else if(res > 0)
//   {
//     /*Pai*/
//     wait(NULL);
//   }
  
  
// }
  


/*
void pipe_rec(char* comando)
{
  int *link[2] = malloc(2 * sizeof(int));

  pipe(link[0]);
  pipe(link[1]);

  pid_t *filhos = malloc(2 * sizeof(pid_t));

  filhos[0] = fork(); //filho a
  filhos[1] = fork();//filho b

  if (filhos[0] == 0)
  {
    /* filho A echo abc | grep a
    dup2(link[0][WRITE], STDOUT_FILENO);
    close(link[0][WRITE]);
    close(link[0][READ]);

    executa(comando);

  }

  if (filhos[1] == 0)
  {
    /* filho B 
    dup2(link[0][READ], STDIN_FILENO);//lendo de a
    close(link[0][WRITE]);
    close(link[0][READ]);

    dup2(link[1][WRITE], STDOUT_FILENO);//escrevendo em b
    close(link[1][WRITE]);
    close(link[1][READ]);

    executa(comando);

  }

}
*/
