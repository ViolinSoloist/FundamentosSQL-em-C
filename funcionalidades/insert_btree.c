#include "insert_btree.h"

#define DEBUGGAR false

// --------------------------------------------------------------------------------------
// Função 1: Lida APENAS com a inserção no arquivo de dados binário (.bin)
// Retorna o byteOffset onde o registro foi gravado para ser usado na Árvore-B
// --------------------------------------------------------------------------------------
long logicaInsercaoBinario(FILE* file, Registro* temp) {
    int topo, proxRRN;

    // Lê o topo e o proxRRN do cabeçalho
    fseek(file, 1, SEEK_SET);
    fread(&topo, sizeof(int), 1, file);
    fseek(file, 5, SEEK_SET);
    fread(&proxRRN, sizeof(int), 1, file);

    // Determina o RRN de inserção
    int rrnInsercao;
    if (topo != -1){
        rrnInsercao = topo;
    } else {
        rrnInsercao = proxRRN;
        proxRRN++;          
    }

    // Calcula o offset exato do registro
    long byteOffset = 17 + ((long)rrnInsercao * 80);

    // Salva o próximo elemento da lista de removidos (se existir)
    int prox = -1;
    if (topo != -1){
        fseek(file, byteOffset + 1, SEEK_SET); // Pula o status '1' de removido no offset
        fread(&prox, sizeof(int), 1, file);
    }

    // Grava o novo registro no arquivo de dados no offset calculado
    fseek(file, byteOffset, SEEK_SET);
    char removido = '0';
    gravarRegistroBin(temp, file, removido, -1);

    // Atualiza o cabeçalho do arquivo de dados com o novo topo e proxRRN
    fseek(file, 1, SEEK_SET);
    fwrite(&prox, sizeof(int), 1, file);
    fseek(file, 5, SEEK_SET);
    fwrite(&proxRRN, sizeof(int), 1, file);

    // Retorna a posição (offset) para a Árvore-B
    return byteOffset; 
}

// --------------------------------------------------------------------------------------
// Função 2: Lida APENAS com a inserção no arquivo de índices da Árvore-B (.idx)
// --------------------------------------------------------------------------------------
void logicaInsercaoIndice(FILE* arv, CabecalhoArvore* cab, int chave, long byteOffset) {
    ArvoreInserir(arv, cab, chave, byteOffset);
}

// --------------------------------------------------------------------------------------
// Função 3: Função principal que une a leitura, gravação no binário e gravação no índice
// --------------------------------------------------------------------------------------
void insert_btree(const char* nomeArquivoBin, const char* nomeArquivoArvoreBin, int numeroLeituras){

    FILE* bin = abrirVerificarInconsistentar(nomeArquivoBin);
    FILE* arv = abrirVerificarInconsistentar(nomeArquivoArvoreBin);

    if (bin == NULL || arv == NULL){
        if (bin) fclose(bin);
        if (arv) fclose(arv);
        return;
    }

    CabecalhoArvore cab;
    lerCabecalhoArvore(arv, &cab);

    // IMPORTANTE: Limpa o '\n' que ficou no buffer do terminal quando o 'scanf' leu a quantidade de inserções
    getchar();

    for (int i = 0; i < numeroLeituras; i++){
        Registro temp;
        
        // 1. Lê os dados do teclado (apenas UMA vez por repetição)
        lerRegistro(&temp);
        
        // 2. Módulo de gravação de dados: Grava no .bin e pega o offset
        long byteOffset = logicaInsercaoBinario(bin, &temp);
        
        // 3. Módulo de indexação: Grava a chave e o offset na Árvore-B
        logicaInsercaoIndice(arv, &cab, temp.codEstacao, byteOffset);

        // Libera a memória alocada dinamicamente pela leitura das strings
        if (temp.nomeEstacao) free(temp.nomeEstacao);
        if (temp.nomeLinha) free(temp.nomeLinha);
    }
    
    // Finalizações do arquivo de dados
    atualizarContadoresCabecalho(bin);
    finalizarArquivo(bin, DEBUGGAR);

    // Finalizações da Árvore-B
    gravarCabecalhoArvore(arv, &cab);
    finalizarArquivo(arv, DEBUGGAR);

    // Exibe os hashes na tela para o caso de teste
    BinarioNaTela((char*)nomeArquivoBin);
    BinarioNaTela((char*)nomeArquivoArvoreBin);
}