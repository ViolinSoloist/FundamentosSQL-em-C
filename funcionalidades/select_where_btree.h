#ifndef SELECT_WHERE_BTREE_H
#define SELECT_WHERE_BTREE_H

#include "stdio.h"
#include "stdbool.h"

#include "select_all.h"
#include "select_where.h"
#include "estruturas.h"
#include "busca.h"
#include "terminal.h"
#include "serial.h"
#include "manipul_arq.h"
#include "../arvoreB/arvoreB.h"

void select_where_btree(const char* nomeArquivoBin, const char* nomeArquivoArvoreBin, int numeroBuscas);

#endif