#ifndef CABECALHO_H
#define CABECALHO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "estruturas.h"

void escreveCabecarioBin(bool seek_inicio, FILE* bin, Cabecalho* head);
void contarEstacoesEPares(Registro* temporario, CamposUsados* campos);
void gravaEFinaliza(FILE* bin, CamposUsados* campo);
void atualizarContadoresCabecalho(FILE* bin);

#endif