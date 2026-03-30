#include <stdio.h>
#include <stdlib.h>
#include "create_table.h"

int main() {   
    // menu de opções
    // printf("\nEscolha uma das seguintes opções:\n");
    // printf(" 1 - CREATE TABLE\n");
    // printf(" 2 - SELECT\n");
    // printf(" 3 - SELECT WHERE\n");
    // printf(" 4 - DELETE\n");
    // printf(" 5 - INSERT INTO\n");
    // printf(" 6 - UPDATE\n");

    int funcionalidade;

    // buffer temporário
    char arquivoEntrada[50];
    char arquivoSaida[50];

    // leitura da operação
    if (scanf("%d", &funcionalidade) != 1) {
		fprintf(stderr, "Escolha inexistente.\n");
        return 0; // fim se não houver entrada
    }

    switch (funcionalidade) {
        case 1: 
            // CREATE TABLE: recebe CSV e gera o Binário
            scanf("%s %s", arquivoEntrada, arquivoSaida);
            create_table(arquivoEntrada, arquivoSaida);
            break;
            
        case 2: 
            // SELECT ALL: le arquivo bin e imprime na tela	
            break;
            
        case 3:
            // SELECT C/ BUSCA: Le nome do arquivo e os n critérios de busca
            
            break;

        case 4:
			// DELETE: recebe nome do arquivo + critérios de busca para remoção
    
            break;

        case 5:
			// INSERT INTO: inserir dados na tabela
			break;
		
		case 6:
			// UPDATE: atualizar dados na tabela
			break;

        default:
            printf("Falha no processamento do arquivo.\n");
            break;
    }

    return 0;
}