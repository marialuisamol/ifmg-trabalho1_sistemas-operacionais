#ifndef UTIL_H
#define UTIL_H

/*void executar(char* comando);*/
void cria_fork(char *comando);
void executa(char *comando) ;
void separa_pipe(char* linha_comando);
void separa_maior(char* linha_comando);
void separa_menor(char* linha_comando);
char* getln();
void pipe_simples(char **comandos);
void inicializar();
void inserir(char* comando);
//void pipe_rec(char* comando);
void redirecionamento_maior(char** comandos);
void redirecionamento_menor(char** comandos);
#endif