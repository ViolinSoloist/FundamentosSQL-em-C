#include "serial.h"

// strsep versao windows (temporário) (por algum motivo, strsep não funciona mesmo com #include <string.h> ¯\_(ツ)_/¯)
char *strsep(char **stringp, const char *delim) {
    char *start = *stringp;
    char *p;
    
    if (start == NULL) {
        return NULL;
    }
    
    // procura o delimitador
    p = strpbrk(start, delim);
    if (p) {
        *p = '\0'; 
        *stringp = p + 1; 
    } else {
        *stringp = NULL; // fim da linha
    }
    
    return start;
}

void gravarRegistroBin(Registro* temporario, FILE* bin, char removido, int proxLista)
{
    // gravação dos dados fixos
    fwrite(&removido, sizeof(char), 1, bin);
    fwrite(&proxLista, sizeof(int), 1, bin);
    fwrite(&temporario->codEstacao, sizeof(int), 1, bin);
    fwrite(&temporario->codLinha, sizeof(int), 1, bin);
    fwrite(&temporario->codProxEstacao, sizeof(int), 1, bin);
    fwrite(&temporario->distProxEstacao, sizeof(int), 1, bin);
    fwrite(&temporario->codLinhaIntegra, sizeof(int), 1, bin);
    fwrite(&temporario->codEstIntegra, sizeof(int), 1, bin);

    int bytesEscritos = 29; // 1 char + (7 ints * 4 bytes)
    
    // --- dados variaveis ---
    
    // Tratamento seguro para Nome da Estação
    int tamNomeEstacao = 0;
    if (temporario->nomeEstacao != NULL) {
        tamNomeEstacao = strlen(temporario->nomeEstacao);
    }
    
    fwrite(&tamNomeEstacao, sizeof(int), 1, bin);
    if (tamNomeEstacao > 0) {
        fwrite(temporario->nomeEstacao, sizeof(char), tamNomeEstacao, bin); // sem o '\0'
    }
    bytesEscritos += 4 + tamNomeEstacao; 


    // Tratamento seguro para Nome da Linha
    int tamNomeLinha = 0;
    if (temporario->nomeLinha != NULL) {
        tamNomeLinha = strlen(temporario->nomeLinha);
    }
    
    fwrite(&tamNomeLinha, sizeof(int), 1, bin); 
    if (tamNomeLinha > 0) { 
        fwrite(temporario->nomeLinha, sizeof(char), tamNomeLinha, bin); 
    }
    bytesEscritos += 4 + tamNomeLinha; 

    // --- preenchimento do restante com lixo ---
    char lixo = '$';
    while (bytesEscritos < 80) {
        fwrite(&lixo, sizeof(char), 1, bin);
        bytesEscritos++;
    }
    
}

void binToStruct(Registro* regAtual, FILE* bin) {
    regAtual->removido = '0';

    // leitura pra struct Registro dos campos fixos (28 bytes: 7 * 4 )
    fread(&regAtual->proximo, sizeof(int), 1, bin);
    fread(&regAtual->codEstacao, sizeof(int), 1, bin);
    fread(&regAtual->codLinha, sizeof(int), 1, bin);
    fread(&regAtual->codProxEstacao, sizeof(int), 1, bin);
    fread(&regAtual->distProxEstacao, sizeof(int), 1, bin);
    fread(&regAtual->codLinhaIntegra, sizeof(int), 1, bin);
    fread(&regAtual->codEstIntegra, sizeof(int), 1, bin);
    
    int bytes_lidos = 29; // 1 char  + 7*4

    // Leitura do Nome da Estação
    fread(&regAtual->tamNomeEstacao, sizeof(int), 1, bin);
    bytes_lidos += 4;

    if (regAtual->tamNomeEstacao > 0) {
        // Aloca o tamanho exato + 1 para o '\0'
        regAtual->nomeEstacao = malloc(regAtual->tamNomeEstacao + 1);
        
        // Lê do arquivo direto para a memória alocada
        fread(regAtual->nomeEstacao, sizeof(char), regAtual->tamNomeEstacao, bin);
        regAtual->nomeEstacao[regAtual->tamNomeEstacao] = '\0';
        
        bytes_lidos += regAtual->tamNomeEstacao;
    } else {
        // Garante que o ponteiro fique NULL se não houver nome
        regAtual->nomeEstacao = NULL; 
    }

    // Leitura do Nome da Linha
    fread(&regAtual->tanNomeLinha, sizeof(int), 1, bin);
    bytes_lidos += 4;
    
    if (regAtual->tanNomeLinha > 0) {
        // Aloca o tamanho exato + 1 para o '\0'
        regAtual->nomeLinha = malloc(regAtual->tanNomeLinha + 1);
        
        // Lê do arquivo direto para a memória alocada
        fread(regAtual->nomeLinha, sizeof(char), regAtual->tanNomeLinha, bin);
        regAtual->nomeLinha[regAtual->tanNomeLinha] = '\0';
        
        bytes_lidos += regAtual->tanNomeLinha;
    } else {
        // Garante que o ponteiro fique NULL se não houver nome
        regAtual->nomeLinha = NULL;
    }
    // pular lixo
    fseek(bin, 80 - bytes_lidos, SEEK_CUR);
}

void salvaDadosNoRegistro(Registro* temporario, char* linha_entrada) 
{   
    // total é a linha inteira, após primeira execução de strsep, pedaco = tudo até a primeira virgula, total = todo restante
    char *total = linha_entrada;
    char *pedaco;
    
    pedaco = strsep(&total, ",");
    temporario->codEstacao = atoi(pedaco);

    pedaco = strsep(&total, ",");
    if (pedaco && *pedaco) {
        temporario->nomeEstacao = malloc(strlen(pedaco) + 1);
        strcpy(temporario->nomeEstacao, pedaco);
    } else {
        // se estiver vazio, aloca uma string vazia
        temporario->nomeEstacao = malloc(1);
        temporario->nomeEstacao[0] = '\0';
    }

    pedaco = strsep(&total, ",");
    temporario->codLinha = (pedaco && *pedaco) ? atoi(pedaco) : -1;

    pedaco = strsep(&total, ",");
    if (pedaco && *pedaco) {
        temporario->nomeLinha = malloc(strlen(pedaco) + 1);
        strcpy(temporario->nomeLinha, pedaco);
    } else {
        temporario->nomeLinha = malloc(1);
        temporario->nomeLinha[0] = '\0';
    }

    pedaco = strsep(&total, ",");
    temporario->codProxEstacao = (pedaco && *pedaco) ? atoi(pedaco) : -1;

    pedaco = strsep(&total, ",");
    temporario->distProxEstacao = (pedaco && *pedaco) ? atoi(pedaco) : -1;

    pedaco = strsep(&total, ",");
    temporario->codLinhaIntegra = (pedaco && *pedaco) ? atoi(pedaco) : -1;

    pedaco = strsep(&total, ",");
    temporario->codEstIntegra = (pedaco && *pedaco) ? atoi(pedaco) : -1;
}