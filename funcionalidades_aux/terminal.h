#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "estruturas.h"
#include "busca.h"
#include "fornecidas.h"

// arquivo para isolar toda a lógica que envolve ler dados de entrada e imprimir resultados no terminal.

/// @brief le todos os dados necessários para escrever um único registro completo (usado em INSERT INTO)
void lerRegistro(Registro* temp);

/// @brief lê o que se procura, usado em DELETE, SELECT WHERE, etc
void lerLinhaBusca(FILE* file, ArgumentosCallback* args);

/// @brief imprime os dados de um @param registro formatado (USADO EM SELECT)
void mostrarRegistro(Registro* registro_lido);

#endif