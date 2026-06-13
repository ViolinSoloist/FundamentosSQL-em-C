// Erik Min Soo Chung   - 15450334
// João Pedro Neves     - 14713404

#include <stdio.h>
#include <stdlib.h>

#include "create_table.h"
#include "delete.h"
#include "busca.h"
#include "select_all.h"
#include "select_where.h"
#include "insert.h"
#include "update.h"

#include "arvoreB.h"
#include "insert_btree.h"

int main() { 

    int funcionalidade;

    // buffer temporário
    char arquivoEntrada[50];
    char arquivoSaida[50];
    char arquivoIndice[50];

    // leitura da operação
    if (scanf("%d ", &funcionalidade) != 1) {
		fprintf(stderr, "Escolha inexistente.\n");
        return 0; // fim se não houver entrada
    }

    int n;

    switch (funcionalidade) {
        case 1: 
            // CREATE TABLE: recebe CSV e gera o Binário
            scanf("%s %s", arquivoEntrada, arquivoSaida);
            create_table(arquivoEntrada, arquivoSaida);
            break;
            
        case 2: 
            // SELECT ALL: le arquivo bin e imprime na tela
            scanf("%s", arquivoEntrada);
            select_all(arquivoEntrada);	
            break;
            
        case 3:
            // SELECT C/ BUSCA: Le nome do arquivo e os n critérios de busca
            scanf("%s %d", arquivoEntrada, &n);
            select_where(arquivoEntrada, n);
            break;

        case 4:
			// DELETE: deleta N registros
            scanf("%s %d", arquivoEntrada, &n);
            deletar(arquivoEntrada, n);
            break;

        case 5:
			// INSERT INTO: inserir dados na tabela
            scanf("%s %d", arquivoEntrada, &n);
            insert(arquivoEntrada, n);
			break;
		
		case 6:
			// UPDATE: atualizar dados na tabela
            scanf("%s %d", arquivoEntrada, &n);
            update(arquivoEntrada, n);
			break;

        case 7:
            // CREATE INDEX: cria a árvore a partir de um arquivo bin
            scanf("%s %s", arquivoEntrada, arquivoIndice);
            // create_index(arquivoEntrada, arquivoIndice); // a ser implementado
            break;

        case 8:
            // SELECT COM ÍNDICE: busca usando B-Tree
            scanf("%s %s", arquivoEntrada, arquivoIndice);
            // select_btree(arquivoEntrada, arquivoIndice); // a ser implementado
            break;

        case 9:
            // INSERT COM ÍNDICE: insere no arquivo de dados e na Árvore B
            scanf("%s %s %d", arquivoEntrada, arquivoIndice, &n);
            insert_btree(arquivoEntrada, arquivoIndice, n);
            break;

        case 10:
            // DELETE COM ÍNDICE: remove do arquivo de dados e da Árvore B
            scanf("%s %s %d", arquivoEntrada, arquivoIndice, &n);
            // delete_btree(arquivoEntrada, arquivoIndice, n); // a ser implementado (por mim)
            break;

        default:
            printf("Falha no processamento do arquivo.\n");
            break;
    }

    return 0;
}