#include "select_where_juncao.h"

// roda o loop interno no bin2 pra ver se acha par pro registro do bin1
static void processarLoopInterno(FILE* bin2, Registro* temp1, const char* campo1, const char* campo2) {
    char removido2;
    fseek(bin2, OFFSET_INI_DADOS, SEEK_SET); // volta pro começo do arquivo 2

    while(fread(&removido2, sizeof(char), 1, bin2) == 1){
        if (removido2 == '1') fseek(bin2, TAM_REGISTRO-1, SEEK_CUR);
        else {
            Registro temp2;
            binToStruct(&temp2, bin2);
            
            // achou? imprime o registro da junção
            if (comparaCampos(temp1, campo1, &temp2, campo2)) printf("%d %s %s %d %s\n", temp1->codEstacao, temp1->nomeEstacao, temp1->nomeLinha, temp1->codProxEstacao, temp2.nomeEstacao);
            
            // libera memória a no final do loop
            if (temp2.nomeEstacao) free(temp2.nomeEstacao);
            if (temp2.nomeLinha) free(temp2.nomeLinha);
        }
    }
}

void select_where_juncao(const char* nomeArquivoBin1, const char* campo1, const char* nomeArquivoBin2, const char* campo2){

    FILE* bin1 = rotinaAbrirArquivo(nomeArquivoBin1, LEITURA);
    FILE* bin2 = rotinaAbrirArquivo(nomeArquivoBin2, LEITURA);
    if (bin1 == NULL || bin2 == NULL) {
        if(bin1) fclose(bin1);
        if(bin2) fclose(bin2);
        return;
    }
    
    // percorre arquivo 1
    char removido1;
    fseek(bin1, OFFSET_INI_DADOS, SEEK_SET);
    
    while(fread(&removido1, sizeof(char), 1, bin1) == 1){
        if (removido1 == '1') fseek(bin1, TAM_REGISTRO-1, SEEK_CUR);
        else {
            Registro temp1;
            binToStruct(&temp1, bin1);
            
            // delega a busca no arquivo 2
            processarLoopInterno(bin2, &temp1, campo1, campo2);

            if (temp1.nomeEstacao) free(temp1.nomeEstacao);
            if (temp1.nomeLinha) free(temp1.nomeLinha);
        }
    }

    fclose(bin1);
    fclose(bin2);
}