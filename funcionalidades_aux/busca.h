#ifndef BUSCA_H
#define BUSCA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "estruturas.h" 
#include "fornecidas.h"
#include "cabecalho.h"
#include "serial.h"
#include "terminal.h"

/*
esse arquivo contém o mecanismo principal de busca e varredura que outras funções (delete, update, select, etc) vão usar
deixei separado pois como é algo usado em diferentes funções, é interessante modularizar esse mecanismo

a forma como a busca funciona, é basicamente usando uma struct (está em estruturas.h) OQueBuscar, como se fosse um "checklist":
depois de ler a entrada, salva nessa struct quais são os campos a serem pesquisados, marcando eles como true (os campos começam inicializados como false)
e quais são os valores buscados desse campo.

Então, na hora de percorrer o arquivo bin, vai de registro em registro, e em cada registro, copia os dados dele pra struct registro, e compara campo a campo da struct OQueBuscar
até achar o campo que esteja marcado como true (ou seja, campo que está sendo buscado), quando acha, ve se o valor dele corresponde com a da busca.
Faz isso para todos os campos, e se em qualquer um deles o valor dá diferente, já "falha" o match. Se passa por todos os critérios, quer dizer que atende à busca.
*/

typedef void (*AcaoPosBusca)(FILE* file, ArgumentosCallback* args);

// lê parâmetros da busca (seja pra deleção, select, etc) e marca na struct OQueBuscar os campos a serem buscados com true
/// @attention LEMBRAR DE DAR FREE NOS CAMPOS QUE CORRESPONDEM AOS NOMES (SÃO CAMPOS ALOCADOS DINAMICAMENTE) QND TERMINAR DE USAR O REGISTRO DE QUERY
void marcadorFlag(char* nomeCampo, OQueBuscar* Query);

// antes de marcar os campos buscados, inicializa todos como false
void zerarFlags(OQueBuscar* query);

void preencherQuery(OQueBuscar* oqbuscar, int m);

/**
 * @brief função genérica de busca, que assim que acha uma correspondência, pausa e faz um callback para executar a funcionalidade específica, depois continua normalmente)
 * @param qtd_encontrados é atualizado internamente.
*/
void percorreEBuscaCorrespondencia(FILE* bin, ArgumentosCallback* args);

/// @attention USAR NO FINAL DAS FUNCIONALIDADES QUE ALTERAM OS REGISTROS : INSERIR, DELETAR, ETC
/// @brief vasculha todos os registros de dados do arq bin para contar qntd estacoes e qntd de pares de estacoes para atualizar pós delete, insert, etc
// usa vetores de estacoes/pares vistos alocados dinamicamente para ir guardando os que já foram vistos
/// recebe arquivo @param bin 
void atualizarContadoresCabecalho(FILE* bin);

/**
 * @brief função auxiliar, le o registro atual do arq bin e copia os dados pra struct Registro
 * recebe ponteiro para @param regAtual e o arquivo @param bin
 */
void binToStruct(Registro* regAtual, FILE* bin);

#endif