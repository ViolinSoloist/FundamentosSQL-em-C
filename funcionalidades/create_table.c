#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "estruturas.h" 
#include "fornecidas.h"

#define N_MAX_NAMES 1000
#define LENGTH_MAX_NAMES 100

// strsep versao windows (temporário) (por algum motivo, strsep não funciona mesmo com #include <string.h> ¯\_(ツ)_/¯)
char *strsep(char **stringp, const char *delim) {
    char *start = *stringp;
    char *p;
    
    if (start == NULL) {
        return NULL;
    }
    
    // procura o delimitador
    p = strpbrk(start, delim);
    if (p) {
        *p = '\0'; 
        *stringp = p + 1; 
    } else {
        *stringp = NULL; // fim da linha
    }
    
    return start;
}

void create_table(const char *nomeArquivoCSV, const char *nomeArquivoBin)
{
    FILE *csv = fopen(nomeArquivoCSV, "r");
    FILE *bin = fopen(nomeArquivoBin, "wb");

    // verificação
    if (csv == NULL || bin == NULL) {
        fprintf(stderr, "Falha no processamento do arquivo.\n");
        if (csv) fclose(csv);
        if (bin) fclose(bin);
        return;
    }

    // CABEÇALHO 
    char status = '0';          // começa inconsistente
    int topo = -1;              // stack removidos vazia
    int proxRRN = 0;            // próximo RRN disponível (começa zerado)

    // valores zerados mas que ainda antes da função terminar serão preenchidos:
    int nroEstacoes = 0;            // qntd de estacoes diferentes  DEV NOTE: usar estrutura SET para armazenar entidades únicas e depois só contar
    int nroParesEstacao = 0;
    
    // escrever campo a campo para evitar problemas com padding
    fwrite(&status, sizeof(char), 1, bin);
    fwrite(&topo, sizeof(int), 1, bin);
    fwrite(&proxRRN, sizeof(int), 1, bin);
    fwrite(&nroEstacoes, sizeof(int), 1, bin);
    fwrite(&nroParesEstacao, sizeof(int), 1, bin);

    // skip header
    char linha[512];
    fgets(linha, sizeof(linha), csv); 

    // ----- contagem de estações e pares -------

    // contagem de pares por ID distintos
    int idsVistos[1000];
    int totalEstacoes = 0;

    // struct e vetor para guardar os pares únicos
    typedef struct {
        int origem;
        int destino;
    } Par;
    
    Par paresVistos[5000];
    int totalPares = 0;

    // --------- LEITURA CSV + GUARDAR DADOS -----------

    while (fgets(linha, sizeof(linha), csv) != NULL) {
        
        // tratamento de quebra de linhas (windows, linux)
        linha[strcspn(linha, "\n")] = '\0';
        linha[strcspn(linha, "\r")] = '\0';

        // tratamento linah "fantasma"
        if (strlen(linha) == 0) continue; 

        // total é a linha inteira, após primeira execução de strsep, pedaco = tudo até a primeira virgula, total = todo restante
        char *total = linha;
        char *pedaco;

        pedaco = strsep(&total, ",");
        int codEstacao = atoi(pedaco);

        pedaco = strsep(&total, ",");
        char nomeEstacao[100] = "";
        if (pedaco && *pedaco) strcpy(nomeEstacao, pedaco);

        pedaco = strsep(&total, ",");
        int codLinha = (pedaco && *pedaco) ? atoi(pedaco) : -1;

        pedaco = strsep(&total, ",");
        char nomeLinha[100] = "";
        if (pedaco && *pedaco) strcpy(nomeLinha, pedaco);

        pedaco = strsep(&total, ",");
        int codProxEstacao = (pedaco && *pedaco) ? atoi(pedaco) : -1;

        pedaco = strsep(&total, ",");
        int distProxEstacao = (pedaco && *pedaco) ? atoi(pedaco) : -1;

        pedaco = strsep(&total, ",");
        int codLinhaIntegra = (pedaco && *pedaco) ? atoi(pedaco) : -1;

        pedaco = strsep(&total, ",");
        int codEstIntegra = (pedaco && *pedaco) ? atoi(pedaco) : -1;

        // contagem estações únicas
        int estacaoJaExiste = 0;
        for (int i = 0; i < totalEstacoes; i++) {
            if (idsVistos[i] == codEstacao) {
                estacaoJaExiste = 1;
                break;
            }
        }
        if (!estacaoJaExiste) {
            idsVistos[totalEstacoes] = codEstacao;
            totalEstacoes++;
        }

        // contagem de pares
        if (codProxEstacao != -1) { 
            int parJaExiste = 0;
            for (int i = 0; i < totalPares; i++) {
                if (paresVistos[i].origem == codEstacao && paresVistos[i].destino == codProxEstacao) {
                    parJaExiste = 1;
                    break;
                }
            }
            if (!parJaExiste) {
                paresVistos[totalPares].origem = codEstacao;
                paresVistos[totalPares].destino = codProxEstacao;
                totalPares++;
            }
        }

        // setup dados fixos 
        char removido = '0'; // não removido
        int proxLista = -1;  // RRN do próximo removido (-1 inicial)
        
        // gravação dos dados fixos
        fwrite(&removido, sizeof(char), 1, bin);
        fwrite(&proxLista, sizeof(int), 1, bin);
        fwrite(&codEstacao, sizeof(int), 1, bin);
        fwrite(&codLinha, sizeof(int), 1, bin);
        fwrite(&codProxEstacao, sizeof(int), 1, bin);
        fwrite(&distProxEstacao, sizeof(int), 1, bin);
        fwrite(&codLinhaIntegra, sizeof(int), 1, bin);
        fwrite(&codEstIntegra, sizeof(int), 1, bin);

        int bytesEscritos = 29; // 1 char + (7 ints * 4 bytes)

        // dados variaveis
        
        // nao nulo
        int tamNomeEstacao = strlen(nomeEstacao);
        fwrite(&tamNomeEstacao, sizeof(int), 1, bin);
        fwrite(nomeEstacao, sizeof(char), tamNomeEstacao, bin); // sem o '\0'
        bytesEscritos += 4 + tamNomeEstacao; 


        int tamNomeLinha = strlen(nomeLinha);
        fwrite(&tamNomeLinha, sizeof(int), 1, bin); 
        if (tamNomeLinha > 0) { 
            // escreve se existir, se não vira lixo
            fwrite(nomeLinha, sizeof(char), tamNomeLinha, bin); 
        }
        bytesEscritos += 4 + tamNomeLinha; 

        // preenchimento do restante com lixo
        char lixo = '$';
        while (bytesEscritos < 80) {
            fwrite(&lixo, sizeof(char), 1, bin);
            bytesEscritos++;
        }
        
        // próximo registro até acabar
        proxRRN++;
    }
    
    // atualizacao do estado do registro (inconsistente -> consistente)
    fseek(bin, 0, SEEK_SET);
    
    status = '1';                           // volta para o primeiro byte e marca como consistente
    fwrite(&status, sizeof(char), 1, bin);  // (sobrescrever)

    // pular campo topo e ir para byte 5, atualizar proxRRN, nroEstacos, nroParesEstacoes 
    fseek(bin, 5, SEEK_SET); 
    fwrite(&proxRRN, sizeof(int), 1, bin);
    fwrite(&totalEstacoes, sizeof(int), 1, bin); // grava nroEstacoes
    fwrite(&totalPares, sizeof(int), 1, bin);    // grava nroParesEstacao
    
    // debug
    // printf("DEBUG - proxRRN: %d | Estacoes: %d | Pares: %d\n", proxRRN, totalEstacoes, totalPares);

    fclose(csv);
    fclose(bin);

    BinarioNaTela((char *)nomeArquivoBin);
}