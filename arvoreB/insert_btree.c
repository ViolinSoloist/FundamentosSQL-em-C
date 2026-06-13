#include "insert_btree.h"
#include "arvoreB.h"
#include "manipul_arq.h"
#include "terminal.h"
#include "serial.h"

static void logicaInsercaoBTree(FILE* fileDados, FILE* fileIndice, CabecalhoArvore* cabIndice) {
    // pega valores guardados (TOPO e proxRRN) do arquivo de dados
    int topo, proxRRN;
    fseek(fileDados, 1, SEEK_SET);
    fread(&topo, sizeof(int), 1, fileDados);
    fseek(fileDados, 5, SEEK_SET);
    fread(&proxRRN, sizeof(int), 1, fileDados);

    // determina em qual RRN o novo registro será escrito
    int rrn_inserido; 
    if (topo != -1) {rrn_inserido = topo;} // reutiliza espaço vazio, se der
    else {
        rrn_inserido = proxRRN; 
        proxRRN++;        
    }

    // encontra o byte exato no arquivo de dados
    long byteOffset = 17 + (rrn_inserido * 80);
    fseek(fileDados, byteOffset, SEEK_SET);

    // if (reaproveitando espaço, ou topo != -1) { guardamos o próximo removido }
    int prox = -1;
    if (topo != -1){
        fseek(fileDados, 1, SEEK_CUR);
        fread(&prox, sizeof(int), 1, fileDados);
    }

    // le novo registro (user input)
    Registro temp;
    lerRegistro(&temp); 

    // reposiciona cursos e grava
    fseek(fileDados, byteOffset, SEEK_SET);
    char removido = '0';
    gravarRegistroBin(&temp, fileDados, removido, -1);

    // atualiza cabeçalho de dados
    fseek(fileDados, 1, SEEK_SET);
    fwrite(&prox, sizeof(int), 1, fileDados);
    fseek(fileDados, 5, SEEK_SET);
    fwrite(&proxRRN, sizeof(int), 1, fileDados);

    // ---------- ARVORE B ----------------------
    // * após inserir fisicamente => pegar chave (codEstacao) e o local onde está salvo (rrn_inserido) e "avisa" o índice.
    
    ArvoreInserir(fileIndice, cabIndice, temp.codEstacao, rrn_inserido); /// @attention A IMPLEMENTAR AINDA

    free(temp.nomeEstacao);
    free(temp.nomeLinha);
}

/// @brief função principal que será chamada na main
void insert_btree(const char* nomeArquivoDados, const char* nomeArquivoIndice, int n) {

    FILE* fileDados = rotinaAbrirArquivo(nomeArquivoDados, ESCRITA); 
    if (fileDados == NULL) return;

    FILE* fileIndice = rotinaAbrirArquivo(nomeArquivoIndice, ESCRITA); 
    if (fileIndice == NULL) {
        finalizarArquivo(fileDados, false); // fecha de dados se o índice falhar
        return;
    }

    CabecalhoArvore cabIndice; // lê o cabeçalho da árvore para atualizar
    lerCabecalhoArvore(fileIndice, &cabIndice);

    for(int i = 0; i < n; i++) logicaInsercaoBTree(fileDados, fileIndice, &cabIndice);

    // finalização (dos arquivos)
    gravarCabecalhoArvore(fileIndice, &cabIndice);
    finalizarArquivo(fileDados, false); /// @attention usar essa função se o arquivo foi aberto como ESCRITA
    fclose(fileIndice); 
}