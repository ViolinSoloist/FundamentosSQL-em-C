#include "stdio.h"
#include "stdbool.h"

#include "select_where.h"
#include "select_all.h"
#include "estruturas.h"
#include "busca.h"
#include "manipul_arq.h"
#include "IO.h"

#define DEBUGGAR false

void acaoImprimirRegistros(FILE* file, int qtd_encontrados, long* offsets, void* dados_extras){
    (void)dados_extras;

    if (qtd_encontrados == 0) {
        printf("Registro inexistente.\n\n");
        return;
    }

    Registro reg_atual;
    for (int i = 0; i < qtd_encontrados; i++) {
        fseek(file, offsets[i], SEEK_SET); // Pula direto pro byte do registro
        
        char removido;
        fread(&removido, sizeof(char), 1, file); // Consome o byte 'removido'
        
        binToStruct(&reg_atual, file); 
        mostrarRegistro(&reg_atual);
    }
    printf("\n");
}

void select_where(const char* nomeArquivoBin, int numeroBuscas){

    FILE* file = abrirVerificarInconsistentar(nomeArquivoBin);
    if (file == NULL) return;
    
    for (int i = 0; i < numeroBuscas; i++){
        lerLinhaBusca(file, acaoImprimirRegistros, NULL);
    }

    finalizarArquivo(file, DEBUGGAR);
}