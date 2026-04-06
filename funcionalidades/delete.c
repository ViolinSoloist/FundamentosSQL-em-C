#include <stdio.h>
#include <stdlib.h>

#include "estruturas.h" 
#include "fornecidas.h"
#include "busca.h"
#include "delete.h"
#include "manipul_arq.h"
#include "IO.h"

#define DEBUGGAR false

// ----------------- FUNÇÕES AUXILIARES/PRIVADAS --------------------------

/// @private @brief aplica lógica de ponteiros para deleção (topo e proxRRN)
static void logicaDelecao(FILE* file, int* qtd_encontrados, int* topo, long* offsets)
{
    // itera por todos os offsets que satisfizeram a busca
    for (int k=0; k<(*qtd_encontrados); k++)
    {
        long posicao_registro = offsets[k];

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
    // alocação dentro de percorreEBuscaCorrespondencia
    free(offsets); 
}

/// @private @brief transforma a função de deleção lógica em uma função callback aceitável para função de leitura
/// @param file 
/// @param qtd_encontrados 
/// @param offsets 
/// @param dados_extras 
void wrapperLogicaDelecao(FILE* file, int qtd_encontrados, long* offsets, void* dados_extras) {
    if (qtd_encontrados == 0 || offsets == NULL) return;

    // Converte o ponteiro genérico de volta para o tipo correto (int*)
    int* topo = (int*) dados_extras;

    // Chama a lógica de deleção original
    logicaDelecao(file, &qtd_encontrados, topo, offsets);
}

// -------------------- FIM FUNÇÕES PRIVADAS ------------------------------------------


/// @public função principal de deleção
void deletar(const char *nomeArquivoBin, int n) {
    
    // rotina ao abrir arquivo binário rb+
    FILE* file = abrirVerificarInconsistentar(nomeArquivoBin);

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