#ifndef ARVOREB_H
#define ARVOREB_H

#include <stdio.h>
#include "estruturas.h"

#define MAX_NOS 3
#define MAX_PONTEIROS 4

/*
------------------------------------------------------------------------------------
Registro do cabecalho da arvore B, implementando conforme especificação do trabalho
------------------------------------------------------------------------------------
*/

typedef struct {
    char status;    // 1 byte
    int noRaiz;     // 4 bytes
    int topo;       // 4 bytes
    int proxRRN;    // 4 bytes
    int nroNos;     // 4 bytes
} CabecalhoArvore;

/*
--------------------------------------------------------------------------------------
Struct de um nó da arvore B, implementado conforme especificação do trabalho
--------------------------------------------------------------------------------------
*/

typedef struct {
    char removido;  // 1 byte
    int proximo;    // 4 bytes
    int tipoNo;     // 4 bytes
    int nroChaves;  // 4 bytes
    int Chaves[3];  // 3*4 bytes
    int Pr[3];      // 3*3 bytes
    int P[4];       // 4*3 bytes
} NoArvore;

/*
------------------------------------------------------------------------------------
Declarando funções de inicialização da Árvore-B: Criar tabela e funções auxiliares 
------------------------------------------------------------------------------------
*/

void ArvoreCriar();     // Cria o arquivo de indices Arvore-B
void NoCriar();         // Cria um Nó da Árvore-B

/*
----------------------------------------------------------------------------------------------
Declarando funções de Saída e Entrada (I/O) para auxiliar na criação e manutenção da Árvore-B
----------------------------------------------------------------------------------------------
*/

void lerCabecalhoArvore();      // Le do arquivo de indices para a struct cabeçalho
void gravarCabecalhoArvore();   // Grava o conteudo da struct cabeçaho pro arquivo de indices

void lerNoArvore();             // Le do arquivo de indices para a struct No
void gravarNoArvore();          // Grava o conteudo da struct No pro arquivo de indices


/*
----------------------------------------------------------------------
Declarando funções básicas da Árvore-B: Busca, inserção e remoção
----------------------------------------------------------------------
*/

int ArvoreBuscar(FILE* arv, int RRN, int chave, int foundRRN, int foundPos);


#endif