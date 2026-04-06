// #include <stdio.h>
// #include <stdlib.h>

// #include "estruturas.h" 
// #include "fornecidas.h"
// #include "busca.h"
// #include "update.h"
// #include "manipul_arq.h"


// /// @private @brief 
// void Le2LinhasBuscaEUpdate(FILE* file)
// {    
//     int m; scanf("%d", &m);
//     OQueBuscar oqbuscar;
//     preencherQuery(&oqbuscar, m); // PREENCHIMENTO DA QUERY "CHECKLIST"

//     int p; scanf("%d", &p);
//     OQueBuscar oqmudar;
//     preencherQuery(&oqmudar, p);
//     // ignorar o nome, é basicamente a mesma lógica de preencher a query "checklist" de o buscar, pra "checklist" de o que tem que dar update


//     // LEMBRAR DE CHAMAR DESALOCAÇÃO DE MEMÓRIA
    
//     // o que foi feito até agora foi só preencher a checklist de busca e do que se deve dar update 
//     /*
//         RESTO DA FUNÇÃO A FAZER: a busca em si (percorrer arquivo binário, e copiar cada registro do binário pra struct Registro)
//         (isso permite realizar comparações entre as checklist e os dados do registro de forma mais organizada)

//         usar a função que percorre e busca correspondencias pra salvar os offsets que satisfazem a busca num vetor alocado dinamicamente

//         e basicamnete, percorrer de offset em offset, fazendo os updates necessários

//         lembrar de dar free quando necessário

//         fechar o arquivo com função já criada (se der)
//     */ 


// }

// void update(const char* nomeArquivoBin, int n)
// {
//     // rotina ao abrir arquivo binário rb+
//     FILE* file = abrirVerificarInconsistentar(nomeArquivoBin);

//     for (int i=0; i<n; i++) 
//         Le2LinhasBuscaEUpdate(file);
    
// }
