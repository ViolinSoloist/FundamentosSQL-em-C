#include <stdio.h>
#include <stdlib.h>

#include "estruturas.h" 
#include "fornecidas.h"
#include "busca.h"
#include "update.h"
#include "manipul_arq.h"


/// @private @brief 
Le2LinhasBuscaEUpdate(FILE* nomeArquivoBin)
{    
    int m; scanf("%d", &m);
    OQueBuscar oqbuscar;
    preencherQuery(&oqbuscar, m); // PREENCHIMENTO DA QUERY "CHECKLIST"

    int p; scanf("%d", &p);
    OQueBuscar oqmudar;
    preencherQuery(&oqmudar, p);
    // ignorar o nome, é basicamente a mesma lógica de preencher a query "checklist" de o buscar, pra "checklist" de o que dar update


    // LEMBRAR DE CHAMAR DESALOCAÇÃO DE MEMÓRIA

}

void update(FILE* nomeArquivoBin, int n)
{
    // rotina ao abrir arquivo binário rb+
    FILE* file = abrirVerificarInconsistentar(nomeArquivoBin);

    for (int i=0; i<n; i++) 
        Le2LinhasBuscaEUpdate(nomeArquivoBin);
    
}
