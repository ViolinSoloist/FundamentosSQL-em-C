#ifndef CABECALHO_H
#define CABECALHO_H

#include <stdio.h>

#include "estruturas.h"

static void escreveCabecarioBin(bool seek_inicio, FILE* bin, char status, int proxRRN, int nroEstacoes, int nroParesEstacao);
static void contarEstacoesEPares(Registro* temporario, char* nomesVistos[], int* totalEstacoes, Par paresVistos[], int* totalPares);
static void gravaEFinaliza(FILE* bin, CamposUsados* campo);
void atualizarContadoresCabecalho(FILE* bin);

#endif