#include "create_table.h"

#define N_MAX_NAMES 1000
#define LENGTH_MAX_NAMES 100


void create_table(const char *nomeArquivoCSV, const char *nomeArquivoBin)
{
    FILE *csv = fopen(nomeArquivoCSV, "r");
    FILE *bin = fopen(nomeArquivoBin, "wb");

    // verificação
    if (csv == NULL || bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        if (csv) fclose(csv);
        if (bin) fclose(bin);
        return;
    }

    // -------------- começa escrevendo o cabeçalho do arquivo bin ----------------
    Cabecalho cab;
    cab.status = '0';                   // começa inconsistente
    cab.topo = -1;
    cab.proxRRN = 0;                    // próximo RRN disponível (começa zerado)
    // valores zerados mas que ainda antes da função terminar serão preenchidos:
    cab.nroEstacoes = 0;                // qntd de estacoes diferentes
    cab.nroParesEstacao = 0;
    // não precisa dar seek pro inicio
    bool seek_inicio = false; escreveCabecarioBin(seek_inicio, bin, &cab);

    // skip header
    char linha[500];            // bufer linha, toda linha lida é salva temporáriamente aqui
    fgets(linha, sizeof(linha), csv); 


    // ----- inicialização de variáveis/estruturas para guardar contagem de estações e pares -------

    CamposUsados cmps;

    // contagem de pares por nomes distintos (não pode ser ID porque contaria estações com mesmo nome mas em linhas diferentes (tipo luz, que faz parte de 4 linhas?))
    char* nomesVistos[2000];
    
    Par paresVistos[5000];

    cmps.nomes_vistos = nomesVistos;
    cmps.qtd_estacoes = 0;
    cmps.pares_vistos = paresVistos;
    cmps.qtd_pares = 0;

    // --------- LEITURA CSV + GUARDAR DADOS -----------

    while (fgets(linha, sizeof(linha), csv) != NULL) {
        
        // ------------------- LEITURA DE UMA LINHA DO CSV -------------------------------

        // tratamento de quebra de linhas (windows, linux)
        linha[strcspn(linha, "\n")] = '\0';
        linha[strcspn(linha, "\r")] = '\0';

        // tratamento linah "fantasma"
        if (strlen(linha) == 0) continue; 

        // --------------------------------------------- SALVA DADOS DO CSV EM UM REGISTRO TEMPORÁRIO (BUFFER) -------------------------------------
        Registro temporario;
        salvaDadosNoRegistro(&temporario, linha);
        
        // ---------------- CONTAGEM DE ESTAÇÕES E PARES -----------------------
        contarEstacoesEPares(&temporario, &cmps);

        // --------------- GRAVAÇÃO NO ARQUIVO BIN -------------------
        // setup dados fixos 
        char removido = '0'; // não removido
        int proxLista = -1;  // RRN do próximo removido (-1 inicial)
        gravarRegistroBin(&temporario, bin, removido, proxLista);
        cab.proxRRN++;

        // liberar memória dos campos variáveis do registro temporário ao fim do uso
        free(temporario.nomeEstacao);
        free(temporario.nomeLinha);
    }
    
    cab.status = '1';                           // volta para o primeiro byte e marca como consistente
    cab.nroEstacoes = cmps.qtd_estacoes;
    cab.nroParesEstacao = cmps.qtd_pares;
    seek_inicio = true; // true quando é a segunda vez escrevendo no cabecario, e necessita de um seek
    escreveCabecarioBin(seek_inicio, bin, &cab);
    
    // limpeza de memória do espaço alocado pros nomes no vetor de nomes já vistos
    for (int i = 0; i < cmps.qtd_estacoes; i++) 
        free(nomesVistos[i]);

    // debug
    // printf("DEBUG - proxRRN: %d | Estacoes: %d | Pares: %d\n", proxRRN, totalEstacoes, totalPares);

    fclose(csv);
    fclose(bin);

    BinarioNaTela((char *)nomeArquivoBin);
}