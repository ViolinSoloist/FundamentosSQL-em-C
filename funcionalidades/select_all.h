#ifndef SELECT_ALL_H
#define SELECT_ALL_H

#include "stdio.h"
#include "stdbool.h"

#include "estruturas.h"
#include "busca.h"
#include "serial.h"
#include "terminal.h"
#include "manipul_arq.h"

/// @brief Função principal de recuperação de registros
/// @param nomeArquivoBin Arquivo binário do qual os registros serão recuperados
void select_all(const char* nomeArquivoBin);

#endif