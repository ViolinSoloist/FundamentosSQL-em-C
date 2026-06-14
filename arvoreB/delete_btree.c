#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "busca.h"
#include "estruturas.h"
#include "arvoreB.h"
#include "manipul_arq.h"
#include "terminal.h"

/// @details após todas as remoções lógicas, precisamos recontar as estações únicas e pares únicos
/// @param fileDados 
static void recalcularContadores(FILE* fileDados) {
    fseek(fileDados, 17, SEEK_SET); // Pula o cabeçalho
    Registro* ativos = NULL;
    int numAtivos = 0;
    char removido;
    
    // itera pelo arquivo e salva apenas os registros não removidos
    while (fread(&removido, sizeof(char), 1, fileDados) == 1) {
        Registro reg;
        binToStruct(&reg, fileDados);
        
        if (removido == '0') {  // SE NÃO REMOVIDO
            ativos = realloc(ativos, (numAtivos + 1) * sizeof(Registro));
            ativos[numAtivos] = reg;
            numAtivos++;
        } else {                // SE REMOVIDO
            if (reg.nomeEstacao) free(reg.nomeEstacao);
            if (reg.nomeLinha) free(reg.nomeLinha);
        }
    }
    
    int nroEstacoes = 0;
    int nroPares = 0;
    
    for (int i = 0; i < numAtivos; i++) {

        // ---------- CONTAGEM ---------------------
        // estações únicas
        if (ativos[i].nomeEstacao != NULL && strlen(ativos[i].nomeEstacao) > 0) {
            bool repetido = false;
            for (int j = 0; j < i; j++) {
                if (ativos[j].nomeEstacao != NULL && strcmp(ativos[i].nomeEstacao, ativos[j].nomeEstacao) == 0) {
                    repetido = true;
                    break;
                }
            }
            if (!repetido) nroEstacoes++;
        }
        
        // pares únicos
        if (ativos[i].codEstacao != -1 && ativos[i].codProxEstacao != -1) {
            bool repetido = false;
            for (int j = 0; j < i; j++) {
                if (ativos[i].codEstacao == ativos[j].codEstacao && 
                    ativos[i].codProxEstacao == ativos[j].codProxEstacao) {
                    repetido = true;
                    break;
                }
            }
            if (!repetido) nroPares++;
        }
    }

    // gravação cabeçalho: nroEstacoes (offset 9) nroParesEstacoes (offset13)
    fseek(fileDados, 9, SEEK_SET);
    fwrite(&nroEstacoes, sizeof(int), 1, fileDados);
    fwrite(&nroPares, sizeof(int), 1, fileDados);
    
    // desalocação de memória antes de terminar a função
    for (int i = 0; i < numAtivos; i++) {
        if (ativos[i].nomeEstacao) free(ativos[i].nomeEstacao);
        if (ativos[i].nomeLinha) free(ativos[i].nomeLinha);
    }
    if (ativos) free(ativos);
}


/// @brief FUNÇÕES DE FILTRO E EXECUÇÃO
static bool registroPassaFiltro(Registro* reg, OQueBuscar* query) { 

    if (query->checar_codEstacao && reg->codEstacao != query->valores.codEstacao) return false;
    if (query->checar_codLinha && reg->codLinha != query->valores.codLinha) return false;
    if (query->checar_codProxEstacao && reg->codProxEstacao != query->valores.codProxEstacao) return false;
    if (query->checar_distProxEstacao && reg->distProxEstacao != query->valores.distProxEstacao) return false;
    if (query->checar_codLinhaIntegra && reg->codLinhaIntegra != query->valores.codLinhaIntegra) return false;
    if (query->checar_codEstIntegra && reg->codEstIntegra != query->valores.codEstIntegra) return false;
    
    if (query->checar_nomeEstacao) {
        if (reg->nomeEstacao == NULL || strcmp(reg->nomeEstacao, query->valores.nomeEstacao) != 0) return false;
    }
    if (query->checar_nomeLinha) {
        if (reg->nomeLinha == NULL || strcmp(reg->nomeLinha, query->valores.nomeLinha) != 0) return false;
    }
    return true;
}

static void executarRemocaoPorCriterio(FILE* fileDados, FILE* fileIndice, CabecalhoArvore* cabIndice, OQueBuscar* query) {
    
    // busca chave (codEstacao)? => Usa Arvore
    if (query->checar_codEstacao) {
    
        long byteOffset = ArvoreBuscar(fileIndice, cabIndice, query->valores.codEstacao);
        
        if (byteOffset != -1) { 
            fseek(fileDados, byteOffset, SEEK_SET);
            char removido;
            fread(&removido, sizeof(char), 1, fileDados);
            
            Registro reg;
            binToStruct(&reg, fileDados); 
            
            if (removido == '0' && registroPassaFiltro(&reg, query)) {
                
                // convete pra RRN pra empilhar no topo
                int rrn_dados = (byteOffset - 17) / 80;

                int topo_dados;
                fseek(fileDados, 1, SEEK_SET);
                fread(&topo_dados, sizeof(int), 1, fileDados);
                
                fseek(fileDados, byteOffset, SEEK_SET);
                char marcaRemovido = '1';
                fwrite(&marcaRemovido, sizeof(char), 1, fileDados);
                fwrite(&topo_dados, sizeof(int), 1, fileDados);
                
                fseek(fileDados, 1, SEEK_SET);
                fwrite(&rrn_dados, sizeof(int), 1, fileDados);
                
                // remoção na árbore (boa sorte)
                ArvoreRemover(fileIndice, cabIndice, reg.codEstacao);
            }
            
            if (reg.nomeEstacao) { free(reg.nomeEstacao); reg.nomeEstacao = NULL; }
            if (reg.nomeLinha) { free(reg.nomeLinha); reg.nomeLinha = NULL; }
        }
    } 

    // qualquer outra coisa sendo buscada? usa iteração pelo arquivo normal (chega de árvore plmds)
    else {
        fseek(fileDados, 17, SEEK_SET);
        int rrn_atual = 0;
        char removido;
        Registro reg;
        
        while (fread(&removido, sizeof(char), 1, fileDados) == 1) {
            binToStruct(&reg, fileDados); 
            
            if (removido == '0' && registroPassaFiltro(&reg, query)) {
                int topo_dados;
                fseek(fileDados, 1, SEEK_SET);
                fread(&topo_dados, sizeof(int), 1, fileDados);
                
                long byteOffset = 17 + (rrn_atual * 80);
                fseek(fileDados, byteOffset, SEEK_SET);
                char marcaRemovido = '1';
                fwrite(&marcaRemovido, sizeof(char), 1, fileDados);
                fwrite(&topo_dados, sizeof(int), 1, fileDados);
                
                fseek(fileDados, 1, SEEK_SET);
                fwrite(&rrn_atual, sizeof(int), 1, fileDados);
                
                // usando arvore ou não pra achar, se você removeu tem que tirar da arvore também
                ArvoreRemover(fileIndice, cabIndice, reg.codEstacao);
            }
            
            if (reg.nomeEstacao) { free(reg.nomeEstacao); reg.nomeEstacao = NULL;} // Remoção Física na Árvore-B
            if (reg.nomeLinha) { free(reg.nomeLinha); reg.nomeLinha = NULL; }
            
            rrn_atual++;
            fseek(fileDados, 17 + (rrn_atual * 80), SEEK_SET); 
        }
    }
}

/// @brief FUNÇÃO PRINCIPAL
/// @param nomeArquivoDados 
/// @param nomeArquivoIndice 
/// @param n 
void delete_btree(const char* nomeArquivoDados, const char* nomeArquivoIndice, int n) {
    FILE* fileDados = rotinaAbrirArquivo(nomeArquivoDados, ESCRITA);
    if (fileDados == NULL) return;

    FILE* fileIndice = rotinaAbrirArquivo(nomeArquivoIndice, ESCRITA);
    if (fileIndice == NULL) {
        finalizarArquivoEscrita(fileDados, false);
        return;
    }

    CabecalhoArvore cabIndice;
    lerCabecalhoArvore(fileIndice, &cabIndice);

    for (int i = 0; i < n; i++) {
        OQueBuscar query;
        memset(&query, 0, sizeof(OQueBuscar));
        
        int m;
        scanf("%d", &m); 
        
        /// @attention GAMBIARRA AFRENTE!!!!!!!!! TEMPORÁRIO PARA CONTORNAR BUG DO ScanQuoteString
        for (int j = 0; j < m; j++) {
            char nomeCampo[50];
            scanf("%s", nomeCampo);

            if (strcmp(nomeCampo, "codEstacao") == 0) {
                query.checar_codEstacao = true;
                scanf("%d", &query.valores.codEstacao);
            } else if (strcmp(nomeCampo, "codLinha") == 0) {
                query.checar_codLinha = true;
                scanf("%d", &query.valores.codLinha);
            } else if (strcmp(nomeCampo, "codProxEstacao") == 0) {
                query.checar_codProxEstacao = true;
                scanf("%d", &query.valores.codProxEstacao);
            } else if (strcmp(nomeCampo, "distProxEstacao") == 0) {
                query.checar_distProxEstacao = true;
                scanf("%d", &query.valores.distProxEstacao);
            } else if (strcmp(nomeCampo, "codLinhaIntegra") == 0) {
                query.checar_codLinhaIntegra = true;
                scanf("%d", &query.valores.codLinhaIntegra);
            } else if (strcmp(nomeCampo, "codEstIntegra") == 0) {
                query.checar_codEstIntegra = true;
                scanf("%d", &query.valores.codEstIntegra);
            } else if (strcmp(nomeCampo, "nomeEstacao") == 0) {
                query.checar_nomeEstacao = true;
                char buffer[200];
                ScanQuoteString(buffer);
                query.valores.nomeEstacao = malloc(strlen(buffer) + 1);
                strcpy(query.valores.nomeEstacao, buffer);
            } else if (strcmp(nomeCampo, "nomeLinha") == 0) {
                query.checar_nomeLinha = true;
                char buffer[200];
                ScanQuoteString(buffer);
                query.valores.nomeLinha = malloc(strlen(buffer) + 1);
                strcpy(query.valores.nomeLinha, buffer);
            }
        }
        
        executarRemocaoPorCriterio(fileDados, fileIndice, &cabIndice, &query);

        if (query.checar_nomeEstacao && query.valores.nomeEstacao) free(query.valores.nomeEstacao);
        if (query.checar_nomeLinha && query.valores.nomeLinha) free(query.valores.nomeLinha);
    }

    // finalização
    recalcularContadores(fileDados);

    finalizarArquivoEscrita(fileDados, false);
    
    cabIndice.status = '1';
    gravarCabecalhoArvore(fileIndice, &cabIndice);
    fclose(fileIndice);

    BinarioNaTela((char*)nomeArquivoDados);
    BinarioNaTela((char*)nomeArquivoIndice);
}