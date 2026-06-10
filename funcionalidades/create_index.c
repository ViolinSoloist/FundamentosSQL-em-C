#include "create_index.h"

#define N_MAX_NAMES 1000
#define LENGTH_MAX_NAMES 100
#define INICIO_DADOS 


void create_index(const char* ArquivoBin, const char* ArquivoIndiceBin)
{
    FILE *arquivoBin = fopen(ArquivoBin, "rb");
    FILE *arquivoIndiceBin = fopen(ArquivoIndiceBin, "wb");

    // verificação
    if (arquivoBin == NULL || arquivoIndiceBin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        if (arquivoBin) fclose(arquivoBin);
        if (arquivoIndiceBin) fclose(arquivoIndiceBin);
        return;
    }

    // -------------- começa escrevendo o cabeçalho do arquivo de indices ----------------
    CabecalhoArvore cab;
    ArvoreCriar(arquivoIndiceBin, &cab);
    gravarCabecalhoArvore(arquivoIndiceBin, &cab);

    // Cabeçalho tem 1 char + 4 int = 17 bytes
    fseek(arquivoBin, OFFSET_INI_DADOS, SEEK_SET);

    // A partir daqui, le registro por registro e vai inserindo na árvore
    char removido;
    long offsetAtual = OFFSET_INI_DADOS;     // De novo usamos uma variavel pra guardar o cursor e evitar ftell, assim como no primeiro trabalho
    while (fread(&removido, sizeof(char), 1, arquivoBin) == 1){
        // Se o registro estiver marcado como removido, só pula ele e vai pro próximo
        if (removido == '1'){
            fseek(arquivoBin, TAM_REGISTRO-1, SEEK_CUR);
            offsetAtual += TAM_REGISTRO;

        // Senão, le ele pra struct, e insere na arvore um nó com as informações dele
        } else {
            Registro temp;
            binToStruct(&temp, arquivoBin);
            ArvoreInserir(arquivoIndiceBin, &cab, temp.codEstacao, offsetAtual);
            offsetAtual += TAM_REGISTRO;
        }
    }

    cab.status = '1';
    gravarCabecalhoArvore(arquivoIndiceBin, &cab);

    fclose(arquivoBin);
    fclose(arquivoIndiceBin);

    BinarioNaTela((char *)ArquivoIndiceBin);
}