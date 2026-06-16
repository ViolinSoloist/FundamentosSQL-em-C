#ifndef DELETE_BTREE_H
#define DELETE_BTREE_H

#include "estruturas.h"

/**
 * delete_btree - remove registros da árvore B e arquivo de dados conforme critérios
 * 
 * executa n operações de remoção lógica no arquivo de dados e remove as chaves
 * correspondentes do índice em árvore B.
 */
void delete_btree(const char* nomeArquivoDados, const char* nomeArquivoIndice, int n);

#endif 
