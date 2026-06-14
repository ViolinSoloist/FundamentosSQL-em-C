#ifndef DELETE_BTREE_H
#define DELETE_BTREE_H

#include "estruturas.h"

/**
 * delete_btree - Remove registros da árvore B e arquivo de dados conforme critérios
 * @nomeArquivoDados: caminho do arquivo de dados
 * @nomeArquivoIndice: caminho do arquivo de índice (árvore B)
 * @n: número de consultas/remoções a executar
 * 
 * Executa n operações de remoção lógica no arquivo de dados e remove as chaves
 * correspondentes do índice em árvore B.
 */
void delete_btree(const char* nomeArquivoDados, const char* nomeArquivoIndice, int n);

#endif 
