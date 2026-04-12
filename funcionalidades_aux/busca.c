#include "busca.h"
#include <estruturas.h>

// define o tamanho maximo do nome de um campo
#define MAX_NOMECAMPO 69

/// ---------------- FUNÇÕES PRIVADAS AUXILIARES ----------------------

/**
 * recebe struct @param reg_lido (cópia dos dados do registro atualmente lido do bin) e o "checklist" @param query e compara pra ver se o registro atual
 * satisfaz as condições ou não
 */
static bool atendeCriterios(Registro reg_lido, OQueBuscar query)
{
    if (query.checar_codEstacao)
        if (reg_lido.codEstacao != query.valores.codEstacao)
            return false;

    if (query.checar_codEstIntegra)
        if (reg_lido.codEstIntegra != query.valores.codEstIntegra)
            return false;

    if (query.checar_codLinha)
        if (reg_lido.codLinha != query.valores.codLinha)
            return false;

    if (query.checar_codLinhaIntegra)
        if (reg_lido.codLinhaIntegra != query.valores.codLinhaIntegra)
            return false;

    if (query.checar_nomeEstacao)
        if (strcmp(reg_lido.nomeEstacao, query.valores.nomeEstacao) != 0)
            return false;

    if (query.checar_nomeLinha)
        if (strcmp(reg_lido.nomeLinha, query.valores.nomeLinha) != 0)
            return false;

    if (query.checar_codProxEstacao)
        if (reg_lido.codProxEstacao != query.valores.codProxEstacao)
            return false;

    if (query.checar_distProxEstacao)
        if (reg_lido.distProxEstacao != query.valores.distProxEstacao)
            return false;
    
    // só chega aqui se passar por todos os filtros de pesquisa
    return true; 
}

static void comparaECallback(FILE* bin, char removido, ArgumentosCallback* args)
{
    // registro não removido (logicamente)
    if (removido == '0') {            
        Registro reg_atual;
        binToStruct(&reg_atual, bin);

        // teste de match
        if (atendeCriterios(reg_atual, args->query)) {
            args->qntd_found++; 

            // CORREÇÃO: Usa args->callback e passa &args->offset_atual
            args->callback(bin, 1, &args->offset_atual, args->dados_extras);    

            fseek(bin, args->offset_atual + TAM_REGISTRO, SEEK_SET);
        }

        if (reg_atual.nomeEstacao) free(reg_atual.nomeEstacao);
        if (reg_atual.nomeLinha) free(reg_atual.nomeLinha);

    } else {
        fseek(bin, TAM_REGISTRO - 1, SEEK_CUR);
    }
    // ao final de cada loop, é necessário avançar o cursor em 1 registro
    args->offset_atual += TAM_REGISTRO;
}

//  --------------- FUNÇÕES PÚBLICAS (PRINCIPAIS) -----------------------------



// procedimento: cascata de if/elses par ver qual campo está sendo buscado (não é bonito mas eu acho que funciona)
// também pega a entrada do valor do campo que está sendo buscado para a struct de query
void marcadorFlag(char* nomeCampo, OQueBuscar* Query)
{
    //Char que sempre vai ler o valor para verificar se é nulo
    char valor[5];
    
    // se as strings forem iguais, strcmp retorna 0 ao em vez de 1, por isso o "!" em cada if
    if (!strcmp(nomeCampo, "codEstacao")) {
        Query->checar_codEstacao = true;
        // leitura do valor do que se buscar
        scanf("%d", &Query->valores.codEstacao);
    }
    else if (!strcmp(nomeCampo, "codLinha")) {
        Query->checar_codLinha = true;
        scanf("%s", valor);
        if (!strcmp(valor, "NULO"))
            Query->valores.codLinha = -1;
        else Query->valores.codLinha = atoi(valor);
    }
    else if (!strcmp(nomeCampo, "codLinhaIntegra")) {
        Query->checar_codLinhaIntegra = true;
        scanf("%s", valor);
        if (!strcmp(valor, "NULO"))
            Query->valores.codLinhaIntegra = -1;
        else Query->valores.codLinhaIntegra = atoi(valor);
    }
    else if (!strcmp(nomeCampo, "codEstIntegra")) {
        Query->checar_codEstIntegra = true;
        scanf("%s", valor);
        if (!strcmp(valor, "NULO"))
            Query->valores.codEstIntegra = -1;
        else Query->valores.codEstIntegra = atoi(valor);
    }
    else if (!strcmp(nomeCampo, "nomeEstacao")) {
        Query->checar_nomeEstacao = true;

        char bufferString[69];
        ScanQuoteString(bufferString);

        // aloca memória pro ponteiro da struct que representa nome da estacao
        // DEV NOTE: futuramente, se for necessário, trocar para vetor de string estática, se tiver certeza que os nomes tem um limite baixo de tamanho
        Query->valores.nomeEstacao = malloc(strlen(bufferString) + 1);
        strcpy(Query->valores.nomeEstacao, bufferString);
    }
    else if (!strcmp(nomeCampo, "nomeLinha")) {
        Query->checar_nomeLinha = true;

        char bufferString[69];
        ScanQuoteString(bufferString);

        // aloca memória pro ponteiro da struct que representa nome da linha
        Query->valores.nomeLinha = malloc(strlen(bufferString) + 1);
        strcpy(Query->valores.nomeLinha, bufferString);
    }
    else if (!strcmp(nomeCampo, "distProxEstacao")){
        Query->checar_distProxEstacao = true;
        scanf("%s", valor);
        if (!strcmp(valor, "NULO"))
            Query->valores.distProxEstacao = -1;
        else Query->valores.distProxEstacao = atoi(valor);
    }
    else if (!strcmp(nomeCampo, "codProxEstacao")){
        Query->checar_codProxEstacao = true;
        scanf("%s", valor);
        if (!strcmp(valor, "NULO"))
            Query->valores.codProxEstacao = -1;
        else Query->valores.codProxEstacao = atoi(valor);
    }
}

/// @brief zera todos os indicadores de busca antes de ler a entrada e marcar os campos que precisam ser buscados com true
void zerarFlags(OQueBuscar* query)
{
    query->checar_codEstacao = false;
    query->checar_codLinha = false;
    query->checar_codLinhaIntegra = false;
    query->checar_codEstIntegra = false;
    query->checar_nomeEstacao = false;
    query->checar_nomeLinha = false;
    query->checar_codProxEstacao = false;
    query->checar_distProxEstacao = false;
    
    query->valores.nomeEstacao = NULL;
    query->valores.nomeLinha = NULL;
}

/// @attention lembrar de desalocar memória  
void preencherQuery(OQueBuscar* oqbuscar, int m) {
    // zera todas as flags antes de marcar as que vao ser buscadas como true
    zerarFlags(oqbuscar);
    
    // loop que que pega todos os nomes de campo de entrada e também lê o valor dos campos a serem buscados, para preencher a "checklist"
    for (int j=0; j<m; j++) {
        char nomeCampo[MAX_NOMECAMPO];
        scanf("%s", nomeCampo);
        marcadorFlag(nomeCampo, oqbuscar); // LEMBRAR DE LIBERAR ALOCAÇÃO PARA OS CAMPOS DE NOMES
    }
}

/**
 * @brief função genérica de busca, que ao achar correspondência, faz um callback para funcinoalidade correspondente
 * @param query é a struct que serve para marcar quais campos estão sendo procurados, além dos valores correspondentes
 */
void percorreEBuscaCorrespondencia(FILE* bin, ArgumentosCallback* args) {
    args->qntd_found = 0;

    // pula pro início dos dados
    fseek(bin, OFFSET_INI_DADOS, SEEK_SET);

    // tomamos conta do cursor para evitar uso desnecessário de ftell()
    args->offset_atual = OFFSET_INI_DADOS;

    // enquanto ainda conseguir ler o campo removido dos registros
    char removido;
    while (fread(&removido, sizeof(char), 1, bin) == 1) {comparaECallback(bin, removido, args);}
    
    if (args->qntd_found == 0)
        args->callback(bin, 0, NULL, args->dados_extras);
}

