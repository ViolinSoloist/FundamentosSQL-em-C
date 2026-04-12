#ifndef CABECALHO_H
#define CABECALHO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "estruturas.h"

/*
responsável por todas funções que mexem com o cabeçalho: escrita, contagem de estacoes/pares, atualização de contagem/pares
*/

/// @brief inicializa a escrita de dados no arquivo bin, começando com o registro de cabeçalho
void escreveCabecarioBin(bool seek_inicio, FILE* bin, Cabecalho* head);

void contarEstacoesEPares(Registro* temporario, CamposUsados* campos);

// grava valores atualizados no cabeçalho
void gravaEFinaliza(FILE* bin, CamposUsados* campo);

void atualizarContadoresCabecalho(FILE* bin);

#endif