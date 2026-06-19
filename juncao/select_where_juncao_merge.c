#include "select_where_juncao_merge.h"

void select_where_juncao_merge(const char* nomeArquivoBin1, const char* campo1, const char* nomeArquivoBin2, const char* campo2){

    const char* tempOrdenadoCodEstacao = "tempOrdenadoCodEstacao.bin";
    const char* tempOrdenadoProxEstacao = "tempOrdenadoProxEstacao.bin";

    // Gera os dois arquivos ordenados
    order_by(nomeArquivoBin1, campo1, tempOrdenadoCodEstacao, false);
    order_by(nomeArquivoBin2, campo2, tempOrdenadoProxEstacao, false);

    // Abre ambos os arquivos gerados em modo de LEITURA
    FILE* bin1 = rotinaAbrirArquivo(tempOrdenadoCodEstacao, LEITURA);
    FILE* bin2 = rotinaAbrirArquivo(tempOrdenadoProxEstacao, LEITURA);

    if (bin1 == NULL || bin2 == NULL) {
        if (bin1) fclose(bin1);
        if (bin2) fclose(bin2);
        return;
    }

    fseek(bin1, 0, SEEK_END);
    long tamanho1 = ftell(bin1);

    fseek(bin2, 0, SEEK_END);
    long tamanho2 = ftell(bin2);

    long offset1 = OFFSET_INI_DADOS;
    long offset2 = OFFSET_INI_DADOS;

    fseek(bin1, OFFSET_INI_DADOS, SEEK_SET);
    fseek(bin2, OFFSET_INI_DADOS, SEEK_SET);

    Registro temp1;
    Registro temp2;

    lerProximoRegistro(&temp1, bin1);
    lerProximoRegistro(&temp2, bin2);
    
    while (offset1 < tamanho1 && offset2 < tamanho2){
        
        if (temp1.codProxEstacao < temp2.codEstacao){
            if (temp1.nomeEstacao) free(temp1.nomeEstacao);
            if (temp1.nomeLinha) free(temp1.nomeLinha);
            lerProximoRegistro(&temp1, bin1);
            offset1 += TAM_REGISTRO;
            
        } else if (temp1.codProxEstacao > temp2.codEstacao){
            if (temp2.nomeEstacao) free(temp2.nomeEstacao);
            if (temp2.nomeLinha) free(temp2.nomeLinha);
            lerProximoRegistro(&temp2, bin2);
            offset2 += TAM_REGISTRO;
            

        } else if (temp1.codProxEstacao == temp2.codEstacao) {
            printf("%d %s %s %d %s\n", temp1.codEstacao, temp1.nomeEstacao, temp1.nomeLinha, temp1.codProxEstacao, temp2.nomeEstacao);
            if (temp1.nomeEstacao) free(temp1.nomeEstacao);
            if (temp1.nomeLinha) free(temp1.nomeLinha);
            lerProximoRegistro(&temp1, bin1);
            offset1 += TAM_REGISTRO;
            
        }

    }

    
    if (temp1.nomeEstacao) free(temp1.nomeEstacao);
    if (temp1.nomeLinha) free(temp1.nomeLinha);

    if (temp2.nomeEstacao) free(temp2.nomeEstacao);
    if (temp2.nomeLinha) free(temp2.nomeLinha);


    fclose(bin1);
    fclose(bin2);

    remove(tempOrdenadoCodEstacao);
    remove(tempOrdenadoProxEstacao);

}