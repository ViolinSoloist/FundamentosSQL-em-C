#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "estruturas.h" 
#include "fornecidas.h"
#include "manipul_arq.h"

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

/// @private @brief pega os dados do csv pra salvar num registro intermediário, ai usa esse registro pra escrever no binário
// daria pra escrever diretamente pegando do csv, mas não consegui achar uma forma de fazer isso sem ficar bagunçado, confuso e desorganizado)
static void salvaDadosCSVNoRegistro(Registro* temporario, char* linha_entrada_csv) 
{   
    // total é a linha inteira, após primeira execução de strsep, pedaco = tudo até a primeira virgula, total = todo restante
    char *total = linha_entrada_csv;
    char *pedaco;
    
    pedaco = strsep(&total, ",");
    temporario->codEstacao = atoi(pedaco);

    pedaco = strsep(&total, ",");
    if (pedaco && *pedaco) {
        temporario->nomeEstacao = malloc(strlen(pedaco) + 1);
        strcpy(temporario->nomeEstacao, pedaco);
    } else {
        // se estiver vazio, aloca uma string vazia
        temporario->nomeEstacao = malloc(1);
        temporario->nomeEstacao[0] = '\0';
    }

    pedaco = strsep(&total, ",");
    temporario->codLinha = (pedaco && *pedaco) ? atoi(pedaco) : -1;

    pedaco = strsep(&total, ",");
    if (pedaco && *pedaco) {
        temporario->nomeLinha = malloc(strlen(pedaco) + 1);
        strcpy(temporario->nomeLinha, pedaco);
    } else {
        temporario->nomeLinha = malloc(1);
        temporario->nomeLinha[0] = '\0';
    }

    pedaco = strsep(&total, ",");
    temporario->codProxEstacao = (pedaco && *pedaco) ? atoi(pedaco) : -1;

    pedaco = strsep(&total, ",");
    temporario->distProxEstacao = (pedaco && *pedaco) ? atoi(pedaco) : -1;

    pedaco = strsep(&total, ",");
    temporario->codLinhaIntegra = (pedaco && *pedaco) ? atoi(pedaco) : -1;

    pedaco = strsep(&total, ",");
    temporario->codEstIntegra = (pedaco && *pedaco) ? atoi(pedaco) : -1;

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

/// @private @brief pega todas as coisas salvas na struct Registro buffer (copiadas do csv) e escreve no arquivo binario
static void gravarRegistroBin(Registro* temporario, FILE* bin, char removido, int proxLista, int* proxRRN)
{
    // gravação dos dados fixos
    fwrite(&removido, sizeof(char), 1, bin);
    fwrite(&proxLista, sizeof(int), 1, bin);
    fwrite(&temporario->codEstacao, sizeof(int), 1, bin);
    fwrite(&temporario->codLinha, sizeof(int), 1, bin);
    fwrite(&temporario->codProxEstacao, sizeof(int), 1, bin);
    fwrite(&temporario->distProxEstacao, sizeof(int), 1, bin);
    fwrite(&temporario->codLinhaIntegra, sizeof(int), 1, bin);
    fwrite(&temporario->codEstIntegra, sizeof(int), 1, bin);

    int bytesEscritos = 29; // 1 char + (7 ints * 4 bytes)

    // dados variaveis
    
    // nao nulo
    int tamNomeEstacao = strlen(temporario->nomeEstacao);
    fwrite(&tamNomeEstacao, sizeof(int), 1, bin);
    fwrite(temporario->nomeEstacao, sizeof(char), tamNomeEstacao, bin); // sem o '\0'
    bytesEscritos += 4 + tamNomeEstacao; 


    int tamNomeLinha = strlen(temporario->nomeLinha);
    fwrite(&tamNomeLinha, sizeof(int), 1, bin); 
    if (tamNomeLinha > 0) { 
        // escreve se existir, se não vira lixo
        fwrite(temporario->nomeLinha, sizeof(char), tamNomeLinha, bin); 
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
        salvaDadosCSVNoRegistro(&temporario, linha);
        
        // ---------------- CONTAGEM DE ESTAÇÕES E PARES -----------------------
        contarEstacoesEPares(&temporario, nomesVistos, &totalEstacoes, paresVistos, &totalPares);

        // --------------- GRAVAÇÃO NO ARQUIVO BIN -------------------
        // setup dados fixos 
        char removido = '0'; // não removido
        int proxLista = -1;  // RRN do próximo removido (-1 inicial)
        gravarRegistroBin(&temporario, bin, removido, proxLista, &proxRRN);

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