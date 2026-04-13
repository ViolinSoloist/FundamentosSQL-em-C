#include "cabecalho.h"
#include "serial.h"

#define CONTADOR_MAX 1000

static bool inicializarVariaveis(CamposUsados* campo_usado)
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


/// @brief inicializa a escrita de dados no arquivo bin, começando com o registro de cabeçalho
void escreveCabecarioBin(bool seek_inicio, FILE* bin, Cabecalho* head)
{
    if (!seek_inicio) {
        // primeira passada: escreve tudo do na ordem
        fseek(bin, 0, SEEK_SET);
        fwrite(&head->status, sizeof(char), 1, bin);
        fwrite(&head->topo, sizeof(int), 1, bin);
        fwrite(&head->proxRRN, sizeof(int), 1, bin);
        fwrite(&head->nroEstacoes, sizeof(int), 1, bin);
        fwrite(&head->nroParesEstacao, sizeof(int), 1, bin);
    } else {
        // segunda passada: atualiza o que mudou, mantem topo intacto, precisa de fseek
        fseek(bin, 0, SEEK_SET);
        fwrite(&head->status, sizeof(char), 1, bin);
        
        fseek(bin, 5, SEEK_SET); // Pula o 'topo' (-1) que já está no arquivo
        fwrite(&head->proxRRN, sizeof(int), 1, bin);
        fwrite(&head->nroEstacoes, sizeof(int), 1, bin);
        fwrite(&head->nroParesEstacao, sizeof(int), 1, bin);
    }
}

void contarEstacoesEPares(Registro* temporario, CamposUsados* campos)
{
    // contagem estações únicas (pelo nome)
    if (strlen(temporario->nomeEstacao) > 0) {
        bool estacaoJaExiste = false;
        for (int i = 0; i < (campos->qtd_estacoes); i++) {
            if (!strcmp(campos->nomes_vistos[i], temporario->nomeEstacao)) {
                estacaoJaExiste = true;
                break;
            }
        }
        if (!estacaoJaExiste) {
            // aloca espaço (no primeiro index não ocupado do vetor de nomes) suficente para o nome
            campos->nomes_vistos[campos->qtd_estacoes] = malloc(strlen(temporario->nomeEstacao) + 1);
            strcpy(campos->nomes_vistos[campos->qtd_estacoes], temporario->nomeEstacao);
            campos->qtd_estacoes++;
        }
    }

    // contagem de pares
    if (temporario->codProxEstacao != -1) { 
        bool parJaExiste = false;
        for (int i = 0; i < campos->qtd_pares; i++) {
            if (campos->pares_vistos[i].origem == temporario->codEstacao && campos->pares_vistos[i].destino == temporario->codProxEstacao) {
                parJaExiste = true;
                break;
            }
        }
        if (!parJaExiste) {
            campos->pares_vistos[campos->qtd_pares].origem = temporario->codEstacao;
            campos->pares_vistos[campos->qtd_pares].destino = temporario->codProxEstacao;
            campos->qtd_pares++;
        }
    }
}

// grava valores atualizados no cabeçalho
void gravaEFinaliza(FILE* bin, CamposUsados* campo)
{
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
    CamposUsados campo_usado;
    inicializarVariaveis(&campo_usado);

    fseek(bin, 17, SEEK_SET);
    char removido;

    while (fread(&removido, sizeof(char), 1, bin) == 1) {   
        if (removido == '0') {             
            
            // lê o registro inteiro do binario para a memória (e também avança a agulha 80 bytes)
            Registro reg_atual;
            binToStruct(&reg_atual, bin);

            contarEstacoesEPares(&reg_atual, &campo_usado);

            // limpeza da memória que o binToStruct alocou
            if (reg_atual.nomeEstacao) free(reg_atual.nomeEstacao);
            if (reg_atual.nomeLinha) free(reg_atual.nomeLinha);

        } else {
            fseek(bin, 79, SEEK_CUR); 
        }
    }

    gravaEFinaliza(bin, &campo_usado);
}