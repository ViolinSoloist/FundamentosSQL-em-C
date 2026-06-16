/// @author Erik

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "busca.h"
#include "estruturas.h"
#include "arvoreB.h"
#include "manipul_arq.h"
#include "terminal.h"

typedef enum {
    ESTACOES,
    PARES,
} tipoContavel; // basicamente pra indicar os dois tipo possíveis de contaveis que existem

// struct que contém variáveis que servem de parâmetro para as funções auxilizares de CONTAGEM (atualização do nroEstacoes e nroPares)
typedef struct {
    Registro* ativos;
    tipoContavel tipo;
    int i;
} paramFuncContador;

/// @brief função auxiliar que faz a contagem de um dado contável (nroEstacoes ou nroParesEstacoes) 
static void contador(paramFuncContador* args, int* contavel) {
    
    bool repetido = false;

    for (int j=0; j<(args->i); j++) {
        /*
        * VERIFICAÇÃO DE DUPLICIDADE 
        * 
        * checa se o registro atual (i) é duplicata do registro de ntes (j)
        * ESTACOES:
        *  nome da estação no j não pode ser nulo
        *  compara (strcmp) o nome da estação i com o nome da estação 'j. Se forem iguais é repetido.
        *
        * PARES:
        *  checa se codEstacao i é igual ao de j
        *  checa se o codProxEstacao i também é igual ao de j
        *   se os dois forem verdade, o par é repetido.
        */
        bool condicaoRepetido = (args->tipo == ESTACOES ?
        (args->ativos)[j].nomeEstacao != NULL && strcmp((args->ativos)[args->i].nomeEstacao, args->ativos[j].nomeEstacao) == 0 :
        (args->ativos)[args->i].codEstacao == (args->ativos)[j].codEstacao && (args->ativos)[args->i].codProxEstacao == (args->ativos)[j].codProxEstacao
        );

        if (condicaoRepetido) { repetido = true; break; }
    }

    if (!repetido) (*contavel)++;
}

/// @related auxiliar de recalcularContadores
static void recontagem(paramFuncContador* args, Par* contadores) {

    // estacoes únicas
    if ((args->ativos)[args->i].nomeEstacao != NULL && strlen((args->ativos)[args->i].nomeEstacao) > 0) {
        args->tipo = ESTACOES;
        contador(args, &(contadores->origem));
    }
    
    // pares únicos
    if ((args->ativos)[args->i].codEstacao != -1 && (args->ativos)[args->i].codProxEstacao != -1) {
        args->tipo = PARES;
        contador(args, &(contadores->destino));
    }
}

/// @details após todas as remoções lógicas, precisamos recontar as estações únicas e pares únicos
/// @param fileDados 
static void recalcularContadores(FILE* fileDados) {
    fseek(fileDados, 17, SEEK_SET); // Pula o cabeçalho
    Registro* ativos = NULL;
    int numAtivos = 0;
    char removido;
    
    // itera pelo arquivo e salva apenas os registros não removidos
    while (fread(&removido, sizeof(char), 1, fileDados) == 1) {
        Registro reg;
        binToStruct(&reg, fileDados);
        
        if (removido == '0') {  // SE NÃO REMOVIDO
            ativos = realloc(ativos, (numAtivos + 1) * sizeof(Registro));
            ativos[numAtivos] = reg;
            numAtivos++;
        } else {                // SE REMOVIDO
            if (reg.nomeEstacao) free(reg.nomeEstacao);
            if (reg.nomeLinha) free(reg.nomeLinha);
        }
    }
    
    // preenchimento dos parâmetros das funções de contagem
    paramFuncContador args; args.ativos = ativos;
    Par contadores; // nroEstacoes, nroParesEstacoes
    contadores.origem = 0; contadores.destino = 0;

    for (int i=0; i<numAtivos; i++) { // loop em que ocorre a recontagem
        args.i = i;
        recontagem(&args, &contadores);
    }

    // gravação cabeçalho: nroEstacoes (offset 9) nroParesEstacoes (offset13)
    fseek(fileDados, 9, SEEK_SET);
    fwrite(&contadores.origem, sizeof(int), 1, fileDados); // origem = nroEstacoes
    fwrite(&contadores.destino, sizeof(int), 1, fileDados); // destino = nroParesEstacoes
    
    // desalocação de memória antes de terminar a função
    for (int i=0; i<numAtivos; i++) {
        if (ativos[i].nomeEstacao) free(ativos[i].nomeEstacao);
        if (ativos[i].nomeLinha) free(ativos[i].nomeLinha);
    }
    if (ativos) free(ativos);
}


/// @brief FUNÇÕES DE FILTRO E EXECUÇÃO
static bool registroPassaFiltro(Registro* reg, OQueBuscar* query) { 

    if (query->checar_codEstacao && reg->codEstacao != query->valores.codEstacao) return false;
    if (query->checar_codLinha && reg->codLinha != query->valores.codLinha) return false;
    if (query->checar_codProxEstacao && reg->codProxEstacao != query->valores.codProxEstacao) return false;
    if (query->checar_distProxEstacao && reg->distProxEstacao != query->valores.distProxEstacao) return false;
    if (query->checar_codLinhaIntegra && reg->codLinhaIntegra != query->valores.codLinhaIntegra) return false;
    if (query->checar_codEstIntegra && reg->codEstIntegra != query->valores.codEstIntegra) return false;
    
    if (query->checar_nomeEstacao) {
        if (reg->nomeEstacao == NULL || strcmp(reg->nomeEstacao, query->valores.nomeEstacao) != 0) return false;
    }
    if (query->checar_nomeLinha) {
        if (reg->nomeLinha == NULL || strcmp(reg->nomeLinha, query->valores.nomeLinha) != 0) return false;
    }
    return true;
}

// ---------------- FUNÇÕES RELACIONADAS Á BUSCA PARA PODER DELETAR -----------------------

// struct com parâmetros usados nessas funções
typedef struct {
    FILE* fileDados;
    FILE* fileIndice;
    CabecalhoArvore* cabIndice;
    OQueBuscar* query;
} ParamBuscaDelecao;

/// @brief se o que está sendo buscado é a chave (codEstacao), usa-se busca em árvore-B
static void buscaUsaArvoreRemove(ParamBuscaDelecao* args) {
    
    long byteOffset = ArvoreBuscar(args->fileIndice, args->cabIndice, args->query->valores.codEstacao);
        
    if (byteOffset != -1) { 
        fseek(args->fileDados, byteOffset, SEEK_SET);
        char removido;
        fread(&removido, sizeof(char), 1, args->fileDados);
        
        Registro reg;
        binToStruct(&reg, args->fileDados); 
        
        if (removido == '0' && registroPassaFiltro(&reg, args->query)) {
            
            // convete pra RRN pra empilhar no topo
            int rrn_dados = (byteOffset - 17) / 80;

            int topo_dados;
            fseek(args->fileDados, 1, SEEK_SET);
            fread(&topo_dados, sizeof(int), 1, args->fileDados);
            
            fseek(args->fileDados, byteOffset, SEEK_SET);
            char marcaRemovido = '1';
            fwrite(&marcaRemovido, sizeof(char), 1, args->fileDados);
            fwrite(&topo_dados, sizeof(int), 1, args->fileDados);
            
            fseek(args->fileDados, 1, SEEK_SET);
            fwrite(&rrn_dados, sizeof(int), 1, args->fileDados);
            
            // remoção na árbore (boa sorte)
            ArvoreRemover(args->fileIndice, args->cabIndice, reg.codEstacao);
        }
        
        if (reg.nomeEstacao) { free(reg.nomeEstacao); reg.nomeEstacao = NULL; }
        if (reg.nomeLinha) { free(reg.nomeLinha); reg.nomeLinha = NULL; }
    }
}

// qualquer outro registro que não seja codEstacao é buscado de maneira "normal", sequencial, pelo arquivo
static void buscaNormalRemove(ParamBuscaDelecao* args) {

    fseek(args->fileDados, 17, SEEK_SET);
    int rrn_atual = 0;
    char removido;
    Registro reg;
    
    while (fread(&removido, sizeof(char), 1, args->fileDados) == 1) {
        binToStruct(&reg, args->fileDados); 
        
        if (removido == '0' && registroPassaFiltro(&reg, args->query)) {
            int topo_dados;
            fseek(args->fileDados, 1, SEEK_SET);
            fread(&topo_dados, sizeof(int), 1, args->fileDados);
            
            long byteOffset = 17 + (rrn_atual * 80);
            fseek(args->fileDados, byteOffset, SEEK_SET);
            char marcaRemovido = '1';
            fwrite(&marcaRemovido, sizeof(char), 1, args->fileDados);
            fwrite(&topo_dados, sizeof(int), 1, args->fileDados);
            
            fseek(args->fileDados, 1, SEEK_SET);
            fwrite(&rrn_atual, sizeof(int), 1, args->fileDados);
            
            // usando arvore ou não pra achar, se você removeu tem que tirar da arvore também
            ArvoreRemover(args->fileIndice, args->cabIndice, reg.codEstacao);
        }
        
        if (reg.nomeEstacao) { free(reg.nomeEstacao); reg.nomeEstacao = NULL;} // Remoção Física na Árvore-B
        if (reg.nomeLinha) { free(reg.nomeLinha); reg.nomeLinha = NULL; }
        
        rrn_atual++;
        fseek(args->fileDados, 17 + (rrn_atual * 80), SEEK_SET); 
    }
}

/// @brief função manual reponsável por manualmente preencher a query de busca, para contornar bug do scanquotestring
static void getQueryManualmente(OQueBuscar* query) {
    char nomeCampo[67];
    scanf("%s", nomeCampo);

    if (strcmp(nomeCampo, "codEstacao") == 0) {
        query->checar_codEstacao = true;
        scanf("%d", &query->valores.codEstacao);
    } else if (strcmp(nomeCampo, "codLinha") == 0) {
        query->checar_codLinha = true;
        scanf("%d", &query->valores.codLinha);
    } else if (strcmp(nomeCampo, "codProxEstacao") == 0) {
        query->checar_codProxEstacao = true;
        scanf("%d", &query->valores.codProxEstacao);
    } else if (strcmp(nomeCampo, "distProxEstacao") == 0) {
        query->checar_distProxEstacao = true;
        scanf("%d", &query->valores.distProxEstacao);
    } else if (strcmp(nomeCampo, "codLinhaIntegra") == 0) {
        query->checar_codLinhaIntegra = true;
        scanf("%d", &query->valores.codLinhaIntegra);
    } else if (strcmp(nomeCampo, "codEstIntegra") == 0) {
        query->checar_codEstIntegra = true;
        scanf("%d", &query->valores.codEstIntegra);
    } else if (strcmp(nomeCampo, "nomeEstacao") == 0) {
        query->checar_nomeEstacao = true;
        char buffer[200];
        ScanQuoteString(buffer);
        query->valores.nomeEstacao = malloc(strlen(buffer) + 1);
        strcpy(query->valores.nomeEstacao, buffer);
    } else if (strcmp(nomeCampo, "nomeLinha") == 0) {
        query->checar_nomeLinha = true;
        char buffer[200];
        ScanQuoteString(buffer);
        query->valores.nomeLinha = malloc(strlen(buffer) + 1);
        strcpy(query->valores.nomeLinha, buffer);
    }
}

/// @brief FUNÇÃO PRINCIPAL
/// @param nomeArquivoDados 
/// @param nomeArquivoIndice 
/// @param n 
void delete_btree(const char* nomeArquivoDados, const char* nomeArquivoIndice, int n) {
    
    FILE* fileDados = rotinaAbrirArquivo(nomeArquivoDados, ESCRITA);
    if (fileDados == NULL) return;

    FILE* fileIndice = rotinaAbrirArquivo(nomeArquivoIndice, ESCRITA);
    if (fileIndice == NULL) { finalizarArquivoEscrita(fileDados, false); return;}

    CabecalhoArvore cabIndice;
    lerCabecalhoArvore(fileIndice, &cabIndice);

    ParamBuscaDelecao args; // parametros das funções que organizam o tipo busca (para remoção, não confundir com a funcionalidade de busca em si)
    for (int i=0; i<n; i++) {
        OQueBuscar query;
        memset(&query, 0, sizeof(OQueBuscar));
        
        int m;
        scanf("%d", &m); 
        
        for (int j=0; j<m; j++) getQueryManualmente(&query); // função ScanQuoteString bugada? => checkagem manual
        
        // ---------- EXECUTA BUSCA SEQUENCIAL/ARVORE DO REGISTRO A SER REMOVIDO E REMOVE ------------------
        args.fileDados = fileDados;     args.fileIndice = fileIndice;   args.cabIndice = &cabIndice;      args.query = &query;
        
        if (query.checar_codEstacao) buscaUsaArvoreRemove(&args); // busca chave (codEstacao)? => Usa Arvore, essa função também chama a remoção em arvore

        else buscaNormalRemove(&args); // qualquer outra coisa sendo buscada? usa iteração pelo arquivo normal, também chama remoção em arvore (chega de árvore plmds)

        // liberação de memória
        if (query.checar_nomeEstacao && query.valores.nomeEstacao) free(query.valores.nomeEstacao);
        if (query.checar_nomeLinha && query.valores.nomeLinha) free(query.valores.nomeLinha);
    }

    recalcularContadores(fileDados);

    finalizarArquivoEscrita(fileDados, false);
    
    cabIndice.status = '1';
    gravarCabecalhoArvore(fileIndice, &cabIndice);
    fclose(fileIndice);

    BinarioNaTela((char*)nomeArquivoDados);
    BinarioNaTela((char*)nomeArquivoIndice);
}