#ifndef CABECALHO_H
#define CABECALHO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "estruturas.h"

void escreveCabecarioBin(bool seek_inicio, FILE* bin, char status, int proxRRN, int nroEstacoes, int nroParesEstacao);
void contarEstacoesEPares(Registro* temporario, char* nomesVistos[], int* totalEstacoes, Par paresVistos[], int* totalPares);
void gravaEFinaliza(FILE* bin, CamposUsados* campo);
void atualizarContadoresCabecalho(FILE* bin);

#endif