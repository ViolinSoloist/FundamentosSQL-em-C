#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "estruturas.h"
#include "IO.h"
#include "fornecidas.h"
#include "busca.h"
#include "update.h"
#include "manipul_arq.h"

#define DEBUGGAR false

/// @private @brief 
void Le2LinhasBuscaEUpdate(FILE* file)
{    
    
    int m; scanf("%d", &m);
    OQueBuscar oqbuscar;
    preencherQuery(&oqbuscar, m); // PREENCHIMENTO DA QUERY "CHECKLIST"

    int p; scanf("%d", &p);
    OQueBuscar oqmudar;
    preencherQuery(&oqmudar, p);
    // ignorar o nome, é basicamente a mesma lógica de preencher a query "checklist" de o buscar, pra "checklist" de o que tem que dar update

    int qtd_encontrados = 0;
    long* offsets = percorreEBuscaCorrespondencia(file, oqbuscar, &qtd_encontrados);

    for (int i = 0; i < qtd_encontrados; i++){
        Registro temp = {0};

        fseek(file, offsets[i], SEEK_SET);
        binToStruct(&temp, file);           // binToStruct aloca memória dinamicamente para os nomes -> N ESQUECER DE DAR FREE

        if (oqmudar.checar_codEstacao) 
            temp.codEstacao = oqmudar.valores.codEstacao;
            
        if (oqmudar.checar_codLinha) 
            temp.codLinha = oqmudar.valores.codLinha;
            
        if (oqmudar.checar_codProxEstacao) 
            temp.codProxEstacao = oqmudar.valores.codProxEstacao;
            
        if (oqmudar.checar_distProxEstacao) 
            temp.distProxEstacao = oqmudar.valores.distProxEstacao;
            
        if (oqmudar.checar_codLinhaIntegra) 
            temp.codLinhaIntegra = oqmudar.valores.codLinhaIntegra;
            
        if (oqmudar.checar_codEstIntegra) 
            temp.codEstIntegra = oqmudar.valores.codEstIntegra;

        // Para strings, precisamos liberar a memória da string antiga antes de alocar a nova
        if (oqmudar.checar_nomeEstacao) {
            free(temp.nomeEstacao);
            temp.nomeEstacao = malloc(strlen(oqmudar.valores.nomeEstacao) + 1);
            strcpy(temp.nomeEstacao, oqmudar.valores.nomeEstacao);
        }

        if (oqmudar.checar_nomeLinha) {
            free(temp.nomeLinha);
            temp.nomeLinha = malloc(strlen(oqmudar.valores.nomeLinha) + 1);
            strcpy(temp.nomeLinha, oqmudar.valores.nomeLinha);
        }

        fseek(file, offsets[i], SEEK_SET);
        gravarRegistroBin(&temp, file, temp.removido, temp.proximo);

        if (temp.nomeEstacao) free (temp.nomeEstacao);
        if (temp.nomeLinha) free(temp.nomeLinha);

    }

    free(offsets);

    if (oqbuscar.valores.nomeEstacao) free(oqbuscar.valores.nomeEstacao);
    if (oqbuscar.valores.nomeLinha) free(oqbuscar.valores.nomeLinha);

    if (oqmudar.valores.nomeEstacao) free(oqmudar.valores.nomeEstacao);
    if (oqmudar.valores.nomeLinha) free(oqmudar.valores.nomeLinha);

}

void update(const char* nomeArquivoBin, int n)
{
    // rotina ao abrir arquivo binário rb+
    FILE* file = abrirVerificarInconsistentar(nomeArquivoBin);

    for (int i=0; i<n; i++) 
        Le2LinhasBuscaEUpdate(file);

    atualizarContadoresCabecalho(file);
    
    finalizarArquivo(file, DEBUGGAR);

    BinarioNaTela((char*)nomeArquivoBin);
}
