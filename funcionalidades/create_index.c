#include "create_index.h"

#define N_MAX_NAMES 1000
#define LENGTH_MAX_NAMES 100


void create_index(const char* ArquivoBin, const char* ArquivoIndiceBin)
{
    
    FILE *bin = abrirVerificarInconsistentar(ArquivoBin);
    FILE *arv = fopen(ArquivoIndiceBin, "wb+");

    // Verificação de erro na abertura
    if (bin == NULL || arv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        if (bin) fclose(bin);
        if (arv) fclose(arv);
        return;
    }

    // -------------- começa escrevendo o cabeçalho do arquivo de indices ----------------
    CabecalhoArvore cab;
    ArvoreCriar(arv, &cab);
    gravarCabecalhoArvore(arv, &cab);

    // Cabeçalho tem 1 char + 4 int = 17 bytes
    fseek(bin, OFFSET_INI_DADOS, SEEK_SET);

    // A partir daqui, le registro por registro e vai inserindo na árvore
    char removido;
    long offsetAtual = OFFSET_INI_DADOS;     // De novo usamos uma variavel pra guardar o cursor e evitar ftell, assim como no primeiro trabalho
    int contador = 0;
    while (fread(&removido, sizeof(char), 1, bin) == 1){
        if (removido == '1'){
            fseek(bin, TAM_REGISTRO-1, SEEK_CUR);
            offsetAtual += TAM_REGISTRO;
        } else {
            Registro temp;
            binToStruct(&temp, bin);
            //printf("Inserindo chave %d offset %ld\n", temp.codEstacao, offsetAtual);  // debug
            ArvoreInserir(arv, &cab, temp.codEstacao, offsetAtual);
            //printf("Inserido! nroNos=%d\n", cab.nroNos);  // debug
            offsetAtual += TAM_REGISTRO;
            contador++;
        }
    }
    //printf("Total inserido: %d\n", contador);

    finalizarArquivo(bin, false);
    cab.status = '1';
    gravarCabecalhoArvore(arv, &cab);
    fclose(arv);

    BinarioNaTela((char *)ArquivoIndiceBin);
}