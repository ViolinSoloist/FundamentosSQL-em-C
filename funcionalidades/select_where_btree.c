#include "select_where_btree.h"

/* Callback chamado pelo percorreEBuscaCorrespondencia quando encontra (ou não) registros correspondentes a busca.
   Imprime os registros encontrados ou mensagem de inexistente caso nenhum seja achado */
void acaoImprimirRegistros(FILE* file, int qtd_encontrados, long* offsets, void* dados_extras){
    (void)dados_extras;

    // Se não achou nenhum registro correspondente, imprime mensagem e retorna
    if (qtd_encontrados == 0) {
        printf("Registro inexistente.\n\n");
        return;
    }

    fseek(file, *offsets, SEEK_SET); // Pula direto pro byte de início do registro
    
    char removido;
    fread(&removido, sizeof(char), 1, file); // Consome o byte 'removido' antes de ler os campos
    
    Registro reg_atual;
    binToStruct(&reg_atual, file);  // Lê os campos do registro para a struct
    mostrarRegistro(&reg_atual);    // Imprime os campos do registro formatados

    // Libera os campos alocados dinamicamente da struct
    if (reg_atual.nomeEstacao) free(reg_atual.nomeEstacao);
    if (reg_atual.nomeLinha) free(reg_atual.nomeLinha);
}

/* Funcionalidade [8]: busca registros no arquivo de dados que satisfaçam um critério de busca.
   Se o critério incluir codEstacao, usa o índice árvore-B para busca direta pelo offset.
   Caso contrário, faz varredura sequencial usando o mecanismo de busca do trabalho anterior. */
void select_where_btree(const char* nomeArquivoBin, const char* nomeArquivoArvoreBin, int numeroBuscas){

    // Abre os arquivos verificando inconsistência de status
    FILE* bin = abrirVerificarInconsistentar(nomeArquivoBin);
    FILE* arv = abrirVerificarInconsistentar(nomeArquivoArvoreBin);
    if (bin == NULL || arv == NULL) return;

    // Lê o cabeçalho do arquivo de índice para a struct
    CabecalhoArvore cab;
    lerCabecalhoArvore(arv, &cab);

    OQueBuscar query;

    // Faz as n buscas
    for (int i = 0; i < numeroBuscas; i++){
        int m;
        scanf("%d", &m);

        // Lê os m pares nomeCampo/valorCampo da entrada e preenche a struct de busca
        preencherQuery(&query, m);
        
        // Se a busca inclui codEstacao, usa o índice árvore-B para busca direta
        if(query.checar_codEstacao){
            long offset = ArvoreBuscar(arv, &cab, query.valores.codEstacao);

            // Se não achou a chave na árvore, imprime mensagem e continua pro próximo critério
            if (offset == -1){
                printf("Registro Inexistente.\n");
                continue;
            } else {
                fseek(bin, offset, SEEK_SET);   // Vai direto ao offset do registro no arquivo de dados
                char removido;
                fread(&removido, sizeof(char), 1, bin); // Consome o byte 'removido'
                Registro temp;
                binToStruct(&temp, bin);    // Lê os campos do registro
                mostrarRegistro(&temp);     // Imprime o registro
            }

        // Se a busca não inclui codEstacao, faz varredura sequencial
        } else {
            ArgumentosCallback args;
            args.callback = acaoImprimirRegistros;  // Callback que imprime os registros encontrados
            args.dados_extras = NULL;
            args.query = query;                     // Critério de busca a ser aplicado em cada registro
            percorreEBuscaCorrespondencia(bin, &args);
        }
    }
}