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
    char *args[64];
    const char arg = args;
    char **prox = args;

    char *temp = strtok(comando, " ");

    while (temp != NULL)
    {
      *prox++ = temp;
      temp = strtok(NULL, " ");
    }
    *prox = NULL;
    
    //EXECUTA
    execvp(args[0], arg);
    printf("\n%s\n", args);
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

void separa_maior(char* linha_comando)
{
  char *p;

  p = strtok(linha_comando, ">");

  
  while (p)
  {
    p = malloc(sizeof(char)); //aloca posição de memoria pra fazer um vetor de comandos
    p = strtok(NULL, ">");

  }
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
  

/*              PIPES              */

void pipe_simples(char** comandos)
{
  int pipe_a[2], status;
  pipe(pipe_a);

  pid_t res;

  res = fork();
  

  if(res == 0)
  {
    /*    Filho A     */
    // comandos[0]
    printf("\nFILHO A\n");
    dup2(pipe_a[WRITE], STDOUT_FILENO);
    
    close(pipe_a[READ]);
    close(pipe_a[WRITE]);

    executa(comandos[0]);

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
      printf("\nFILHO B\n");
      dup2(pipe_a[READ], STDIN_FILENO);

      close(pipe_a[WRITE]);
      close(pipe_a[READ]);

      executa(comandos[1]);
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

