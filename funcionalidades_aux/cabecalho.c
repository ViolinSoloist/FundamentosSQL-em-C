#include "cabecalho.h"

#define CONTADOR_MAX 1000

void leituraCamposParaAtualizar(FILE* bin, CamposUsados* campos) {
    // le e salva os campos seguintes (APENAS codEstacao e codProxEstacao SERÃO ÚTEIS, proximo e codLinha lido junto apenas porque tá no meio de tudo)
    int proximo, codLinha;
    fread(&proximo, sizeof(int), 1, bin);
    fread(&campos->codEstacao, sizeof(int), 1, bin);        // necessário
    fread(&codLinha, sizeof(int), 1, bin);
    fread(&campos->codProxEstacao, sizeof(int), 1, bin);    // necessário

    // pula distProxEstacao + codLinhaIntegra + codEstIntegra = 3*4=12 bytes
    fseek(bin, 12, SEEK_CUR);

    // leitura do tamanho + nome (estação)
    fread(&campos->tamNomeEstacao, sizeof(int), 1, bin);

    
    if (campos->tamNomeEstacao > 0) {
        fread(campos->bufferNome, sizeof(char), campos->tamNomeEstacao, bin);
        campos->bufferNome[campos->tamNomeEstacao] = '\0';      // importante lembrar de adicionar '\0' no final porque no registro bin é salvo sem
    }
}

void recontagemNomeEPares(CamposUsados* campos) {
    // contagem de estacoes pelo nome 
    bool achouEstacao = false;
    for (int i=0; i<(campos->qtd_estacoes); i++) {
        if (!strcmp(campos->nomes_vistos[i], campos->bufferNome)) {
            achouEstacao = true;
            break;
        }
    }

    // se não achar a estação no vetor de estações vistas, e ela existir, adiciona o nome ao vetor de nomes vistos e  incrimenta
    if (!achouEstacao && campos->tamNomeEstacao > 0) {
        campos->nomes_vistos[campos->qtd_estacoes] = malloc(campos->tamNomeEstacao + 1);
        strcpy(campos->nomes_vistos[campos->qtd_estacoes], campos->bufferNome);
        (campos->qtd_estacoes)++;
    }

    // contagem de pares (codEstacao -> codProxEstacao)
    if ((campos->codProxEstacao) != -1) {
        bool achou_par = false;
        for (int i = 0; i < (campos->qtd_pares); i++) {
            if (campos->pares_vistos[i].origem == (campos->codEstacao) && campos->pares_vistos[i].destino == (campos->codProxEstacao)) {
                achou_par = true;
                break;
            }
        }

        // se não achou o par, faz basicamente a mesma coisa de antes: adiciona no vetor de vistos e incrementa qntd
        if (!achou_par) {
            campos->pares_vistos[campos->qtd_pares].origem = campos->codEstacao;
            campos->pares_vistos[campos->qtd_pares].destino = campos->codProxEstacao;
            (campos->qtd_pares)++;
        }
    }
}

bool inicializarVariaveis(CamposUsados* campo_usado)
{
    // usando a heap pra não estourar a memória stack, char** = matriz de char = vetor de string
    // usa-se os nomes e não ID's porque ID's diferentes seriam contabilizados, como os ID's de estações que têm o mesmo nome mas estão em linhas diferentes
    campo_usado->nomes_vistos = malloc(CONTADOR_MAX * sizeof(char*));
    campo_usado->pares_vistos = malloc(2 * CONTADOR_MAX * sizeof(Par));
    campo_usado->qtd_estacoes = 0;
    campo_usado->qtd_pares = 0;

    /* if (campo_usado->nomes_vistos == NULL || campo_usado->pares_vistos == NULL) return false;
    return true; */

    return ((campo_usado->nomes_vistos == NULL || campo_usado->pares_vistos == NULL) ? false : true);
}


/// @private @brief inicializa a escrita de dados no arquivo bin, começando com o registro de cabeçalho
void escreveCabecarioBin(bool seek_inicio, FILE* bin, char status, int proxRRN, int nroEstacoes, int nroParesEstacao)
{
    if (!seek_inicio) {
        // primeira passada: escreve tudo do na ordem
        int topo = -1;
        fseek(bin, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, bin);
        fwrite(&topo, sizeof(int), 1, bin);
        fwrite(&proxRRN, sizeof(int), 1, bin);
        fwrite(&nroEstacoes, sizeof(int), 1, bin);
        fwrite(&nroParesEstacao, sizeof(int), 1, bin);
    } else {
        // segunda passada: atualiza o que mudou, mantem topo intacto, precisa de fseek
        fseek(bin, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, bin);
        
        fseek(bin, 5, SEEK_SET); // Pula o 'topo' (-1) que já está no arquivo
        fwrite(&proxRRN, sizeof(int), 1, bin);
        fwrite(&nroEstacoes, sizeof(int), 1, bin);
        fwrite(&nroParesEstacao, sizeof(int), 1, bin);
    }
}

/// @private
void contarEstacoesEPares(Registro* temporario, char* nomesVistos[], int* totalEstacoes, Par paresVistos[], int* totalPares)
{
    // contagem estações únicas (pelo nome)
    if (strlen(temporario->nomeEstacao) > 0) {
        bool estacaoJaExiste = false;
        for (int i = 0; i < (*totalEstacoes); i++) {
            if (!strcmp(nomesVistos[i], temporario->nomeEstacao)) {
                estacaoJaExiste = true;
                break;
            }
        }
        if (!estacaoJaExiste) {
            // aloca espaço (no primeiro index não ocupado do vetor de nomes) suficente para o nome
            nomesVistos[*totalEstacoes] = malloc(strlen(temporario->nomeEstacao) + 1);
            strcpy(nomesVistos[*totalEstacoes], temporario->nomeEstacao);
            (*totalEstacoes)++;
        }
    }

    // contagem de pares
    if (temporario->codProxEstacao != -1) { 
        bool parJaExiste = false;
        for (int i = 0; i < (*totalPares); i++) {
            if (paresVistos[i].origem == temporario->codEstacao && paresVistos[i].destino == temporario->codProxEstacao) {
                parJaExiste = true;
                break;
            }
        }
        if (!parJaExiste) {
            paresVistos[*totalPares].origem = temporario->codEstacao;
            paresVistos[*totalPares].destino = temporario->codProxEstacao;
            (*totalPares)++;
        }
    }
}

void gravaEFinaliza(FILE* bin, CamposUsados* campo)
{
    // grava valores atualizados no cabeçalho
    fseek(bin, 9, SEEK_SET);
    fwrite(&campo->qtd_estacoes, sizeof(int), 1, bin);
    fwrite(&campo->qtd_pares, sizeof(int), 1, bin);

    // LIBERAR MEMÓRIA ALOCADA PROS VETORES DE VISTOS
    for (int i = 0; i < campo->qtd_estacoes; i++)
        free(campo->nomes_vistos[i]);
    free(campo->nomes_vistos);  
    free(campo->pares_vistos);
}

void atualizarContadoresCabecalho(FILE* bin)
{
    // ---------------------- ALOCAÇÃO DE MEMÓRIA PARA VETOR DE VISTOS (ESTAÇÃO E PARES DE ESTAÇÃO) ------------------------------  
    CamposUsados campo_usado;
    inicializarVariaveis(&campo_usado);

    fseek(bin, 17, SEEK_SET);
    char removido;

    // le primeiro campo do registro de dados atual E TAMBÉM fica no loop enquanto ainda der pra ler removido
    while (fread(&removido, sizeof(char), 1, bin) == 1) {   
        // considera apenas registros não removidos
        if (removido == '0') {             
            leituraCamposParaAtualizar(bin, &campo_usado);

            // ITERA PELOS VETORES PRA VER SE nomeEstacao e codEstacao -> codProxEstacao JÁ ESTÃO LÁ
            recontagemNomeEPares(&campo_usado);

            // pula lixo: lido até agora: 1 (removido) + 16 (4 ints) + 12 (pulados) + 4 (tam) + tamNome = 33 + tamNome
            fseek(bin, 80 - (33 + campo_usado.tamNomeEstacao), SEEK_CUR);
        } else {
            fseek(bin, 79, SEEK_CUR); // registro deletado pula)
        }
    }

    gravaEFinaliza(bin, &campo_usado);
}