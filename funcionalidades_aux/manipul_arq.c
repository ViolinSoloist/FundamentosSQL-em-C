#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// lembrando que é em rb+
FILE* abrirVerificarInconsistentar(const char* nomeArqBin)
{
    FILE* file = fopen(nomeArqBin, "rb+");
    if (file == NULL) {
        fprintf(stderr, "Falha no processamento do arquivo.\n");
        return NULL;
    }
    
    // verifica consistencia do arquivo
    char status;
    fread(&status, sizeof(char), 1, file);
    if (status == '0') {
        fprintf(stderr, "Falha no processamento do arquivo.\n");
        fclose(file);
        return NULL;
    }
    
    // se tá tudo certo, então deixa inconsistente, uma vez que está sendo editado
    status = '0';
    fseek(file, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file);

    return file;
}

// deixa como consistente e finaliza o arquivo
void finalizarArquivo(FILE* file, bool debuggar) {
    
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