#ifndef BUSCA_H
#define BUSCA_H

#include <stdbool.h>
#include <stdio.h>
#include "estruturas.h"

void zerarFlags(OQueBuscar* query);

// Preenche a struct de query baseado no nome do campo
void marcadorFlag(char* nomeCampo, OQueBuscar* Query);

// Função que compara o registro lido com a query do usuário
bool atende_criterios(Registro reg_lido, OQueBuscar query);

// Retorna um vetor de offsets (posições no arquivo) onde houve match.
// O parâmetro qtd_encontrados é atualizado por referência.
// ftell() retorna long
long* percorre_e_buscaCorrespondencia(FILE* bin, OQueBuscar query, int* qtd_encontrados);

void atualizar_contadores_cabecalho(FILE* bin);

#endif