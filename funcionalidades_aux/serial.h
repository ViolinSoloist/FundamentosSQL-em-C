#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "estruturas.h"

/* 
SERIALIZAÇÃO - funções relacionadas com passagem de dados entre Disco -> RAM e vice-versa
*/

char *strsep(char **stringp, const char *delim); // mesma funcionalidade do strsep do string.h, mas por algum motivo esse não funciona

/// @brief pega todas as coisas salvas na struct Registro buffer (copiadas do csv) e escreve no arquivo binario
void gravarRegistroBin(Registro* temporario, FILE* bin, char removido, int proxLista); 

/**
 * @brief função auxiliar, le o registro atual do arq bin e copia os dados pra struct Registro
 * recebe ponteiro para @param regAtual e o arquivo @param bin
 */
void binToStruct(Registro* regAtual, FILE* bin);

/// @brief pega os dados do csv pra salvar num registro intermediário, ai usa esse registro pra escrever no binário
// forma mais organizada, ao custo de uso de memória pequeno constante (O(1)) independente da quantidade de registros
void salvaDadosNoRegistro(Registro* temporario, char* linha_entrada);

#endif
