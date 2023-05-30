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
  //pipe_simples(comandosAExecutar);
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

void separa_menor(char* linha_comando, int* fd1)
{
  char *p, **comandos = malloc(2 * sizeof(char*));

  p = strtok(linha_comando, "<");

  comandos[0] = p;
  p = strtok(NULL, "<");
  comandos[1] = p;
  printf("0: %s\t 1: %s\n", comandos[0],comandos[1]);

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
  int pipe_a[ultimo-1][2], status, fd1[2];

  for(int j = 0; j < ultimo - 1; j++){
    pipe(pipe_a[j]);
  }

  pipe(fd1);
  pid_t res[ultimo-1];

  //cria forks
  for(int j = 0; j < ultimo - 1; j++){
    res[j] = fork();
  }

  if(res[0] == 0){
    //FILHO    
    dup2(pipe_a[0][WRITE], STDOUT_FILENO);
    close(pipe_a[0][READ]);
    close(pipe_a[0][WRITE]);

    if (strstr(comandos[0], ">") != NULL)
    {
      separa_maior(comandos[0], fd1);
    }
    else if (strstr(comandos[0], "<") != NULL)
    {
      separa_menor(comandos[0], fd1);
    }
    else
    {
      executa(comandos[0]);
    }

    printf("\n$ Falha ao executar %s", comandos[0]);
    exit(1);
  }
  
  for(int i = 1; i < ultimo-1; i++){
    if(res[i] == 0){
      //FILHO[1]
      dup2(pipe_a[i - 1][READ], STDIN_FILENO); 
      close(pipe_a[i - 1][READ]);
      close(pipe_a[i - 1][WRITE]);
    }
    else if(res[i+1] == 0){
      dup2(pipe_a[i][WRITE], STDOUT_FILENO); 
      close(pipe_a[i][READ]);
      close(pipe_a[i][WRITE]);
    }

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

    printf("\n$ Falha ao executar %s", comandos[i]);
    exit(1);
  }
  
  //ULTIMO COMANDO
   if(res[ultimo - 1] == 0){
    //FILHO    
    dup2(pipe_a[ultimo-2][READ], STDIN_FILENO);
    close(pipe_a[ultimo-2][READ]);
    close(pipe_a[ultimo-2][WRITE]);

    if (strstr(comandos[ultimo-1], ">") != NULL)
    {
      separa_maior(comandos[ultimo-1], fd1);
    }
    else if (strstr(comandos[ultimo-1], "<") != NULL)
    {
      separa_menor(comandos[ultimo-1], fd1);
    }
    else
    {
      executa(comandos[ultimo-1]);
    }

    printf("\n$ Falha ao executar %s", comandos[ultimo-1]);
    exit(1);
  }

  else if (res[ultimo-1] > 0){
    waitpid(res[ultimo-1], &status, 0);
  }
}