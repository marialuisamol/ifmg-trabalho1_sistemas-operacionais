Autor: Maria Luísa Mendonça Oliveira
Data: 31/05/2023

A partir do enunciado do trabalho comecei com os casos mais simples,
por exemplo comandos que não possuem operadores |, < ou >.
Fazendo fork para resolver os comandos e os quebrando nos espaços e 
realizando exec.
Após esse passo, tentei implementar uma função para realizar comandos 
com apenas um pipe. Tendo sucesso nessa etapa, achei que seria possível 
fazer uma função apenas para quantos pipes o comando tivesse. Tive mais 
dificuldades nessa parte, portanto foi a mais demorada. A ideia é que a 
linha de comando digitada pelo usuario fosse tranformada em um vetor de 
comandos separados pelo pipe, exemplo linha de comando digitada: 
ls -l | grep util | wc -l -> [ls -l, grep util, wc -l]. 
Após transformado em vetor são criados todos os pipes necessários, 
então, entramos em um loop que verifica qual é a posição e faz as 
devidas verificações, posteriormente dup2, closes e exec's.
A implementação dos redirecionadores foi feita seguindo as indicações do 
professor. Onde quebramos (trim) no operador realizamos e dup2.
É importante ressaltar que ao criar um arquivo o programa cria da 
seguinte forma, se o nome digitado for texto.txt o arquivo tem como 
resultado o nome ' texto.txt'.
Além disso, o programa não funciona como deveria com os dois operadores < 
e > numa mesma linha de comando.