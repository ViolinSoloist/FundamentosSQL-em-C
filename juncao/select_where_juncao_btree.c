#include "select_where_juncao_btree.h"

// busca o registro correspondente na árvore e imprime o match, se existir
static void buscarParNaArvore(FILE* bin2, Registro* temp1, FILE* arv, CabecalhoArvore* cab) {
    long offset = ArvoreBuscar(arv, cab, temp1->codProxEstacao);

    if (offset != -1){
        fseek(bin2, offset, SEEK_SET);
        char removido2;
        fread(&removido2, sizeof(char), 1, bin2);

        if (removido2 == '0'){
            Registro temp2;
            binToStruct(&temp2, bin2);
            printf("%d %s %s %d %s\n", temp1->codEstacao, temp1->nomeEstacao, temp1->nomeLinha, temp1->codProxEstacao, temp2.nomeEstacao);
            
            if (temp2.nomeEstacao) free(temp2.nomeEstacao);
            if (temp2.nomeLinha) free(temp2.nomeLinha);
        }
    }
}

void select_where_juncao_btree(const char* nomeArquivoBin1, const char* campo1, const char* nomeArquivoBin2, const char* campo2, const char* nomeArv){
    (void)campo1;
    (void)campo2;

    FILE* bin1 = rotinaAbrirArquivo(nomeArquivoBin1, LEITURA);
    FILE* bin2 = rotinaAbrirArquivo(nomeArquivoBin2, LEITURA);
    FILE* arv = rotinaAbrirArquivo(nomeArv, LEITURA);
    if (bin1 == NULL || bin2 == NULL || arv == NULL) return;

    CabecalhoArvore cab;
    lerCabecalhoArvore(arv, &cab);

    // loop externo
    char removido1;
    fseek(bin1, OFFSET_INI_DADOS, SEEK_SET);
    
    while(fread(&removido1, sizeof(char), 1, bin1) == 1){
        if (removido1 == '1'){
            fseek(bin1, TAM_REGISTRO-1, SEEK_CUR);
        } else {
            Registro temp1;
            binToStruct(&temp1, bin1);
            
            // delega a busca na arvore
            buscarParNaArvore(bin2, &temp1, arv, &cab);

            if (temp1.nomeEstacao) free(temp1.nomeEstacao);
            if (temp1.nomeLinha) free(temp1.nomeLinha);
        }
    }

    fclose(bin1);
    fclose(bin2);
    finalizarArquivoEscrita(arv, false);
}