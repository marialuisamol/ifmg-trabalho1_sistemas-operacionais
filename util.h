#ifndef UTIL_H
#define UTIL_H

/*void executar(char* comando);*/
void cria_fork(char *comando);
void executa(char *comando) ;
void separa_pipe(char* linha_comando);
void separa_maior(char* linha_comando);
void separa_menor(char* linha_comando);
char* getln();
void inicializar();
void inserir(char* comando);
void pipe_complexo(char **comandos);
void redirecionamento_maior(char** comandos, int* fd1);
void redirecionamento_menor(char** comandos, int* fd1);
void verifica_redirecionadores(char* linha_comando);
#endif