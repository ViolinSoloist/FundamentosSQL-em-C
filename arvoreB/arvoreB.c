#include "arvoreB.h"

// ------------------------------------------------------------------------------------------------------
// Implementação das funções para criar o arquivo de índices da Árvore-B e para criar um nó da Árvore-B
// ------------------------------------------------------------------------------------------------------
void ArvoreCriar(FILE* arv, CabecalhoArvore * cab) {
    if (arv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Inicializa o cabeçalho da árvore
    cab->status = '0';
    cab->noRaiz = -1; // -1 indica que a árvore está vazia
    cab->topo = -1; // -1 indica que não há nós
    cab->proxRRN = 0; // RRN começa em 0
    cab->nroNos = 0; // Número de nós começa em 0
    return;
}

void NoCriar(NoArvore* no, int tipoNo) {
    if (no == NULL) {
        return;
    }

    no->removido = '0';   // 0 indica que o nó não está removido
    no->proximo = -1;   // guarda o RRN do próximo nó da pilha de removidos, -1 indica que não há próximo
    no->tipoNo = tipoNo;    // -1 indica nó folha, 0 indica nó raiz e 1 indica nó interno. -1 é quando é folha e raiz ao mesmo tempo
    no->nroChaves = 0;  // número de chaves começa em 0
    
    // Inicializa as chaves com -1 (indicando que estão vazias)
    for (int i = 0; i < MAX_NOS; i++) {
        no->Chaves[i] = -1;
    }

    // Inicializa os ponteiros de registro com -1 (indicando que estão vazios)
    for (int i = 0; i < MAX_NOS; i++) {
        no->Pr[i] = -1;
    }

    // Inicializa os ponteiros para os filhos com -1 (indicando que estão vazios)
    for (int i = 0; i < MAX_PONTEIROS; i++) {
        no->P[i] = -1;
    }
}

// ------------------------------------------------------------------------------------------------------
// Implementação das funções de leitura e escrita do cabeçalho da Árvore-B e dos nós da Árvore-B
// ------------------------------------------------------------------------------------------------------

// Grava o conteudo da struct cabeçaho pro arquivo de indices
void gravarCabecalhoArvore(FILE* arv, CabecalhoArvore* cab){
    if (arv == NULL || cab == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    fseek(arv, 0, SEEK_SET); // Move o cursor para o início do arquivo

    // Grava os campos do cabeçalho no arquivo
    fwrite(&cab->status, sizeof(char), 1, arv);
    fwrite(&cab->noRaiz, sizeof(int), 1, arv);
    fwrite(&cab->topo, sizeof(int), 1, arv);
    fwrite(&cab->proxRRN, sizeof(int), 1, arv);
    fwrite(&cab->nroNos, sizeof(int), 1, arv);
}

// Le do arquivo de indices para a struct cabeçalho
void lerCabecalhoArvore(FILE* arv, CabecalhoArvore* cab){
    if (arv == NULL || cab == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    fseek(arv, 0, SEEK_SET); // Move o cursor para o início do arquivo

    // Le os campos do cabeçalho do arquivo para a struct
    fread(&cab->status, sizeof(char), 1, arv);
    fread(&cab->noRaiz, sizeof(int), 1, arv);
    fread(&cab->topo, sizeof(int), 1, arv);
    fread(&cab->proxRRN, sizeof(int), 1, arv);
    fread(&cab->nroNos, sizeof(int), 1, arv);

}   

// Grava o conteudo da struct No pro arquivo de indices
void gravarNoArvore(FILE* arv, NoArvore* no, int RRN) {
    if (arv == NULL || no == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Calcula o deslocamento para o RRN especificado
    long offset = TAM_CABECALHO + RRN * TAM_NO;
    fseek(arv, offset, SEEK_SET); // Move o cursor para a posição do nó

    // Grava os campos do nó no arquivo
    fwrite(&no->removido, sizeof(char), 1, arv);
    fwrite(&no->proximo, sizeof(int), 1, arv);
    fwrite(&no->tipoNo, sizeof(int), 1, arv);
    fwrite(&no->nroChaves, sizeof(int), 1, arv);
    // Loop para gravar as chaves e os ponteiros intercalados
    for (int i = 0; i < MAX_NOS; i++) {
        fwrite(&no->Chaves[i], sizeof(int), 1, arv);
        fwrite(&no->Pr[i], sizeof(int), 1, arv);
    }
    fwrite(no->P, sizeof(int), MAX_PONTEIROS, arv);
}

// Le o conteudo da struct No do arquivo de indices para a struct No
void lerNoArvore(FILE* arv, NoArvore* no, int RRN) {
    if (arv == NULL || no == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Calcula o deslocamento para o RRN especificado
    long offset = TAM_CABECALHO + RRN * TAM_NO;
    fseek(arv, offset, SEEK_SET); // Move o cursor para a posição do nó

    // Le os campos do nó do arquivo para a struct
    fread(&no->removido, sizeof(char), 1, arv);
    fread(&no->proximo, sizeof(int), 1, arv);
    fread(&no->tipoNo, sizeof(int), 1, arv);
    fread(&no->nroChaves, sizeof(int), 1, arv);
    // Loop para ler as chaves e os ponteiros intercalados
    for (int i = 0; i < MAX_NOS; i++) {
        fread(&no->Chaves[i], sizeof(int), 1, arv);
        fread(&no->Pr[i], sizeof(int), 1, arv);
    }
    fread(no->P, sizeof(int), MAX_PONTEIROS, arv);
}

// Ve qual o RRN na criação de um novo nó
int alocaRRN(FILE* arv, CabecalhoArvore* cab){
    int novoRRN;

    // Se a pilha de lógicamente removidos não está vazia, pega o RRN sobrando que ela aponta
    // e atualiza o topo com o valor do campo "próximo" do nó que o topo apontava
    if (cab->topo != -1){
        NoArvore temp;
        novoRRN = cab->topo;
        lerNoArvore(arv, &temp, novoRRN);
        cab->topo = temp.proximo;
        return novoRRN;
    
    // Caso contrário, só pega o valor do próxRRN e atualiza ele
    } else {
        novoRRN= cab->proxRRN;
        cab->proxRRN++;
        return novoRRN;
    }
}


/*
----------------------------------------------------------------------------------
Implementação das funcionalidades básicas da Árvore B: buscar, inserir e remover
----------------------------------------------------------------------------------
*/

// --------------------------------------
// FUNÇÕES PARA A BUSCA NA ÁRVORE B

// Função auxiliar para fazer a busca recursiva na árvore B
int ArvoreBuscarRecursiva(FILE* arv, int RRN, int chave) {
    if (RRN == -1) {
        return -1;
    }

    NoArvore no;
    lerNoArvore(arv, &no, RRN);

    // Verifica se a chave está presente no nó atual
    for (int i = 0; i < no.nroChaves; i++){
        int chaveAtual = no.Chaves[i];
        if (chaveAtual == chave)        // Se estiver, retorna o índice para a página dela no arquivo
            return no.Pr[i];
        if (chaveAtual > chave && no.tipoNo != -1)      // Se achar alguma chave maior que a procurada, retorna o ponteiro para o último nó filho
            return ArvoreBuscarRecursiva(arv, no.P[i], chave);
        }

    // Verifica se o nó não é tipo folha
    // Caso não seja e tenha saído do for, quer dizer que a chave buscada é maior que qualquer chave no nó, procura assim no nó filho mais a direita
    if (no.tipoNo != -1)
        return ArvoreBuscarRecursiva(arv, no.P[no.nroChaves], chave);

    return -1;
}

// Busca um registro na Árvore-B, retornando o RRN do nó onde a chave foi encontrada ou -1 se não encontrada
int ArvoreBuscar(FILE* arv, CabecalhoArvore* cab, int chave) {
    if (arv == NULL || cab == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    if (cab->noRaiz == -1) return -1;
    int atualRRN = cab->noRaiz;

    return ArvoreBuscarRecursiva(arv, atualRRN, chave);
}

// --------------------------------------
// FUNÇÕES PARA A INSERÇÃO NA ÁRVORE B

// Função para inserir ordenado em um nó folha, uma vez que já determinado que ele não está cheio
void FolhaInserirOrdenado(NoArvore* no, int chave, int pr){

    // Procura enquanto não acha uma chave no nó maior que a chave que queremos inserir
    for (int i = 0; i < no->nroChaves; i++){
        if (no->Chaves[i] > chave) {
            for (int j = no->nroChaves-1; j >= i; j--){
                no->Chaves[j] = no->Chaves[j-1];        // Da shift nas chaves antigas pra encaixar a chave nova no lugar dela
                no->Pr[j] = no->Pr[j-1];                // Da shift nos ponteiros pra acompanhar o shift das chaves
            }
            
            no->Chaves[i] = chave;
            no->Pr[i] = pr;
            no->nroChaves++;
            return;
        }
    }

    // Se não achou nada no for, é por que a chave nova é maior que as outras no nó
    int pos = no->nroChaves;
    no->Chaves[pos] = chave;
    no->Pr[pos] = pr;
    no->nroChaves++;
    return;
}

void InternoInserirOrdenado(NoArvore* no, int chave, int pr, int direitoRRN){


    for (int i = 0; i < no->nroChaves; i++){
        if (no->Chaves[i] > chave){
            for (int j = no->nroChaves-1; j >= i; j--){
                no->Chaves[j] = no->Chaves[j-1];        // Da shift nas chaves antigas pra encaixar a chave nova no lugar dela
                no->Pr[j] = no->Pr[j-1];
                no->P[j+1] = no->P[j];                // Da shift nos ponteiros pra acompanhar o shift das chaves
            }
            
            no->Chaves[i] = chave;
            no->Pr[i] = pr;
            no->P[i+1] = direitoRRN;
            no->nroChaves++;
            return;
        }
    }

    // Se não achou nada no for, é por que a chave nova é maior que as outras no nó
    int pos = no->nroChaves;
    no->Chaves[pos] = chave;
    no->Pr[pos] = pr;
    no->nroChaves++;
    no->P[no->nroChaves] = direitoRRN;
    return;
}

ResultadoInsercao split(FILE* arv, CabecalhoArvore* cab, NoArvore no, int chaveNova, int pr, int atualRRN, int direitoRRN){

    // Cria nó temporário e inicializa ele com tamanho maior e a chave que seria adicionada a mais
    int tempChaves[MAX_NOS+1];
    int tempPr[MAX_NOS+1];
    int tempP[MAX_PONTEIROS+1];

    for (int i = 0; i < MAX_NOS; i++){
        tempChaves[i] = no.Chaves[i];
        tempPr[i] = no.Pr[i];
        tempP[i] = no.P[i];
    }
    tempP[MAX_PONTEIROS-1] = no.P[MAX_PONTEIROS-1];

    bool achouChave = false;
    for (int i = 0; i < MAX_NOS; i++){
        if (tempChaves[i] > chaveNova){
            for (int j = MAX_NOS; j >= i; j--){
                tempChaves[j] = tempChaves[j-1];        // Da shift nas chaves antigas pra encaixar a chave nova no lugar dela
                tempPr[j] = tempPr[j-1];
                tempP[j+1] = tempP[j];                // Da shift nos ponteiros pra acompanhar o shift das chaves
            }
            
            tempChaves[i] = chaveNova;
            tempPr[i] = pr;
            tempP[i+1] = direitoRRN;
            achouChave = true;
            break;
        }
    }

    // Se não achou nada no for, é por que a chave nova é maior que as outras no nó
    if (!achouChave){
        int pos = MAX_NOS;
        tempChaves[pos] = chaveNova;
        tempPr[pos] = pr;
        tempP[pos+1] = direitoRRN;
    }
    
    // Pega o nó anterior e mantém só os dois primeiros valores
    // Será o novo nó esquerdo
    for (int i = 0; i < MAX_NOS; i++){
        if (i < 2){
            no.Chaves[i] = tempChaves[i];
            no.Pr[i] = tempPr[i];
            no.P[i] = tempP[i];
        } else {
            no.Chaves[i] = -1;
            no.Pr[i] = -1;
            no.P[i+1] = -1;
        }
    }
    no.P[2] = tempP[2];
    no.nroChaves = 2;
    gravarNoArvore(arv, &no, atualRRN);     // Grava no arquivo de indíces

    // Cria o novo nó que será o direito
    // Inicializa ele com a última chave do nó temporário
    NoArvore novoNo;
    NoCriar(&novoNo, no.tipoNo);
    int novoRRN = alocaRRN(arv, cab);
    if (no.tipoNo == 1){
        novoNo.P[0] = tempP[3];   // filho esquerdo do nó direito
        novoNo.P[1] = tempP[4];   // filho direito do nó direito
        novoNo.nroChaves = 1;
    }
    novoNo.Chaves[0] = tempChaves[3];
    novoNo.Pr[0] = tempPr[3];
    gravarNoArvore(arv, &novoNo, novoRRN);  // Grava no arquivo de indíces

    // Cria struct do resultado da inserção passando as informações do split pra fora da função
    ResultadoInsercao resultado;
    resultado.chavePromovida = tempChaves[2];
    resultado.houveSplit = true;
    resultado.novoRRN = novoRRN;
    resultado.PRpromovido = tempPr[2];

    return resultado;
}

ResultadoInsercao ArvoreInserirRecursiva(FILE* arv, CabecalhoArvore* cab,int atualRRN, int chave, int pr, int noDireitoPromovido, int chavePromovida){

    ResultadoInsercao resultado;
    resultado.chavePromovida = -1;
    resultado.PRpromovido = -1;
    resultado.PRpromovido = -1;
    resultado.houveSplit = false;

    NoArvore no;
    lerNoArvore(arv, &no, atualRRN);
    
    // 1º passo: ve se o nó atual é folha, se não for, ve por qual filho desce
    // Verifica se é nó folha
    if (no.tipoNo == -1) {
        // Se não estiver cheio, só adiciona na folha
        if (no.nroChaves < MAX_NOS){
            FolhaInserirOrdenado(&no, chave, pr);
            gravarNoArvore(arv, &no, atualRRN);
            return resultado;
        
        // Se estiver cheio, precisa tratar o split
        } else {
            return split(arv, cab, no, chave, pr, atualRRN, -1);
        }
    
    // Se não for nó folha, encontra por onde ele desce
    } else {

        for (int i = 0; i < no.nroChaves; i++){
            if (no.Chaves[i] == chave) {
                printf("Chave já inserida na árvore");
                return;
            }
            if (no.Chaves[i] > chave){
                return ArvoreInserirRecursiva(arv, cab, no.Chaves[i], chave, no.Pr[i], -1, -1);
            }
        }
    }

}

void ArvoreInserir(FILE* arv, CabecalhoArvore* cab, int chave){
    return NULL;
}

