#include "create_index.h"

#define N_MAX_NAMES 1000
#define LENGTH_MAX_NAMES 100


void create_index(const char* ArquivoBin, const char* ArquivoIndiceBin)
{
    FILE *bin = abrirVerificarInconsistentar(ArquivoBin);
    FILE *arv = abrirVerificarInconsistentar(ArquivoIndiceBin);

    // verificação
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
    while (fread(&removido, sizeof(char), 1, bin) == 1){
        // Se o registro estiver marcado como removido, só pula ele e vai pro próximo
        if (removido == '1'){
            fseek(bin, TAM_REGISTRO-1, SEEK_CUR);
            offsetAtual += TAM_REGISTRO;

        // Senão, le ele pra struct, e insere na arvore um nó com as informações dele
        } else {
            Registro temp;
            binToStruct(&temp, bin);
            ArvoreInserir(arv, &cab, temp.codEstacao, offsetAtual);
            offsetAtual += TAM_REGISTRO;
        }
    }

    finalizarArquivo(bin, false);
    finalizarArquivo(arv, false);

    BinarioNaTela((char *)ArquivoIndiceBin);
}