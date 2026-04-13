#ifndef CABECALHO_H
#define CABECALHO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "estruturas.h"

/*
responsável por todas funções que mexem com o cabeçalho: escrita, contagem de estacoes/pares, atualização de contagem/pares
*/

/// @brief inicializa a escrita de dados no arquivo bin, começando com o registro de cabeçalho
void escreveCabecarioBin(bool seek_inicio, FILE* bin, Cabecalho* head);

void contarEstacoesEPares(Registro* temporario, CamposUsados* campos);

// grava valores atualizados no cabeçalho
void gravaEFinaliza(FILE* bin, CamposUsados* campo);

/// @attention USAR NO FINAL DAS FUNCIONALIDADES QUE ALTERAM OS REGISTROS : INSERIR, DELETAR, ETC
/// @brief vasculha todos os registros de dados do arq bin para contar qntd estacoes e qntd de pares de estacoes para atualizar pós delete, insert, etc
// usa vetores de estacoes/pares vistos alocados dinamicamente para ir guardando os que já foram vistos
/// recebe arquivo @param bin 
void atualizarContadoresCabecalho(FILE* bin);

#endif