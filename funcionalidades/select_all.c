#include "select_all.h"

/// @brief Função principal de recuperação de registros
/// @param nomeArquivoBin Arquivo binário do qual os registros serão recuperados
void select_all(const char* nomeArquivoBin){
    // rotina ao abrir arquivo binário rb+
    FILE* file = abrirVerificarInconsistentar(nomeArquivoBin);
    if (file == NULL) return;


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
        printf("Registro inexistente\n");

    finalizarArquivo(file, false);
}