#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "estruturas.h" 
#include "fornecidas.h"

// define o tamanho do vetor de strings que guarda estações já vistas (A MUDAR/REMOVER DEPENDENDO DOS TESTES DO RUNCODES)
#define CONTADOR_MAX 1000
#define MAX_NOMECAMPO 69


/// ---------------- FUNÇÕES PRIVADAS AUXILIARES ----------------------

/**
 * @private funcao auxiliar
 * recebe struct @param reg_lido (cópia dos dados do registro atualmente lido do bin) e o "checklist" @param query e compara pra ver se o registro atual
 * satisfaz as condições ou não
 */
bool atendeCriterios(Registro reg_lido, OQueBuscar query)
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


//  --------------- FUNÇÕES PÚBLICAS (PRINCIPAIS) -----------------------------

/**
 * @private função auxiliar, le o registro atual do arq bin e copia os dados pra struct Registro
 * recebe ponteiro para @param regAtual e o arquivo @param bin
 */
void binToStruct(Registro* regAtual, FILE* bin) {
    regAtual->removido = '0';

    // leitura pra struct Registro dos campos fixos (28 bytes: 7 * 4 )
    fread(&regAtual->proximo, sizeof(int), 1, bin);
    fread(&regAtual->codEstacao, sizeof(int), 1, bin);
    fread(&regAtual->codLinha, sizeof(int), 1, bin);
    fread(&regAtual->codProxEstacao, sizeof(int), 1, bin);
    fread(&regAtual->distProxEstacao, sizeof(int), 1, bin);
    fread(&regAtual->codLinhaIntegra, sizeof(int), 1, bin);
    fread(&regAtual->codEstIntegra, sizeof(int), 1, bin);
    
    int bytes_lidos = 29; // 1 char  + 7*4

    // acho que isso dá pra usar vetor estático, pra evitar malloc
    static char bufferEstacao[69] = ""; 
    static char bufferLinha[69] = "";

    fread(&regAtual->tamNomeEstacao, sizeof(int), 1, bin);
    bytes_lidos += 4;

    // se (tamanho > 0) => existe nome
    if (regAtual->tamNomeEstacao > 0) {
        fread(bufferEstacao, sizeof(char), regAtual->tamNomeEstacao, bin);
        bufferEstacao[regAtual->tamNomeEstacao] = '\0';
        bytes_lidos += regAtual->tamNomeEstacao;
    }
    regAtual->nomeEstacao = bufferEstacao; // aponta nomeEstacao da struct para o buffer local, em vez de alocar dinamicamente

    fread(&regAtual->tanNomeLinha, sizeof(int), 1, bin);
    bytes_lidos += 4;
    if (regAtual->tanNomeLinha > 0) {
        fread(bufferLinha, sizeof(char), regAtual->tanNomeLinha, bin);
        bufferLinha[regAtual->tanNomeLinha] = '\0';
        bytes_lidos += regAtual->tanNomeLinha;
    }
    regAtual->nomeLinha = bufferLinha;
    
    // pular lixo
    fseek(bin, 80 - bytes_lidos, SEEK_CUR);
}


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
 * @brief percorre o arquivo bin e retorna um ponteiro para vetor alocado de offsets que deram "match" nos parâmetros de pesquisa
 * @param query é a struct que serve para marcar quais campos estão sendo procurados, além dos valores correspondentes
 */
long* percorreEBuscaCorrespondencia(FILE* bin, OQueBuscar query, int* qntd_found) {
    *qntd_found = 0;

    // meio que um chute de quantas correspondencias vai achar. Uma gambiarra que mistura lista dinâmica com lista est\ática, para evitar tamanho fixo rígido mas para diminuir quantidade de realloc
    // DEV NOTE: eu não sei se no runcodes vai ter um csv com mais de 200 linhas, mas se for só isso mesmo usar buffer estático
    int capacidade_atual = 10; // futuramente, se necessário, aumentado ao dobro com realloc (mas poucas vezes, por favor não brigar comigo)
    long* vetor_offsets = malloc(sizeof(long) * capacidade_atual); // ftell() retorna long, então precisa ser do tipo long

    // pula pro início dos dados
    fseek(bin, 17, SEEK_SET);

    char removido;
    // enquanto ainda conseguir ler o campo removido dos registros
    while (fread(&removido, sizeof(char), 1, bin) == 1)
    {    
        // registro não removido (logicamente)
        if (removido == '0') {
            Registro reg_atual;
            
            binToStruct(&reg_atual, bin);

            // teste de match
            if (atendeCriterios(reg_atual, query)) {
                // verifica se o "chute" pra espaço foi suficiente, se não, realoca em dobro para evitar muitos reallocs
                if (*qntd_found == capacidade_atual) {
                    capacidade_atual *= 2; // 10, 20, 40, 80, 160, etc
                    vetor_offsets = realloc(vetor_offsets, capacidade_atual * sizeof(long));
                }

                // salva posição
                long pos_fim = ftell(bin);
                vetor_offsets[*qntd_found] = pos_fim - 80; //guarda o começo do registro
                (*qntd_found)++;
            }

        } else {
            // se está logicamente removido, pular para próximo registro
            fseek(bin, 79, SEEK_CUR);
        }
    }
    // atualizar topo
    return vetor_offsets;
}

void atualizarContadoresCabecalho(FILE* bin)
{
    // ---------------------- ALOCAÇÃO DE MEMÓRIA PARA VETOR DE VISTOS (ESTAÇÃO E PARES DE ESTAÇÃO) ------------------------------  

    // usando a heap pra não estourar a memória do runcodes
    char** nomes_vistos = malloc(CONTADOR_MAX * sizeof(char*));
    // usa-se os nomes e não ID's porque ID's diferentes seriam contabilizados, como os ID's de estações que têm o mesmo nome mas estão em linhas diferentes
    int qtd_estacoes = 0;

    typedef struct {
        int origem;
        int destino;
    } Par;
    Par* pares_vistos = malloc(2 * CONTADOR_MAX * sizeof(Par));
    
    int qtd_pares = 0;
    // --------------------------- FIM ALOCAÇÃO ---------------------------------------------

    fseek(bin, 17, SEEK_SET);
    char removido;

    // le primeiro campo do registro de dados atual E TAMBÉM fica no loop enquanto ainda der pra ler removido
    while (fread(&removido, sizeof(char), 1, bin) == 1)
    {   
        // considera apenas registros não removidos
        if (removido == '0') {
            
            // ------------- LEITURA DOS CAMPOS -------------------------
            
            // le e salva os campos seguintes (APENAS codEstacao e codProxEstacao SERÃO ÚTEIS, proximo e codLinha lido junto apenas porque tá no meio de tudo)
            int proximo, codEstacao, codLinha, codProxEstacao;
            fread(&proximo, sizeof(int), 1, bin);
            fread(&codEstacao, sizeof(int), 1, bin);        // necessário
            fread(&codLinha, sizeof(int), 1, bin);
            fread(&codProxEstacao, sizeof(int), 1, bin);    // necessário

            // pula distProxEstacao + codLinhaIntegra + codEstIntegra = 3*4=12 bytes
            fseek(bin, 12, SEEK_CUR);
            

            // leitura do tamanho + nome (estação)
            int tamNomeEstacao;
            fread(&tamNomeEstacao, sizeof(int), 1, bin);

            char bufferNome[69] = "";
            if (tamNomeEstacao > 0) {
                fread(bufferNome, sizeof(char), tamNomeEstacao, bin);
                bufferNome[tamNomeEstacao] = '\0';      // importante lembrar de adicionar '\0' no final porque no registro bin é salvo sem
            }
            // ------------------- FIM LEITURA CAMPOS -------------------------------

            // ITERA PELOS VETORES PRA VER SE nomeEstacao e codEstacao -> codProxEstacao JÁ ESTÃO LÁ

            // contagem de estacoes pelo nome 
            bool achouEstacao = false;
            for (int i=0; i<qtd_estacoes; i++) {
                if (!strcmp(nomes_vistos[i], bufferNome)) {
                    achouEstacao = true;
                    break;
                }
            }

            // se não achar a estação no vetor de estações vistas, e ela existir, adiciona o nome ao vetor de nomes vistos e  incrimenta
            if (!achouEstacao && tamNomeEstacao > 0) {
                nomes_vistos[qtd_estacoes] = malloc(tamNomeEstacao + 1);
                strcpy(nomes_vistos[qtd_estacoes], bufferNome);
                qtd_estacoes++;
            }

            // contagem de pares (codEstacao -> codProxEstacao)
            if (codProxEstacao != -1) {
                bool achou_par = false;
                for (int i = 0; i < qtd_pares; i++) {
                    if (pares_vistos[i].origem == codEstacao && pares_vistos[i].destino == codProxEstacao) {
                        achou_par = true;
                        break;
                    }
                }

                // se não achou o par, faz basicamente a mesma coisa de antes: adiciona no vetor de vistos e incrementa qntd
                if (!achou_par) {
                    pares_vistos[qtd_pares].origem = codEstacao;
                    pares_vistos[qtd_pares].destino = codProxEstacao;
                    qtd_pares++;
                }
            }

            // pula lixo 
            // lido até agora: 1 (removido) + 16 (4 ints) + 12 (pulados) + 4 (tam) + tamNome = 33 + tamNome
            fseek(bin, 80 - (33 + tamNomeEstacao), SEEK_CUR);

        } else {
            fseek(bin, 79, SEEK_CUR); // registro deletado pula)
        }
    }

    // é.... (só assim pra passar nos testes)

    // grava valores atualizados no cabeçalho
    fseek(bin, 9, SEEK_SET);
    fwrite(&qtd_estacoes, sizeof(int), 1, bin);
    fwrite(&qtd_pares, sizeof(int), 1, bin);

    // LIBERAR MEMÓRIA ALOCADA PROS VETORES DE VISTOS
    for (int i = 0; i < qtd_estacoes; i++)
        free(nomes_vistos[i]);
    free(nomes_vistos);
    free(pares_vistos);
}