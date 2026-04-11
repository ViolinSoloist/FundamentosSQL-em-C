#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "estruturas.h" 
#include "fornecidas.h"
#include "manipul_arq.h"
#include "IO.h"

#define N_MAX_NAMES 1000
#define LENGTH_MAX_NAMES 100

/// @private @brief inicializa a escrita de dados no arquivo bin, começando com o registro de cabeçalho
static void escreveCabecarioBin(bool seek_inicio, FILE* bin, char status, int proxRRN, int nroEstacoes, int nroParesEstacao)
{
    if (!seek_inicio) {
        // primeira passada: escreve tudo do na ordem
        int topo = -1;
        fseek(bin, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, bin);
        fwrite(&topo, sizeof(int), 1, bin);
        fwrite(&proxRRN, sizeof(int), 1, bin);
        fwrite(&nroEstacoes, sizeof(int), 1, bin);
        fwrite(&nroParesEstacao, sizeof(int), 1, bin);
    } else {
        // segunda passada: atualiza o que mudou, mantem topo intacto, precisa de fseek
        fseek(bin, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, bin);
        
        fseek(bin, 5, SEEK_SET); // Pula o 'topo' (-1) que já está no arquivo
        fwrite(&proxRRN, sizeof(int), 1, bin);
        fwrite(&nroEstacoes, sizeof(int), 1, bin);
        fwrite(&nroParesEstacao, sizeof(int), 1, bin);
    }
}

/// @private
static void contarEstacoesEPares(Registro* temporario, char* nomesVistos[], int* totalEstacoes, Par paresVistos[], int* totalPares)
{
    // contagem estações únicas (pelo nome)
    if (strlen(temporario->nomeEstacao) > 0) {
        bool estacaoJaExiste = false;
        for (int i = 0; i < (*totalEstacoes); i++) {
            if (!strcmp(nomesVistos[i], temporario->nomeEstacao)) {
                estacaoJaExiste = true;
                break;
            }
        }
        if (!estacaoJaExiste) {
            // aloca espaço (no primeiro index não ocupado do vetor de nomes) suficente para o nome
            nomesVistos[*totalEstacoes] = malloc(strlen(temporario->nomeEstacao) + 1);
            strcpy(nomesVistos[*totalEstacoes], temporario->nomeEstacao);
            (*totalEstacoes)++;
        }
    }

    // contagem de pares
    if (temporario->codProxEstacao != -1) { 
        bool parJaExiste = false;
        for (int i = 0; i < (*totalPares); i++) {
            if (paresVistos[i].origem == temporario->codEstacao && paresVistos[i].destino == temporario->codProxEstacao) {
                parJaExiste = true;
                break;
            }
        }
        if (!parJaExiste) {
            paresVistos[*totalPares].origem = temporario->codEstacao;
            paresVistos[*totalPares].destino = temporario->codProxEstacao;
            (*totalPares)++;
        }
    }
}


void create_table(const char *nomeArquivoCSV, const char *nomeArquivoBin)
{
    FILE *csv = fopen(nomeArquivoCSV, "r");
    FILE *bin = fopen(nomeArquivoBin, "wb");

    // verificação
    if (csv == NULL || bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        if (csv) fclose(csv);
        if (bin) fclose(bin);
        return;
    }

    // começa escrevendo o cabeçalho do arquivo bin
    char status = '0';          // começa inconsistente
    int proxRRN = 0;            // próximo RRN disponível (começa zerado)
    // valores zerados mas que ainda antes da função terminar serão preenchidos:
    int nroEstacoes = 0;            // qntd de estacoes diferentes
    int nroParesEstacao = 0;
    // não precisa dar seek pro inicio
    bool seek_inicio = false; escreveCabecarioBin(seek_inicio, bin, status, proxRRN, nroEstacoes, nroParesEstacao);

    // skip header
    char linha[500];            // bufer linha, toda linha lida é salva temporáriamente aqui
    fgets(linha, sizeof(linha), csv); 


    // ----- inicialização de variáveis/estruturas para guardar contagem de estações e pares -------

    // contagem de pares por nomes distintos (não pode ser ID porque contaria estações com mesmo nome mas em linhas diferentes (tipo luz, que faz parte de 4 linhas?))
    char* nomesVistos[2000];
    int totalEstacoes = 0;
    
    Par paresVistos[5000];
    int totalPares = 0;

    // --------- LEITURA CSV + GUARDAR DADOS -----------

    while (fgets(linha, sizeof(linha), csv) != NULL) {
        
        // ------------------- LEITURA DE UMA LINHA DO CSV -------------------------------

        // tratamento de quebra de linhas (windows, linux)
        linha[strcspn(linha, "\n")] = '\0';
        linha[strcspn(linha, "\r")] = '\0';

        // tratamento linah "fantasma"
        if (strlen(linha) == 0) continue; 

        // --------------------------------------------- SALVA DADOS DO CSV EM UM REGISTRO TEMPORÁRIO (BUFFER) -------------------------------------
        Registro temporario;
        salvaDadosNoRegistro(&temporario, linha);
        
        // ---------------- CONTAGEM DE ESTAÇÕES E PARES -----------------------
        contarEstacoesEPares(&temporario, nomesVistos, &totalEstacoes, paresVistos, &totalPares);

        // --------------- GRAVAÇÃO NO ARQUIVO BIN -------------------
        // setup dados fixos 
        char removido = '0'; // não removido
        int proxLista = -1;  // RRN do próximo removido (-1 inicial)
        gravarRegistroBin(&temporario, bin, removido, proxLista);
        proxRRN++;

        // liberar memória dos campos variáveis do registro temporário ao fim do uso
        free(temporario.nomeEstacao);
        free(temporario.nomeLinha);
    }
    
    status = '1';                           // volta para o primeiro byte e marca como consistente
    seek_inicio = true; // true quando é a segunda vez escrevendo no cabecario, e necessita de um seek
    escreveCabecarioBin(seek_inicio, bin, status, proxRRN, totalEstacoes, totalPares);
    
    // limpeza de memória do espaço alocado pros nomes no vetor de nomes já vistos
    for (int i = 0; i < totalEstacoes; i++) 
        free(nomesVistos[i]);

    // debug
    // printf("DEBUG - proxRRN: %d | Estacoes: %d | Pares: %d\n", proxRRN, totalEstacoes, totalPares);

    fclose(csv);
    fclose(bin);

    BinarioNaTela((char *)nomeArquivoBin);
}