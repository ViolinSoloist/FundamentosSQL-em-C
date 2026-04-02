#include <stdio.h>
#include <stdlib.h>

#include "estruturas.h" 
#include "fornecidas.h"
#include "busca.h"
#include "delete.h"

#define MAX_NOMECAMPO 50

void deletar(const char *nomeArquivoBin, int n) {
    // abrindo e verificando o arquivo
    FILE* file = fopen(nomeArquivoBin, "rb+");
    if (file == NULL) {
        fprintf(stderr, "Falha no processamento do arquivo.\n");
        return;
    }

    // verifica consistencia do arquivo
    char status;
    fread(&status, sizeof(char), 1, file);
    if (status == '0') {
        fprintf(stderr, "Falha no processamento do arquivo.\n");
        fclose(file);
        return;
    }

    // se tá tudo certo, então deixa inconsistente, uma vez que está sendo editado
    status = '0';
    fseek(file, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file);

    // salva o que está no topo (necessário para deleção)
    int topo;
    fseek(file, 1, SEEK_SET);
    fread(&topo, sizeof(int), 1, file);
    
    // loop
    for (int i=0; i<n; i++) {
        int m;
        scanf("%d", &m);

        OQueBuscar oqbuscar;

        // zera todas as flags antes de marcar as que vao ser buscadas como true
        zerarFlags(&oqbuscar);

        for (int j=0; j<m; j++) {
            char nomeCampo[MAX_NOMECAMPO];
            scanf("%s", nomeCampo);
            marcadorFlag(nomeCampo, &oqbuscar);
        }

        // 1. CHAMA A BUSCA GENÉRICA
        int qtd_encontrados = 0;
        long* offsets = percorre_e_buscaCorrespondencia(file, oqbuscar, &qtd_encontrados);

        // 2. SE ACHOU ALGUÉM, APLICA A MATEMÁTICA DE DELEÇÃO
        if (qtd_encontrados > 0 && offsets != NULL) {
            for (int k = 0; k < qtd_encontrados; k++) {
                long posicao_registro = offsets[k];

                int rrn_atual = (posicao_registro - 17) / 80;

                // a) Vai para o exato byte inicial do registro
                fseek(file, posicao_registro, SEEK_SET);

                // b) Sobrescreve com '1' (removido)
                char marca = '1';
                fwrite(&marca, sizeof(char), 1, file);

                // c) Grava o 'topo' da memória no campo 'proximo' do registro
                fwrite(&topo, sizeof(int), 1, file);

                // d) Atualiza a variável da memória (O novo topo é este RRN/Byte Offset)
                topo = rrn_atual; 
            }
            free(offsets); // Libera o vetor de endereços
        }

        if (oqbuscar.valores.nomeEstacao) free(oqbuscar.valores.nomeEstacao);
        if (oqbuscar.valores.nomeLinha) free(oqbuscar.valores.nomeLinha);
    }

    // escreve topo atualizado no cabeçario bin
    fseek(file, 1, SEEK_SET);
    fwrite(&topo, sizeof(int), 1, file);

    atualizar_contadores_cabecalho(file);

    // edições finalizadas => consistente novamente
    status = '1';
    fseek(file, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, file);
    
    // debug
    int nEst, nPares;
    fseek(file, 9, SEEK_SET); // Pula status(1) + topo(4) + proxRRN(4) = byte 9
    fread(&nEst, sizeof(int), 1, file);
    fread(&nPares, sizeof(int), 1, file);
    printf("DEBUG - Estacoes: %d | Pares: %d\n", nEst, nPares);

    fclose(file);

    BinarioNaTela((char*)nomeArquivoBin); 
}