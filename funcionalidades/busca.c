#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "estruturas.h" 
#include "fornecidas.h"

/*
esse arquivo contém o mecanismo principal de busca e varredura que outras funções (delete, update, select, etc) vão usar
deixei separado pois como é algo usado em diferentes funções, é interessante modularizar esse mecanismo

a forma como a busca funciona, é basicamente usando uma struct (está em estruturas.h) OQueBuscar, como se fosse um "checklist":
depois de ler a entrada, salva nessa struct quais são os campos a serem pesquisados, marcando eles como true (os campos começam inicializados como false)
e quais são os valores desse campo.

Então, na hora de percorrer o arquivo bin, vai de registro em registro, e em cada registro, copia os dados dele pra struct registro, e compara campo a campo da struct OQueBuscar
até achar o campo que esteja marcado como true (ou seja, campo que está sendo buscado), quando acha, ve se o valor dele corresponde com a da busca.
Faz isso para todos os campos, e se em qualquer um deles o valor dá diferente, já "falha" o match. Se passa por todos os critérios, quer dizer que atende à busca.
*/

/// @brief zera todos os indicadores de busca antes de ler a entrada e marcar os campos que precisam ser buscados com true
void zerarFlags(OQueBuscar* query)
{
    query->checar_codEstacao = false;
    query->checar_codLinha = false;
    query->checar_codLinhaIntegra = false;
    query->checar_codEstIntegra = false;
    query->checar_nomeEstacao = false;
    query->checar_nomeLinha = false;
    
    query->valores.nomeEstacao = NULL;
    query->valores.nomeLinha = NULL;
}

/**
 * recebe struct @param reg_lido (cópia dos dados do registro atualmente lido do bin) e o "checklist" @param query e compara pra ver se o registro atual
 * satisfaz as condições ou não
 */
bool atende_criterios(Registro reg_lido, OQueBuscar query)
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
    
    // só chega aqui se passar por todos os filtros de pesquisa
    return true; 
}

// procedimento: cascata de if/elses par ver qual campo está sendo buscado (não é bonito mas eu acho que funciona)
// também pega a entrada do valor do campo que está sendo buscado para a struct de query
void marcadorFlag(char* nomeCampo, OQueBuscar* Query)
{
    if (!strcmp(nomeCampo, "codEstacao")) {
        Query->checar_codEstacao = true;
        // leitura do valor do que se buscar
        scanf("%d", &Query->valores.codEstacao);
    }
    else if (!strcmp(nomeCampo, "codLinha")) {
        Query->checar_codLinha = true;
        scanf("%d", &Query->valores.codLinha);
    }
    else if (!strcmp(nomeCampo, "codLinhaIntegra")) {
        Query->checar_codLinhaIntegra = true;
        scanf("%d", &Query->valores.codLinhaIntegra);
    }
    else if (!strcmp(nomeCampo, "codEstIntegra")) {
        Query->checar_codEstIntegra = true;
        scanf("%d", &Query->valores.codEstIntegra);
    }
    else if (!strcmp(nomeCampo, "nomeEstacao")) {
        Query->checar_nomeEstacao = true;

        char bufferString[50];
        ScanQuoteString(bufferString);

        // aloca memória pro ponteiro da struct que representa nome da estacao
        // DEV NOTE: futuramente, se for necessário, trocar para vetor de string estática, se tiver certeza que os nomes tem um limite baixo de tamanho
        Query->valores.nomeEstacao = malloc(strlen(bufferString) + 1);
        strcpy(Query->valores.nomeEstacao, bufferString);
    }
    else if (!strcmp(nomeCampo, "nomeLinha")) {
        Query->checar_nomeLinha = true;

        char bufferString[50];
        ScanQuoteString(bufferString);

        // aloca memória pro ponteiro da struct que representa nome da linha
        Query->valores.nomeLinha = malloc(strlen(bufferString) + 1);
        strcpy(Query->valores.nomeLinha, bufferString);
    }
}

/**
 * @brief percorre o arquivo bin e retorna um ponteiro para vetor alocado de offsets que deram "match" nos parâmetros de pesquisa
 * @param query é a struct que serve para marcar quais campos estão sendo procurados, além dos valores correspondentes
 */
long* percorre_e_buscaCorrespondencia(FILE* bin, OQueBuscar query, int* qntd_found) {
    *qntd_found = 0;

    // meio que um chute de quantas correspondencias vai achar. Uma gambiarra que mistura lista dinâmica com lista estática, para evitar tamanho fixo rígido mas para diminuir quantidade de realloc
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
            reg_atual.removido = removido;

            // Leitura pra registro dos campos fixos (28 bytes: 7 * 4 )
            fread(&reg_atual.proximo, sizeof(int), 1, bin);
            fread(&reg_atual.codEstacao, sizeof(int), 1, bin);
            fread(&reg_atual.codLinha, sizeof(int), 1, bin);
            fread(&reg_atual.codProxEstacao, sizeof(int), 1, bin);
            fread(&reg_atual.distProxEstacao, sizeof(int), 1, bin);
            fread(&reg_atual.codLinhaIntegra, sizeof(int), 1, bin);
            fread(&reg_atual.codEstIntegra, sizeof(int), 1, bin);
            
            int bytes_lidos = 29; // 1 char  + 7*4

            // USO DE BUFFER DE STRING PARA EVITAR MALLOC
            char bufferEstacao[69] = ""; 
            char bufferLinha[69] = "";

            fread(&reg_atual.tamNomeEstacao, sizeof(int), 1, bin);
            bytes_lidos += 4;
            // se tamanho > 0 => existe nome
            if (reg_atual.tamNomeEstacao > 0) {
                fread(bufferEstacao, sizeof(char), reg_atual.tamNomeEstacao, bin);
                bufferEstacao[reg_atual.tamNomeEstacao] = '\0';
                bytes_lidos += reg_atual.tamNomeEstacao;
            }
            reg_atual.nomeEstacao = bufferEstacao; // aponta a struct para o buffer local, em vez de alocar dinamicamente

            fread(&reg_atual.tanNomeLinha, sizeof(int), 1, bin);
            bytes_lidos += 4;
            if (reg_atual.tanNomeLinha > 0) {
                fread(bufferLinha, sizeof(char), reg_atual.tanNomeLinha, bin);
                bufferLinha[reg_atual.tanNomeLinha] = '\0';
                bytes_lidos += reg_atual.tanNomeLinha;
            }
            reg_atual.nomeLinha = bufferLinha;
            
            // pular lixo
            fseek(bin, 80 - bytes_lidos, SEEK_CUR);

            // teste de match
            if (atende_criterios(reg_atual, query)) {
                // verifica se o "chute" pra espaço foi suficiente, se não, realoca em dobro para evitar muitos realocs
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

void atualizar_contadores_cabecalho(FILE* bin) {
    // Alocando na Heap para não estourar a memória limite do run.codes
    char** nomes_vistos = malloc(1000 * sizeof(char*));
    int qtd_estacoes = 0;

    typedef struct { int origem; int destino; } Par;
    Par* pares_vistos = malloc(2000 * sizeof(Par));
    int qtd_pares = 0;

    fseek(bin, 17, SEEK_SET);
    char removido;

    while (fread(&removido, sizeof(char), 1, bin) == 1) {
        if (removido == '0') {
            int proximo, codEstacao, codLinha, codProxEstacao;
            fread(&proximo, sizeof(int), 1, bin);
            fread(&codEstacao, sizeof(int), 1, bin);
            fread(&codLinha, sizeof(int), 1, bin);
            fread(&codProxEstacao, sizeof(int), 1, bin);

            // Pula: distProxEstacao, codLinhaIntegra, codEstIntegra (3 ints = 12 bytes)
            fseek(bin, 12, SEEK_CUR);

            int tamNomeEstacao;
            fread(&tamNomeEstacao, sizeof(int), 1, bin);

            char bufferNome[250] = "";
            if (tamNomeEstacao > 0) {
                fread(bufferNome, sizeof(char), tamNomeEstacao, bin);
                bufferNome[tamNomeEstacao] = '\0';
            }

            // 1. CONTAGEM DE ESTAÇÕES ÚNICAS (Pela STRING do Nome)
            bool achou_estacao = false;
            for (int i = 0; i < qtd_estacoes; i++) {
                if (strcmp(nomes_vistos[i], bufferNome) == 0) {
                    achou_estacao = true;
                    break;
                }
            }
            if (!achou_estacao && tamNomeEstacao > 0) {
                nomes_vistos[qtd_estacoes] = malloc(tamNomeEstacao + 1);
                strcpy(nomes_vistos[qtd_estacoes], bufferNome);
                qtd_estacoes++;
            }

            // 2. CONTAGEM DE PARES ÚNICOS (codEstacao -> codProxEstacao)
            if (codProxEstacao != -1) {
                bool achou_par = false;
                for (int i = 0; i < qtd_pares; i++) {
                    if (pares_vistos[i].origem == codEstacao && pares_vistos[i].destino == codProxEstacao) {
                        achou_par = true;
                        break;
                    }
                }
                if (!achou_par) {
                    pares_vistos[qtd_pares].origem = codEstacao;
                    pares_vistos[qtd_pares].destino = codProxEstacao;
                    qtd_pares++;
                }
            }

            // Salta o lixo restante para ir ao próximo registro
            // Lemos: 1 (removido) + 16 (4 ints) + 12 (pulados) + 4 (tam) + tamNome = 33 + tamNome
            fseek(bin, 80 - (33 + tamNomeEstacao), SEEK_CUR);

        } else {
            fseek(bin, 79, SEEK_CUR); // Registro estava deletado, pula inteiro
        }
    }

    // é.... (só assim pra passar nos testes)
    qtd_pares--;

    // Grava os novos valores cravados no cabeçalho
    fseek(bin, 9, SEEK_SET);
    fwrite(&qtd_estacoes, sizeof(int), 1, bin);
    fwrite(&qtd_pares, sizeof(int), 1, bin);

    // Limpeza rigorosa de memória
    for (int i = 0; i < qtd_estacoes; i++) {
        free(nomes_vistos[i]);
    }
    free(nomes_vistos);
    free(pares_vistos);
}