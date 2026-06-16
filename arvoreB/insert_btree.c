#include "insert_btree.h"
#include "manipul_arq.h"
#include "terminal.h"
#include "serial.h"

static void logicaInsercaoBTree(FILE* fileDados, FILE* fileIndice, CabecalhoArvore* cabIndice) {
    Registro temp;
    lerRegistro(&temp); 

    // encontrou na arvore => ATIVO => aborta a inserção (evita de quebrar a pilha)
    if (ArvoreBuscar(fileIndice, cabIndice, temp.codEstacao) != -1) {
        free(temp.nomeEstacao);
        free(temp.nomeLinha);
        return; 
    }

    // a partir daqui, a chave é nova OU o registro antigo estava removido.
    int topo, proxRRN;
    fseek(fileDados, 1, SEEK_SET);
    fread(&topo, sizeof(int), 1, fileDados);
    fseek(fileDados, 5, SEEK_SET);
    fread(&proxRRN, sizeof(int), 1, fileDados);

    int rrn_inserido; 
    int prox = -1;
    
    if (topo != -1) {
        rrn_inserido = topo; // reaproveita espaço da pilha
        long byteOffset = 17 + (rrn_inserido * 80);
        fseek(fileDados, byteOffset + 1, SEEK_SET); // +1 para pular o char 'removido'
        fread(&prox, sizeof(int), 1, fileDados);
    } else {
        rrn_inserido = proxRRN; 
        proxRRN++;        
    }

    // gravação física no .bin
    long byteOffset = 17 + (rrn_inserido * 80);
    fseek(fileDados, byteOffset, SEEK_SET);
    char removido = '0';
    gravarRegistroBin(&temp, fileDados, removido, -1);

    // atualiza cabeçalho do .bin
    fseek(fileDados, 1, SEEK_SET);
    fwrite(&prox, sizeof(int), 1, fileDados);
    fseek(fileDados, 5, SEEK_SET);
    fwrite(&proxRRN, sizeof(int), 1, fileDados);

    // ---------- ARVORE B ----------------------
    // * após inserir fisicamente => pegar chave (codEstacao) e o local onde está salvo (rrn_inserido) e "avisa" o índice.
    
    ArvoreInserir(fileIndice, cabIndice, temp.codEstacao, byteOffset); /// @attention A IMPLEMENTAR AINDA

    if (temp.nomeEstacao) free(temp.nomeEstacao);
    if (temp.nomeLinha) free(temp.nomeLinha);
}

/// @brief função principal que será chamada na main
void insert_btree(const char* nomeArquivoDados, const char* nomeArquivoIndice, int n) {

    FILE* fileDados = rotinaAbrirArquivo(nomeArquivoDados, ESCRITA); 
    if (fileDados == NULL) {
        finalizarArquivoEscrita(fileDados, false);
        return;
    }

    FILE* fileIndice = rotinaAbrirArquivo(nomeArquivoIndice, ESCRITA); 
    if (fileIndice == NULL) {
        finalizarArquivoEscrita(fileDados, false); // fecha de dados se o índice falhar
        return;
    }

    CabecalhoArvore cabIndice; // lê o cabeçalho da árvore para atualizar
    lerCabecalhoArvore(fileIndice, &cabIndice);

    for(int i = 0; i < n; i++) logicaInsercaoBTree(fileDados, fileIndice, &cabIndice);

    atualizarContadoresCabecalho(fileDados);

    // finalização (dos arquivos)
    finalizarArquivoEscrita(fileDados, false); /// @attention usar essa função se o arquivo foi aberto como ESCRITA

    cabIndice.status = '1'; // só pra ter certeza
    gravarCabecalhoArvore(fileIndice, &cabIndice);
    fclose(fileIndice);

    BinarioNaTela((char*)nomeArquivoDados);
    BinarioNaTela((char*)nomeArquivoIndice);

}