#ifndef MANIPUL_ARQ_H
#define MANIPUL_ARQ_H

#include <stdio.h>
#include <stdbool.h>

// faz a rotina de abrir o arquivo binário (LEITURA + ESCRITA BINÁRIO), verificação e depois deixa como inconsistente
// lembrar de fechar arquivo (isso é basicamente um fopen() só que mais completo)
FILE* abrirVerificarInconsistentar(const char* nomeArquivoBin);

void finalizarArquivo(FILE* file, bool debuggar);

#endif