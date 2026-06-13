#ifndef ARVOREB_H
#define ARVOREB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "estruturas.h"

#define ORDEM 4
#define MAX_NOS ORDEM-1
#define MIN_NOS ORDEM/2
#define MAX_PONTEIROS 4
#define TAM_CABECALHO 17
#define TAM_NO 53

/*
------------------------------------------------------------------------------------
Registro do cabecalho da Árvore B, implementando conforme especificação do trabalho
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
Struct de um nó da Árvore B, implementado conforme especificação do trabalho
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
-------------------------------------------------------------------------------------------------------------
Struct para fazer a inserção na Árvore B, útil para ver se precisou de split ou não e guardar os novos RRNs
-------------------------------------------------------------------------------------------------------------
*/

typedef struct {
    int chavePromovida;
    int PRpromovido;
    int novoRRN;
    bool houveSplit;
} ResultadoInsercao;

/*
------------------------------------------------------------------------------------
Declarando funções de inicialização da Árvore-B: Criar tabela e funções auxiliares 
------------------------------------------------------------------------------------
*/

void ArvoreCriar(FILE* arv, CabecalhoArvore* cab);     // Cria o arquivo de indices Arvore-B
void NoCriar(NoArvore* no, int tipoNo);                             // Cria um Nó da Árvore-B

/*
----------------------------------------------------------------------------------------------
Declarando funções de Saída e Entrada (I/O) para auxiliar na criação e manutenção da Árvore-B
----------------------------------------------------------------------------------------------
*/

void gravarCabecalhoArvore(FILE* arv, CabecalhoArvore* cab);   // Grava o conteudo da struct cabeçaho pro arquivo de indices
void lerCabecalhoArvore(FILE* arv, CabecalhoArvore* cab);      // Le do arquivo de indices para a struct cabeçalho

void gravarNoArvore(FILE* arv, NoArvore* no, int RRN);          // Grava o conteudo da struct No pro arquivo de indices
void lerNoArvore(FILE* arv, NoArvore* no, int RRN);             // Le do arquivo de indices para a struct No


/*
----------------------------------------------------------------------
Declarando funções básicas da Árvore-B: Busca, inserção e remoção
----------------------------------------------------------------------
*/

// Busca um registro na Árvore-B, retornando o RRN do nó onde a chave foi encontrada ou -1 se não encontrada
int ArvoreBuscar(FILE* arv, CabecalhoArvore* cab, int chave);   
void ArvoreInserir(FILE* arv, CabecalhoArvore* cab, int chave, int RRN);        // Insere um registro na Árvore-B
void ArvoreRemover(FILE* arv, CabecalhoArvore* cab, int chave);                 // Remove um registro da Árvore-B


#endif