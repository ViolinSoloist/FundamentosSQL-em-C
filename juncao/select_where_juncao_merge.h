#ifndef SELECT_WHERE_JUNCAO_MERGE_H
#define SELECT_WHERE_JUNCAO_MERGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"
#include "busca.h"
#include "manipul_arq.h"
#include "terminal.h"
#include "serial.h"
#include "order_by.h"

void select_where_juncao_merge(const char* nomeArquivoBinario1, const char* campo1, const char* nomeArquivoBinario2, const char* campo2);

#endif