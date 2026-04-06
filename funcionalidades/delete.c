#include <stdio.h>
#include <stdlib.h>

#include "estruturas.h" 
#include "fornecidas.h"
#include "busca.h"
#include "delete.h"
#include "manipul_arq.h"

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

/// @private @brief pega uma linha de entrada (M nomeCampo1 valorCampo1 ...) preenche a query (registro OQueBuscar)
// e percorre o arquivo bin, salvando correspondência num vetor alocado de offsets
static void lerLinhaBuscaDeleta(FILE* file, int* topo)
{
    int m;
    scanf("%d", &m);

    OQueBuscar oqbuscar;

    // PREENCHIMENTO DA QUERY "CHECKLIST"
    preencherQuery(&oqbuscar, m);
    

    // chama função de busca para salvar correspondencias
    int qtd_encontrados = 0;
    long* offsets = percorreEBuscaCorrespondencia(file, oqbuscar, &qtd_encontrados); // lembrar de liberar memória depois de usar essa função

    // se achar correspondências, aplicar lógica dos ponteiros para deleção
    if (qtd_encontrados > 0 && offsets != NULL)
        logicaDelecao(file, &qtd_encontrados, topo, offsets);
    

    // corresponde à alocação dentro de marcadorFlag()
    if (oqbuscar.valores.nomeEstacao) free(oqbuscar.valores.nomeEstacao);
    if (oqbuscar.valores.nomeLinha) free(oqbuscar.valores.nomeLinha);
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
        lerLinhaBuscaDeleta(file, &topo);

    // escreve topo atualizado
    fseek(file, 1, SEEK_SET);
    fwrite(&topo, sizeof(int), 1, file);

    atualizarContadoresCabecalho(file);

    // se quiser debbugar antes de fechar arquivo, deixar como true lá no #define, no início do arquivo
    finalizarArquivo(file, DEBUGGAR);

    BinarioNaTela((char*)nomeArquivoBin); 
}