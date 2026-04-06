// Erik Min Soo Chung   - 15450334
// João Pedro Neves     -

#include <stdio.h>
#include <stdlib.h>

#include "create_table.h"
#include "delete.h"
#include "busca.h"
#include "select_all.h"
#include "update.h"

#define PRINTAR_MENU false

int main() { 
    // menu de opções
    if (PRINTAR_MENU) {
        printf("\nEscolha uma das seguintes opções:\n");
        printf(" 1 - CREATE TABLE\n");
        printf(" 2 - SELECT\n");
        printf(" 3 - SELECT WHERE\n");
        printf(" 4 - DELETE\n");
        printf(" 5 - INSERT INTO\n");
        printf(" 6 - UPDATE\n");
    }

    int funcionalidade;

    // buffer temporário
    char arquivoEntrada[50];
    char arquivoSaida[50];

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
            
            break;

        case 4:
			// DELETE: deleta N registros
            scanf("%s %d", arquivoSaida, &n);
            deletar(arquivoSaida, n);
            break;

        case 5:
			// INSERT INTO: inserir dados na tabela
			break;
		
		case 6:
			// UPDATE: atualizar dados na tabela
            scanf("%s %d", arquivoSaida, &n);
            update((char*)arquivoSaida, n);
			break;

        default:
            printf("Falha no processamento do arquivo.\n");
            break;
    }

    return 0;
}