#include <stdio.h>
#include <stdlib.h>

#include "IO.h"
#include "estruturas.h"
#include "busca.h"

void mostrarRegistro(Registro* registro_lido){
    
    printf("%d ", registro_lido->codEstacao);
    printf("%s ", registro_lido->nomeEstacao);

    if (registro_lido->codLinha == -1) printf("NULO ");
    else printf("%d ", registro_lido->codLinha);

    if (registro_lido->tanNomeLinha > 0 && registro_lido->nomeLinha != NULL)
        printf("%s ", registro_lido->nomeLinha);
    else printf("NULO ");

    if (registro_lido->codProxEstacao == -1) printf("NULO ");
    else printf("%d ", registro_lido->codProxEstacao);

    if (registro_lido->distProxEstacao == -1) printf("NULO ");
    else printf("%d ", registro_lido->distProxEstacao);

    if (registro_lido->codLinhaIntegra == -1) printf("NULO ");
    else printf("%d ", registro_lido->codLinhaIntegra);

    if (registro_lido->codEstIntegra == -1) printf("NULO\n");
    else printf("%d\n", registro_lido->codEstIntegra);
}

void lerLinhaBusca(FILE* file, AcaoPosBusca callback, void* dados_extras){
    int m;
    scanf("%d", &m);

    OQueBuscar oqBuscar;
    
    // PREENCHIMENTO DA QUERY "CHECKLIST"
    preencherQuery(&oqBuscar, m);

    // chama função de busca para salvar correspondencias
    int qtd_encontrados = 0;
    long* offsets = percorreEBuscaCorrespondencia(file, oqBuscar, &qtd_encontrados); // lembrar de liberar memória depois de usar essa função

    // se achar correspondências, aplicar a função callback passando os dados encontrados
    if (callback != NULL)
        callback(file, qtd_encontrados, offsets, dados_extras);
        
    // limpa memória alocada para vetor offsets
    free(offsets);

    // corresponde à alocação dentro de marcadorFlag()
    if (oqBuscar.valores.nomeEstacao) free(oqBuscar.valores.nomeEstacao);
    if (oqBuscar.valores.nomeLinha) free(oqBuscar.valores.nomeLinha);
}