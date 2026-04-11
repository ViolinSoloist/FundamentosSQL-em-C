#include "delete.h"

#define DEBUGGAR false

// ----------------- FUNÇÕES AUXILIARES/PRIVADAS --------------------------

/// @private @brief aplica lógica de ponteiros para deleção (topo e proxRRN)
static void logicaDelecao(FILE* file, int* topo, long* offsets)
{
    long posicao_registro = *offsets;

    int rrn_atual = (posicao_registro - 17) / 80;

    // pula para o byte inicial de um dos registros que satisfez a busca
    fseek(file, posicao_registro, SEEK_SET);

    // deleção lógica (marca como deletado)
    char marca = '1';
    fwrite(&marca, sizeof(char), 1, file);

    // grava o topo do cabeçalho no próximo 
    fwrite(topo, sizeof(int), 1, file);

    // atualiza o topo para rrn atual
    *topo = rrn_atual; 
}

/// @private @brief transforma a função de deleção lógica em uma função callback aceitável para função de leitura
/// @param file 
/// @param qtd_encontrados 
/// @param offsets 
/// @param dados_extras 
static void wrapperLogicaDelecao(FILE* file, int qtd_encontrados, long* offsets, void* dados_extras) {
    if (qtd_encontrados == 0 || offsets == NULL) return;

    // Converte o ponteiro genérico de volta para o tipo correto (int*)
    int* topo = (int*) dados_extras;

    // Chama a lógica de deleção original
    logicaDelecao(file, topo, offsets);
}

// -------------------- FIM FUNÇÕES PRIVADAS ------------------------------------------


/// @public função principal de deleção
void deletar(const char *nomeArquivoBin, int n) {
    
    // rotina ao abrir arquivo binário rb+
    FILE* file = abrirVerificarInconsistentar(nomeArquivoBin);
    if (file == NULL) return;

    // salva o que está no topo (necessário para deleção)
    int topo;
    fseek(file, 1, SEEK_SET);
    fread(&topo, sizeof(int), 1, file);
    
    for (int i=0; i<n; i++) 
        lerLinhaBusca(file, wrapperLogicaDelecao, &topo);

    // escreve topo atualizado
    fseek(file, 1, SEEK_SET);
    fwrite(&topo, sizeof(int), 1, file);

    atualizarContadoresCabecalho(file);

    // se quiser debbugar antes de fechar arquivo, deixar como true lá no #define, no início do arquivo
    finalizarArquivo(file, DEBUGGAR);

    BinarioNaTela((char*)nomeArquivoBin); 
}