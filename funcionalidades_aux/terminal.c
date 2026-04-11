#include "terminal.h"


// Função para ler diretamente do terminal para um registro temporário
void lerRegistro(Registro* temp){
    char buffer[200]; // Buffer temporário para ler qualquer campo

    // codEstacao
    ScanQuoteString(buffer);
    temp->codEstacao = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);

    // nomeEstacao
    ScanQuoteString(buffer);
    temp->nomeEstacao = malloc(strlen(buffer) + 1);
    strcpy(temp->nomeEstacao, buffer);

    // codLinha
    ScanQuoteString(buffer);
    temp->codLinha = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);

    // nomeLinha
    ScanQuoteString(buffer);
    temp->nomeLinha = malloc(strlen(buffer) + 1);
    strcpy(temp->nomeLinha, buffer);

    // codProxEstacao
    ScanQuoteString(buffer);
    temp->codProxEstacao = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);

    // distProxEstacao
    ScanQuoteString(buffer);
    temp->distProxEstacao = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);

    // codLinhaIntegra
    ScanQuoteString(buffer);
    temp->codLinhaIntegra = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);

    // codEstIntegra
    ScanQuoteString(buffer);
    temp->codEstIntegra = (strcmp(buffer, "") == 0) ? -1 : atoi(buffer);
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

void lerLinhaNormal(FILE* file, char* linha){
    fgets(linha, sizeof(linha), file);

    // tratamento de quebra de linhas (windows, linux)
    linha[strcspn(linha, "\n")] = '\0';
    linha[strcspn(linha, "\r")] = '\0';

    // tratamento linah "fantasma"
    if (strlen(linha) == 0) return;
}
