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
      separa_menor(comando, fd1);
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
    close;
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

  pipe_complexo(comandosAExecutar);
  
}

void separa_maior(char* linha_comando, int* fd1)
{ 
  char *p, **comandos = malloc(2 * sizeof(char*));
  p = strtok(linha_comando, ">");
  
  comandos[0] = p;
  p = strtok(NULL, ">");

  if (strstr(comandos, "<") != NULL)
  {
    separa_menor(comandos, fd1);
  }

  comandos[1] = p;
  //printf("0: %s\t 1: %s\n", comandos[0],comandos[1]);

  redirecionamento_maior(comandos, fd1);
}

void separa_menor(char* linha_comando, int* fd1)
{
  char *p, **comandos = malloc(2 * sizeof(char*));

  p = strtok(linha_comando, "<");

  comandos[0] = p;
  p = strtok(NULL, "<");

  if (strstr(comandos, ">") != NULL)
  {
    separa_maior(comandos, fd1);
  }

  comandos[1] = p;
  //printf("0: %s\t 1: %s\n", comandos[0],comandos[1]);

  redirecionamento_menor(comandos, fd1);
}

/*        REDIRECIONADORES        */

void redirecionamento_maior(char** comandos, int* fd1)
{
  /*Filho*/
  FILE* file = fopen(comandos[1], "w");
  int fd = fileno(file);
  
  dup2(fd1[READ], STDIN_FILENO);
  dup2(fd, STDOUT_FILENO);

  close(fd1[WRITE]);
  close(fd1[READ]);
  fclose(file);

  executa(comandos[0]);
}

void redirecionamento_menor(char** comandos, int* fd1)
{
  /*Filho*/
  FILE* file = fopen(comandos[1], "r");
  int fd = fileno(file);

  dup2(fd1[READ], STDIN_FILENO);
  dup2(fd, STDIN_FILENO);
  
  close(fd1[WRITE]);
  close(fd1[READ]);
  fclose(file);

  executa(comandos[0]);
}
  

/*              PIPES              */

void pipe_complexo(char **comandos) {
  
  int pipe_a[ultimo - 1][2], status, fd1[2];

  for(int i = 0; i < ultimo - 1; i++){
    //cria a quantidade de pipes necessarios
    if (pipe(pipe_a[i]) == -1) {//realiza a criação do pipe
        perror("Falha na realização do pipe");
        exit(1);
    }
  }

    // executar os comandos abaixo
    for (int i = 0; i < ultimo; i++) {
      pid_t res = fork();
      
      if (res < 0)
      {
        /* ERRO*/
        fprintf(stderr, "Falha na realizacao do fork()\n");
      }
      
      else if (res == 0) { //FILHO

      if (i == ultimo - 1)
      {
        /* Último comando */
        dup2(pipe_a[i - 1][READ], STDIN_FILENO);
      }
      
      else if(i == 0){
        /* Primeiro comando*/
        dup2(pipe_a[i][WRITE], STDOUT_FILENO);
      }     
      
      else if (i > 0 && i < ultimo - 1)
      {
        /* Comandos do meio */
        dup2(pipe_a[i - 1][READ], STDIN_FILENO);
        dup2(pipe_a[i][WRITE], STDOUT_FILENO);
      }
      

      // executa comando...
      if (strstr(comandos[i], ">") != NULL)
      {
        separa_maior(comandos[i], fd1);
      }
      else if (strstr(comandos[i], "<") != NULL)
      {
        separa_menor(comandos[i], fd1);
      }
      else
      {
        executa(comandos[i]);
      }
    
      } 
      else if (res > 0) { // pai
        if(i < ultimo - 1){
          close(pipe_a[i][WRITE]);
        }
        waitpid(res, &status, 0);
    }
  }
}