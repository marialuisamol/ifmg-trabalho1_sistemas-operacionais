#ifndef UTIL_H
#define UTIL_H

/*void executar(char* comando);*/
void cria_fork(char *comando);
void executa(char *comando) ;
void separa_pipe(char* comando);
char* getln();
void redirecionamento_maior(char* saida);
void redirecionamento_menor(char* entrada);
#endif