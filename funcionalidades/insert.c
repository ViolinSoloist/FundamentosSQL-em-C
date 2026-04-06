#include <stdio.h>
#include <stdlib.h>

#include "insert.h"
#include "create_table.h"
#include "IO.h"
#include "estruturas.h"
#include "manipul_arq.h"
#include "fornecidas.h"
#include "busca.h"

#define DEBUGGAR false

void logicaInsercao(FILE* file){
    // Pega valores guardados em TOPO e proxRRN
    int topo, proxRRN;

    fseek(file, 1, SEEK_SET);
    fread(&topo, sizeof(int), 1, file);
    fseek(file, 5, SEEK_SET);
    fread(&proxRRN, sizeof(int), 1, file);

    // Determina se o offset será a partir do proxRRN ou se existe algum registro logicamente removido que sera substituido
    int offset;

    if (topo != -1){
        offset = topo;
    } else {
        offset = proxRRN;
        proxRRN++;          // Se for a partir do proximo RRN, atualiza ele incrementando
    }

    // Encontra onde será a próxima inserção
    long byteOffset = 17 + (offset*80);
    fseek(file, byteOffset, SEEK_SET);

    // Se houver ponteiro para o próximo lógicamente removido, salva o valor dele na variavel prox
    int prox = -1;
    if (topo != -1){
        fseek(file, 1, SEEK_CUR);
        fread(&prox, sizeof(int), 1, file);
    }

    // Le a entrada para o registro temporário
    Registro temp;
    lerRegistro(&temp);

    // Reposiciona para a posição onde será a inserção
    fseek(file, byteOffset, SEEK_SET);
    char removido = '0';
    gravarRegistroBin(&temp, file, removido, -1);

    // Atualiza o topo do cabeçário e proxRRN
    fseek(file, 1, SEEK_SET);
    fwrite(&prox, sizeof(int), 1, file);
    fseek(file, 5, SEEK_SET);
    fwrite(&proxRRN, sizeof(int), 1, file);

    free(temp.nomeEstacao);
    free(temp.nomeLinha);

}

void insert(const char* nomeArquivoBin, int numeroLeituras){

    FILE* file = abrirVerificarInconsistentar(nomeArquivoBin);

    for (int i = 0; i < numeroLeituras; i++){
        logicaInsercao(file);
    }
    
    atualizarContadoresCabecalho(file);

    finalizarArquivo(file, DEBUGGAR);

    BinarioNaTela((char*)nomeArquivoBin);
}