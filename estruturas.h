#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

// registro de cabeçalho (início do arquivo .bin) 17 bytes
typedef struct {
    char status;            // 0 -> inconsistente 1 -> consistente
    int topo;               // Byte offset do primeiro registro removido (Pilha de removidos)
    int proxRRN;            // valor do próximo RRN disponível (iniciado com 0)
    int nroEstacoes;        // qntd de estacoes diferentes  DEV NOTE: usar estrutura SET para armazenar entidades únicas e depois só contar
    int nroParesEstacao;    // qntd de pares (codEstacao e codProxEstacao) distintos já armazenados 
} Cabecalho;

// registro de dados (tam variavel)
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

#endif