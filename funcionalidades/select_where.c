#include "select_where.h"

#define DEBUGGAR false

void acaoImprimirRegistros(FILE* file, int qtd_encontrados, long* offsets, void* dados_extras){
    (void)dados_extras;

    if (qtd_encontrados == 0) {
        printf("Registro inexistente.\n\n");
        return;
    }

    fseek(file, *offsets, SEEK_SET); // Pula direto pro byte do registro
    
    char removido;
    fread(&removido, sizeof(char), 1, file); // Consome o byte 'removido'
    
    Registro reg_atual;
    binToStruct(&reg_atual, file); 
    mostrarRegistro(&reg_atual);

    if (reg_atual.nomeEstacao) free(reg_atual.nomeEstacao);
    if (reg_atual.nomeLinha) free(reg_atual.nomeLinha);
}

void select_where(const char* nomeArquivoBin, int numeroBuscas){

    FILE* file = abrirVerificarInconsistentar(nomeArquivoBin);
    if (file == NULL) return;
    
    for (int i = 0; i < numeroBuscas; i++){
        lerLinhaBusca(file, acaoImprimirRegistros, NULL);
        if (i == 0 || i != numeroBuscas - 1)
            printf("\n");
    }

    finalizarArquivo(file, DEBUGGAR);
}