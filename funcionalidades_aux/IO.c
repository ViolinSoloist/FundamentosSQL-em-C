#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IO.h"
#include "estruturas.h"
#include "busca.h"
#include "fornecidas.h"

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

/// @brief pega os dados do csv pra salvar num registro intermediário, ai usa esse registro pra escrever no binário
// daria pra escrever diretamente pegando do csv, mas não consegui achar uma forma de fazer isso sem ficar bagunçado, confuso e desorganizado)
void salvaDadosNoRegistro(Registro* temporario, char* linha_entrada) 
{   
    // total é a linha inteira, após primeira execução de strsep, pedaco = tudo até a primeira virgula, total = todo restante
    char *total = linha_entrada;
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

void mostrarRegistro(Registro* registro_lido){
    
    printf("%d ", registro_lido->codEstacao);
    printf("%s ", registro_lido->nomeEstacao);

    if (registro_lido->codLinha == -1) printf("NULO ");
    else printf("%d ", registro_lido->codLinha);

    if (registro_lido->tanNomeLinha > 0 && registro_lido->nomeLinha != NULL)
        printf("%s ", registro_lido->nomeLinha);
    else printf("NULO ");

    if (registro_lido->codProxEstacao == -1) printf("NULO ");
    else printf("%d ", registro_lido->codProxEstacao);

    if (registro_lido->distProxEstacao == -1) printf("NULO ");
    else printf("%d ", registro_lido->distProxEstacao);

    if (registro_lido->codLinhaIntegra == -1) printf("NULO ");
    else printf("%d ", registro_lido->codLinhaIntegra);

    if (registro_lido->codEstIntegra == -1) printf("NULO\n");
    else printf("%d\n", registro_lido->codEstIntegra);
}

void lerLinhaBusca(FILE* file, AcaoPosBusca callback, void* dados_extras){
    int m;
    scanf("%d", &m);

    OQueBuscar oqBuscar;
    
    // PREENCHIMENTO DA QUERY "CHECKLIST"
    preencherQuery(&oqBuscar, m);

    // chama função de busca para salvar correspondencias
    int qtd_encontrados = 0;
    long* offsets = percorreEBuscaCorrespondencia(file, oqBuscar, &qtd_encontrados); // lembrar de liberar memória depois de usar essa função

    // se achar correspondências, aplicar a função callback passando os dados encontrados
    if (callback != NULL)
        callback(file, qtd_encontrados, offsets, dados_extras);
        
    // limpa memória alocada para vetor offsets
    free(offsets);

    // corresponde à alocação dentro de marcadorFlag()
    if (oqBuscar.valores.nomeEstacao) free(oqBuscar.valores.nomeEstacao);
    if (oqBuscar.valores.nomeLinha) free(oqBuscar.valores.nomeLinha);
}

void lerLinhaNormal(FILE* file, char* linha){
    fgets(linha, sizeof(linha), file);

    // tratamento de quebra de linhas (windows, linux)
    linha[strcspn(linha, "\n")] = '\0';
    linha[strcspn(linha, "\r")] = '\0';

    // tratamento linah "fantasma"
    if (strlen(linha) == 0) return;
}

/// @private @brief pega todas as coisas salvas na struct Registro buffer (copiadas do csv) e escreve no arquivo binario
void gravarRegistroBin(Registro* temporario, FILE* bin, char removido, int proxLista)
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
    
    // --- dados variaveis ---
    
    // Tratamento seguro para Nome da Estação
    int tamNomeEstacao = 0;
    if (temporario->nomeEstacao != NULL) {
        tamNomeEstacao = strlen(temporario->nomeEstacao);
    }
    
    fwrite(&tamNomeEstacao, sizeof(int), 1, bin);
    if (tamNomeEstacao > 0) {
        fwrite(temporario->nomeEstacao, sizeof(char), tamNomeEstacao, bin); // sem o '\0'
    }
    bytesEscritos += 4 + tamNomeEstacao; 


    // Tratamento seguro para Nome da Linha
    int tamNomeLinha = 0;
    if (temporario->nomeLinha != NULL) {
        tamNomeLinha = strlen(temporario->nomeLinha);
    }
    
    fwrite(&tamNomeLinha, sizeof(int), 1, bin); 
    if (tamNomeLinha > 0) { 
        fwrite(temporario->nomeLinha, sizeof(char), tamNomeLinha, bin); 
    }
    bytesEscritos += 4 + tamNomeLinha; 

    // --- preenchimento do restante com lixo ---
    char lixo = '$';
    while (bytesEscritos < 80) {
        fwrite(&lixo, sizeof(char), 1, bin);
        bytesEscritos++;
    }
    
}

// Função para ler diretamente do terminal para um registro temporário
void lerRegistro(Registro* temp){
    char buffer[200]; // Buffer temporário para ler qualquer campo

    // codEstacao
    ScanQuoteString(buffer);
    temp->codEstacao = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);

    // nomeEstacao
    ScanQuoteString(buffer);
    temp->nomeEstacao = malloc(strlen(buffer) + 1);
    strcpy(temp->nomeEstacao, buffer);

    // codLinha
    ScanQuoteString(buffer);
    temp->codLinha = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);

    // nomeLinha
    ScanQuoteString(buffer);
    temp->nomeLinha = malloc(strlen(buffer) + 1);
    strcpy(temp->nomeLinha, buffer);

    // codProxEstacao
    ScanQuoteString(buffer);
    temp->codProxEstacao = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);

    // distProxEstacao
    ScanQuoteString(buffer);
    temp->distProxEstacao = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);

    // codLinhaIntegra
    ScanQuoteString(buffer);
    temp->codLinhaIntegra = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);

    // codEstIntegra
    ScanQuoteString(buffer);
    temp->codEstIntegra = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);
}