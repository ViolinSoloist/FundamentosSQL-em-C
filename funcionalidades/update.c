#include "update.h"

#define DEBUGGAR false

static void callbackUpdate(FILE* file, int qtd_encontrados, long* offsets, void* dados_extras) {
    if (qtd_encontrados == 0 || offsets == NULL) return;

    OQueBuscar* oqmudar = (OQueBuscar*) dados_extras;
    long offset = *offsets; // sempre 1 por chamada

    Registro temp;
    fseek(file, offset + 1, SEEK_SET);
    binToStruct(&temp, file);

    // cascata de verificações: se algo estiver sendo buscado, faz receber o valor do que está sendo buscado
    if (oqmudar->checar_codEstacao)      
        temp.codEstacao = oqmudar->valores.codEstacao;

    if (oqmudar->checar_codLinha)        
        temp.codLinha = oqmudar->valores.codLinha;

    if (oqmudar->checar_codProxEstacao)  
        temp.codProxEstacao = oqmudar->valores.codProxEstacao;

    if (oqmudar->checar_distProxEstacao) 
        temp.distProxEstacao = oqmudar->valores.distProxEstacao;

    if (oqmudar->checar_codLinhaIntegra) 
        temp.codLinhaIntegra = oqmudar->valores.codLinhaIntegra;

    if (oqmudar->checar_codEstIntegra)   
        temp.codEstIntegra = oqmudar->valores.codEstIntegra;

    if (oqmudar->checar_nomeEstacao) {
        free(temp.nomeEstacao);
        temp.nomeEstacao = malloc(strlen(oqmudar->valores.nomeEstacao) + 1);
        strcpy(temp.nomeEstacao, oqmudar->valores.nomeEstacao);
    }
    if (oqmudar->checar_nomeLinha) {
        free(temp.nomeLinha);
        temp.nomeLinha = malloc(strlen(oqmudar->valores.nomeLinha) + 1);
        strcpy(temp.nomeLinha, oqmudar->valores.nomeLinha);
    }

    fseek(file, offset, SEEK_SET);
    gravarRegistroBin(&temp, file, temp.removido, temp.proximo);

    if (temp.nomeEstacao) free(temp.nomeEstacao);
    if (temp.nomeLinha)   free(temp.nomeLinha);
}

static void Le2LinhasBuscaEUpdate(FILE* file) {
    ArgumentosCallback args;
    
    int m; scanf("%d", &m);

    // preenche uma query do que está buscando
    preencherQuery(&args.query, m);

    int p; scanf("%d", &p);
    // preenche uma query que mostra para o que está se atualizando
    OQueBuscar oqmudar;
    preencherQuery(&oqmudar, p);

    // passa oqmudar como dados_extras — callback acessa via ponteiro
    args.dados_extras = &oqmudar;
    args.callback = callbackUpdate;
    args.offset_atual = OFFSET_INI_DADOS;

    percorreEBuscaCorrespondencia(file, &args);

    if (args.query.valores.nomeEstacao) free(args.query.valores.nomeEstacao);
    if (args.query.valores.nomeLinha) free(args.query.valores.nomeLinha);

    if (oqmudar.valores.nomeEstacao) free(oqmudar.valores.nomeEstacao);
    if (oqmudar.valores.nomeLinha) free(oqmudar.valores.nomeLinha);
}

void update(const char* nomeArquivoBin, int n) {
    FILE* file = abrirVerificarInconsistentar(nomeArquivoBin);

    for (int i=0; i<n; i++) 
        Le2LinhasBuscaEUpdate(file);

    atualizarContadoresCabecalho(file);
    finalizarArquivo(file, DEBUGGAR);
    BinarioNaTela((char*)nomeArquivoBin);
}