#ifndef SELECT_WHERE_JUNCAO_BTREE_H
#define SELECT_WHERE_JUNCAO_BTREE_H

#include <stdio.h>
#include "estruturas.h"
#include "busca.h"
#include "manipul_arq.h"
#include "terminal.h"
#include "serial.h"
#include "arvoreB.h"

void select_where_juncao_btree(const char* nomeArquivoBin1, const char* campo1, const char* nomeArquivoBin2, const char* campo2, const char* nomeArv);

#endif