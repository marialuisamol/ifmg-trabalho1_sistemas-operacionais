#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define READ 0
#define WRITE 1

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
  int status, fd1[2];
  pipe(fd1);
  res = fork();

  if (res == 0)
  {
    //Filho
    if (strstr(comando, ">") != NULL)
    {
      separa_maior(comando, fd1);
    }
    else if (strstr(comando, "<") != NULL)
    {
      separa_menor(comando);
    }
    else
    {
      executa(comando);
    }
    
  }
  else if (res > 0)
  {
    //Pai
    waitpid(res, status, 0);
  }
   
}

int ultimo = 0, tamanho = 10;
char** comandosAExecutar;

void inicializar() {
  comandosAExecutar = malloc(tamanho * sizeof(char*));
}

void inserir(char* comando) {
  if (ultimo >= tamanho)
  {
    tamanho+= 10;
    comandosAExecutar = realloc (comandosAExecutar, tamanho * sizeof(char*));
  }
  
  comandosAExecutar[ultimo++] = comando;
}

void separa_pipe(char* linha_comando)
{
  char *p;
  int i = 0;

  p = strtok(linha_comando, "|");
  
  while (p)
  {
    inserir(p);    
    p = strtok(NULL, "|");
    
  }

  pipe_simples(comandosAExecutar);
}

void separa_maior(char* linha_comando, int* fd1)
{
  char *p, **comandos = malloc(2 * sizeof(char*));
  p = strtok(linha_comando, ">");
  
  comandos[0] = p;
  p = strtok(NULL, ">");
  comandos[1] = p;
  printf("0: %s\t 1: %s\n", comandos[0],comandos[1]);

  redirecionamento_maior(comandos, fd1);
}

void separa_menor(char* linha_comando)
{
  char *p;

  p = strtok(linha_comando, "<");

  
  while (p)
  {
    p = malloc(sizeof(char)); //aloca posição de memoria pra fazer um vetor de comandos
    p = strtok(NULL, "<");

  }
}

/*        REDIRECIONADORES        */

void redirecionamento_maior(char** comandos, int* fd1)
{
  /*Filho*/
  FILE* fd = fopen(comandos[1], "w");
  
  dup2(fd1[READ], STDIN_FILENO);

  close(fd1[WRITE]);
  close(fd1[READ]);

  dup2(fd, STDOUT_FILENO);

  executa(comandos[0]);
  
  close(fd);
}

void redirecionamento_menor(char** comandos)
{
  /*Filho*/
  FILE* fd = fopen(comandos[1], "r");
  dup2(fd, STDIN_FILENO);

  executa(comandos[0]);

}
  

/*              PIPES              */

void pipe_simples(char** comandos)
{
  int pipe_a[2], status, fd1[2];
  pipe(pipe_a);
  pipe(fd1);
  pid_t res;
  res = fork();
  

  if(res == 0)
  {
    /*    Filho A     */
    // comandos[0]
    
    dup2(pipe_a[WRITE], STDOUT_FILENO); // EScreve a saída
    
    close(pipe_a[READ]);
    close(pipe_a[WRITE]);

    if (strstr(comandos[0], ">") != NULL)
    {
      separa_maior(comandos[0], fd1);
    }
    else if (strstr(comandos[0], "<") != NULL)
    {
      separa_menor(comandos[0]);
    }
    else
    {
      executa(comandos[0]);
    }

    printf("\n$ Falha ao executar %s", comandos[0]);
    exit(1);
  }
  
  else if (res > 0)
  {
    //PAI
    res = fork();
    if (res == 0)
    {
      /* Filho B */
      //comandos[1]

      dup2(pipe_a[READ], STDIN_FILENO); // lê a entrada

      close(pipe_a[WRITE]);
      close(pipe_a[READ]);
      if (strstr(comandos[1], ">") != NULL)
      {
        separa_maior(comandos[1], fd1);
      }
      else if (strstr(comandos[1], "<") != NULL)
      {
        separa_menor(comandos[1]);
      }
      else
      {
        executa(comandos[1]);
      }
        printf("\n$ Falha ao executar %s", comandos[1]);
      exit(1);
    }

    else
    {
      close(pipe_a[READ]);
      close(pipe_a[WRITE]);

      waitpid(res, &status, 0);
    }
    
  }
  else
  {
    return;
  }
}

