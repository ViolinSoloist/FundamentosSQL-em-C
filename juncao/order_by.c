#include "order_by.h"

int comparaRegistrosCodProxEstacao(const void* a, const void* b){
    const Registro* regA = (const Registro*)a;
    const Registro* regB = (const Registro*)b;
    return (regA->codProxEstacao - regB->codProxEstacao);
}

int comparaRegistrosCodEstacao(const void* a, const void* b){
    const Registro* regA = (const Registro*)a;
    const Registro* regB = (const Registro*)b;
    return (regA->codEstacao - regB->codEstacao);
}

void ordenaRegistros(Registro* todosRegistros, long RRN, const char* campo1){
    if (!strcmp(campo1, "codProxEstacao")){
        qsort(todosRegistros, RRN, sizeof(Registro), comparaRegistrosCodProxEstacao);
    }

    if (!strcmp(campo1, "codEstacao")){
        qsort(todosRegistros, RRN, sizeof(Registro), comparaRegistrosCodEstacao);
    }
}

void order_by(const char* nomeArquivoBinario, const char* campo1, const char* nomeArquivoBinarioOrdenado, bool binNaTela) {

    FILE* bin1 = rotinaAbrirArquivo(nomeArquivoBinario, LEITURA);
    if (bin1 == NULL) return;

    // Alocação dinâmica do vetor de registros baseada no tamanho do bin1
    fseek(bin1, 0, SEEK_END);
    long tamanhoArquivo = ftell(bin1);
    long maxRegistros = (tamanhoArquivo - OFFSET_INI_DADOS) / TAM_REGISTRO;
    Registro* todosRegistros = (Registro*)malloc(maxRegistros * sizeof(Registro));

    // Carrega os registros ativos para a memória
    fseek(bin1, OFFSET_INI_DADOS, SEEK_SET);
    long RRN = 0;
    char removido;
    while(fread(&removido, sizeof(char), 1, bin1) == 1){
        if (removido == '1'){
            fseek(bin1, TAM_REGISTRO-1, SEEK_CUR);
        } else {
            binToStruct(&todosRegistros[RRN], bin1);
            RRN += 1;
        }
    }

    fclose(bin1);

    // Ordena o vetor na memória RAM
    ordenaRegistros(todosRegistros, RRN, campo1);

    FILE* bin2 = fopen(nomeArquivoBinarioOrdenado, "wb+");
    if (bin2 == NULL) {
        return;
    }

    // Inicializa o cabeçalho do novo arquivo que vai estar ordenado
    Cabecalho cab2;
    cab2.status = '0';          
    cab2.topo = -1;            
    cab2.proxRRN = 0;          
    cab2.nroEstacoes = 0;      
    cab2.nroParesEstacao = 0;

    // Primeira passada: escreve a estrutura base do cabeçalho
    escreveCabecarioBin(false, bin2, &cab2); 

    // grava os registros ordenados no bin2
    fseek(bin2, OFFSET_INI_DADOS, SEEK_SET);
    for (long i = 0; i < RRN; i++) {
        char removido = '0';
        gravarRegistroBin(&todosRegistros[i], bin2, removido, -1);

        if (todosRegistros[i].nomeEstacao) free(todosRegistros[i].nomeEstacao);
        if (todosRegistros[i].nomeLinha) free(todosRegistros[i].nomeLinha);
    }
    
    // Configura os dados de finalização do arquivo
    cab2.proxRRN = RRN;        // O próximo RRN livre é exatamente a quantidade de registros gravados
    
    // Segunda passada: Atualiza o status e o proxRRN
    cab2.status = '1';
    escreveCabecarioBin(true, bin2, &cab2); 
    // Atualiza os contadores de cabecalho
    atualizarContadoresCabecalho(bin2);
    
    
    // Limpeza final de ponteiros e fechando os arquivos
    free(todosRegistros);
    fclose(bin2);
    
    if (binNaTela) BinarioNaTela((char*)nomeArquivoBinarioOrdenado);
}