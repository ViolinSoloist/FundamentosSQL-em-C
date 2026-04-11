#include "cabecalho.h"

/// @private @brief inicializa a escrita de dados no arquivo bin, começando com o registro de cabeçalho
static void escreveCabecarioBin(bool seek_inicio, FILE* bin, char status, int proxRRN, int nroEstacoes, int nroParesEstacao)
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
static void contarEstacoesEPares(Registro* temporario, char* nomesVistos[], int* totalEstacoes, Par paresVistos[], int* totalPares)
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

static void gravaEFinaliza(FILE* bin, CamposUsados* campo)
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