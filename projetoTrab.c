/// @author Erik Min Soo Chung   - 15450334
/// @author João Pedro Neves     - 14713404

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
#include "create_index.h"
#include "select_where_btree.h"
#include "insert_btree.h"
#include "delete_btree.h"
#include "../juncao/select_where_juncao.h"
#include "../juncao/select_where_juncao_btree.h"
#include "../juncao/order_by.h"
#include "../juncao/select_where_juncao_merge.h"

int main() { 

    int funcionalidade;

    // buffer temporário
    char arquivoEntrada[50];
    char arquivoSaida[50];
    char arquivoIndice[50];

    char campo1[15];
    char campo2[15];

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
            scanf("%s %s", arquivoEntrada, arquivoIndice);
            create_index(arquivoEntrada, arquivoIndice);
            break;
        
        case 8:
            scanf("%s %s %d", arquivoEntrada, arquivoIndice, &n);
            select_where_btree(arquivoEntrada, arquivoIndice, n);
            break;
        
        case 9:
            scanf("%s %s %d", arquivoEntrada, arquivoIndice, &n);
            insert_btree(arquivoEntrada, arquivoIndice, n);
            break;
        
        case 10:
            scanf("%s %s %d", arquivoEntrada, arquivoSaida, &n);
            delete_btree(arquivoEntrada, arquivoSaida, n);
            break;

        case 11:
            scanf("%s %s %s %s", arquivoEntrada, campo1, arquivoSaida, campo2);
            select_where_juncao(arquivoEntrada, campo1, arquivoSaida, campo2);
            break;
            
        case 12:
            scanf("%s %s %s %s %s", arquivoEntrada, campo1, arquivoSaida, campo2, arquivoIndice);
            select_where_juncao_btree(arquivoEntrada, campo1, arquivoSaida, campo2, arquivoIndice);
            break;

        case 13:
            scanf("%s %s %s", arquivoEntrada, campo1, arquivoSaida);
            order_by(arquivoEntrada, campo1, arquivoSaida, true);
            break;

        case 14:
            scanf("%s %s %s %s", arquivoEntrada, campo1, arquivoSaida, campo2);
            select_where_juncao_merge(arquivoEntrada, campo1, arquivoSaida, campo2);
            break;

        default:
            printf("Falha no processamento do arquivo.\n");
            break;
    }

    return 0;
}