#include <stdio.h>
#include <string.h>

#include "estruturas.h"
#include "bin_struct.h"

/**
 * @private função auxiliar, le o registro atual do arq bin e copia os dados pra struct Registro
 * recebe ponteiro para @param regAtual e o arquivo @param bin
 */
void binToStruct(Registro* regAtual, FILE* bin) {
    regAtual->removido = '0';

    // leitura pra struct Registro dos campos fixos (28 bytes: 7 * 4 )
    fread(&regAtual->proximo, sizeof(int), 1, bin);
    fread(&regAtual->codEstacao, sizeof(int), 1, bin);
    fread(&regAtual->codLinha, sizeof(int), 1, bin);
    fread(&regAtual->codProxEstacao, sizeof(int), 1, bin);
    fread(&regAtual->distProxEstacao, sizeof(int), 1, bin);
    fread(&regAtual->codLinhaIntegra, sizeof(int), 1, bin);
    fread(&regAtual->codEstIntegra, sizeof(int), 1, bin);
    
    int bytes_lidos = 29; // 1 char  + 7*4

    // acho que isso dá pra usar vetor estático, pra evitar malloc
    char bufferEstacao[69] = ""; 
    char bufferLinha[69] = "";

    fread(&regAtual->tamNomeEstacao, sizeof(int), 1, bin);
    bytes_lidos += 4;

    // se (tamanho > 0) => existe nome
    if (regAtual->tamNomeEstacao > 0) {
        fread(bufferEstacao, sizeof(char), regAtual->tamNomeEstacao, bin);
        bufferEstacao[regAtual->tamNomeEstacao] = '\0';
        bytes_lidos += regAtual->tamNomeEstacao;
    }
    regAtual->nomeEstacao = bufferEstacao; // aponta nomeEstacao da struct para o buffer local, em vez de alocar dinamicamente

    fread(&regAtual->tanNomeLinha, sizeof(int), 1, bin);
    bytes_lidos += 4;
    if (regAtual->tanNomeLinha > 0) {
        fread(bufferLinha, sizeof(char), regAtual->tanNomeLinha, bin);
        bufferLinha[regAtual->tanNomeLinha] = '\0';
        bytes_lidos += regAtual->tanNomeLinha;
    }
    regAtual->nomeLinha = bufferLinha;
    
    // pular lixo
    fseek(bin, 80 - bytes_lidos, SEEK_CUR);
}