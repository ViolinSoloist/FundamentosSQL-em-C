#include "select_where_juncao_btree.h"


void select_where_juncao_btree(const char* nomeArquivoBin1, const char* campo1, const char* nomeArquivoBin2, const char* campo2, const char* nomeArv){
    
    (void)campo1;
    (void)campo2;

    FILE* bin1 = rotinaAbrirArquivo(nomeArquivoBin1, LEITURA);
    if (bin1 == NULL) return;
    
    FILE* bin2 = rotinaAbrirArquivo(nomeArquivoBin2, LEITURA);
    if (bin2 == NULL) return;
    
    FILE* arv = rotinaAbrirArquivo(nomeArv, LEITURA);
    if (arv == NULL) return;

    CabecalhoArvore cab;
    lerCabecalhoArvore(arv, &cab);

    // Inicializa o cursor no primeiro registro de ambos os arquivos
    fseek(bin1, OFFSET_INI_DADOS, SEEK_SET);
    fseek(bin2, OFFSET_INI_DADOS, SEEK_SET);

    // A partir daqui, percore o algoritmo descrito na funcionalidade 12 comparando registro por registro
    char removido1;
    long offsetAtual1 = OFFSET_INI_DADOS;
    
    
    while(fread(&removido1, sizeof(char), 1, bin1) == 1){
        if (removido1 == '1'){
            fseek(bin1, TAM_REGISTRO-1, SEEK_CUR);
            offsetAtual1 += TAM_REGISTRO;
        } else {
            Registro temp1;
            binToStruct(&temp1, bin1);
            
            long offset = ArvoreBuscar(arv, &cab, temp1.codProxEstacao);

            if (offset != -1){

                fseek(bin2, offset, SEEK_SET);

                char removido2;
                fread(&removido2, sizeof(char), 1, bin2);

                if (removido2 == '0'){
                    Registro temp2;
                    binToStruct(&temp2, bin2);
                    printf("%d %s %s %d %s\n", temp1.codEstacao, temp1.nomeEstacao, temp1.nomeLinha, temp1.codProxEstacao, temp2.nomeEstacao);
                    if (temp2.nomeEstacao) free(temp2.nomeEstacao);
                    if (temp2.nomeLinha) free(temp2.nomeLinha);
                }
                   
            }

            offsetAtual1 += TAM_REGISTRO;
            if (temp1.nomeEstacao) free(temp1.nomeEstacao);
            if (temp1.nomeLinha) free(temp1.nomeLinha);
        }
    }

    fclose(bin1);
    fclose(bin2);
    finalizarArquivoEscrita(arv, false);
}
