#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdbool.h>

// registro de cabeçalho (início do arquivo .bin) 17 bytes
typedef struct {
    char status;            // 0 -> inconsistente 1 -> consistente
    int topo;               // Byte offset do primeiro registro removido (Pilha de removidos)
    int proxRRN;            // valor do próximo RRN disponível (iniciado com 0)
    int nroEstacoes;        // qntd de estacoes diferentes  DEV NOTE: usar estrutura SET para armazenar entidades únicas e depois só contar
    int nroParesEstacao;    // qntd de pares (codEstacao e codProxEstacao) distintos já armazenados 
} Cabecalho;

// registro de dados (tam variavel), serve para mostrar como vai ser o nome e a composição de todos os campos de cada registro de dados
typedef struct {
    char removido;        
    int proximo;   
    int codEstacao;         // n aceita nulo
    int codLinha; 
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    int tamNomeEstacao;
    char* nomeEstacao;     // n aceita nulo
    int tanNomeLinha;
    char* nomeLinha;
} Registro;

/// @brief serve como se fosse uma lista do que se está buscando quando uma busca for realizada
// salva os campos que se busca e os valores procurados 
typedef struct {
    Registro valores;

    // flags para avisar o que se está buscando quando uma busca for feita
    bool checar_codEstacao;
    bool checar_codLinha;
    bool checar_codProxEstacao;
    bool checar_distProxEstacao;
    bool checar_codLinhaIntegra;
    bool checar_codEstIntegra;
    bool checar_nomeEstacao;
    bool checar_nomeLinha;
} OQueBuscar;

// struct e vetor para guardar os pares únicos
typedef struct {
    int origem;
    int destino;
} Par;

// struct contendo todos os campos/variáveis que serão usados (para evitar chamada de funções passando dezenas de argumentos)
typedef struct {
    char** nomes_vistos;
    char bufferNome[69];
    Par* pares_vistos;
    int qtd_estacoes;
    int qtd_pares;
    int tamNomeEstacao;
    int codEstacao;
    int codProxEstacao;
} CamposUsados;

#endif