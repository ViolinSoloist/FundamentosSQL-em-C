#include "stdio.h"
#include "stdbool.h"

#include "estruturas.h"
#include "fornecidas.h"
#include "busca.h"
#include "manipul_arq.h"
#include "select_all.h"
#include "bin_struct.h"

void mostrarRegistro(Registro* registro_lido){
    
    printf("%d ", registro_lido->codEstacao);
    printf("%s ", registro_lido->nomeEstacao);

    if (registro_lido->codLinha == -1) printf("NULO ");
    else printf("%d ", registro_lido->codLinha);

    if (registro_lido->tanNomeLinha > 0 && registro_lido->nomeLinha != NULL)
        printf("%s ", registro_lido->nomeLinha);
    else printf("NULO ");

    if (registro_lido->codProxEstacao == -1) printf("NULO ");
    else printf("%d ", registro_lido->codProxEstacao);

    if (registro_lido->distProxEstacao == -1) printf("NULO ");
    else printf("%d ", registro_lido->distProxEstacao);

    if (registro_lido->codLinhaIntegra == -1) printf("NULO ");
    else printf("%d ", registro_lido->codLinhaIntegra);

    if (registro_lido->codEstIntegra == -1) printf("NULO\n");
    else printf("%d\n", registro_lido->codEstIntegra);
}


/// @brief Função principal de recuperação de registros
/// @param nomeArquivoBin Arquivo binário do qual os registros serão recuperados
void select_all(const char* nomeArquivoBin){
    // rotina ao abrir arquivo binário rb+
    FILE* file = abrirVerificarInconsistentar(nomeArquivoBin);


    int existe_registro = false;

    fseek(file, 17, SEEK_SET);

    char removido;
    while (fread(&removido, sizeof(char), 1, file) == 1){
        if (removido == '0'){
            Registro reg_lido;
            binToStruct(&reg_lido, file);

            mostrarRegistro(&reg_lido);

            existe_registro = true;

        } else {
            fseek(file, 79, SEEK_CUR);
        }
    }

    if (!existe_registro)
        fprintf(stderr, "Registro inexistente\n");

    finalizarArquivo(file, false);
}