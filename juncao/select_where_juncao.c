#include "select_where_juncao.h"


void select_where_juncao(const char* nomeArquivoBin1, const char* campo1, const char* nomeArquivoBin2, const char* campo2){

    FILE* bin1 = rotinaAbrirArquivo(nomeArquivoBin1, LEITURA);
    if (bin1 == NULL) return;
    
    FILE* bin2 = rotinaAbrirArquivo(nomeArquivoBin2, LEITURA);
    if (bin2 == NULL) return;
    
    // Inicializa o cursor no primeiro registro de ambos os arquivos
    fseek(bin1, OFFSET_INI_DADOS, SEEK_SET);
    fseek(bin2, OFFSET_INI_DADOS, SEEK_SET);

    // A partir daqui, percore o algoritmo descrito na funcionalidade 11 comparando registro por registro
    char removido1;
    char removido2;
    long offsetAtual1 = OFFSET_INI_DADOS;
    long offsetAtual2 = OFFSET_INI_DADOS;
    
    while(fread(&removido1, sizeof(char), 1, bin1) == 1){
        if (removido1 == '1'){
            fseek(bin1, TAM_REGISTRO-1, SEEK_CUR);
            offsetAtual1 += TAM_REGISTRO;
        } else {
            Registro temp1;
            binToStruct(&temp1, bin1);
            
            while(fread(&removido2, sizeof(char), 1, bin2) == 1){
                if (removido2 == '1'){
                    fseek(bin2, TAM_REGISTRO-1, SEEK_CUR);
                    offsetAtual2 += TAM_REGISTRO;
                } else {
                    Registro temp2;
                    binToStruct(&temp2, bin2);
                    
                    if (comparaCampos(&temp1, campo1, &temp2, campo2)){
                        printf("%d %s %s %d %s\n", temp1.codEstacao, temp1.nomeEstacao, temp1.nomeLinha, temp1.codProxEstacao, temp2.nomeEstacao);
                    } else {
                        offsetAtual2 += TAM_REGISTRO;
                    }

                    if (temp2.nomeEstacao) free(temp2.nomeEstacao);
                    if (temp2.nomeLinha) free(temp2.nomeLinha);
                }
            }

            fseek(bin2, OFFSET_INI_DADOS, SEEK_SET);
            offsetAtual1 += TAM_REGISTRO;
            if (temp1.nomeEstacao) free(temp1.nomeEstacao);
            if (temp1.nomeLinha) free(temp1.nomeLinha);
        }
    }

    fclose(bin1);
    fclose(bin2);
    
}


