#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "estruturas.h"
#include "cabecalho.h"

// lembrando que é em rb+
FILE* rotinaAbrirArquivo(const char* nomeArqBin, TipoAbertura tipo)
{
    FILE* file;
    if (tipo == LEITURA)        file = fopen(nomeArqBin, "rb");
    else if (tipo == ESCRITA)   file = fopen(nomeArqBin, "rb+");
    else                        fprintf(stderr, "Tipo de abertura de arquivo inexistente.\n");
    
    if (file == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }
    // verifica consistencia do arquivo
    char status;
    fread(&status, sizeof(char), 1, file);
    if (status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(file);
        return NULL;
    }
    
    // se tá tudo certo, então deixa inconsistente, SE ESTIVER sendo editado
    if (tipo == ESCRITA) {
        status = '0';
        fseek(file, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, file);
    }

    return file;
}

// deixa como consistente e finaliza o arquivo que foi aberto como escrita
void finalizarArquivoEscrita(FILE* file, bool debuggar) {

    char status = '1';
    fseek(file, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file);
    
    if (debuggar) {
        int nEst, nPares;
        fseek(file, 9, SEEK_SET); // pula 1 (status) + 4 (topo) + 4 (proxRRN) = 9
        fread(&nEst, sizeof(int), 1, file);
        fread(&nPares, sizeof(int), 1, file);
        printf("DEBUG - Estacoes: %d, Pares: %d\n", nEst, nPares);
    }

    fclose(file);
}