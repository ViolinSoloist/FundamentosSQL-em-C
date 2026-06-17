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
            for (int j = no->nroChaves; j > i; j--){
                no->Chaves[j] = no->Chaves[j-1];        // Da shift nas chaves antigas pra encaixar a chave nova no lugar dela
                no->Pr[j] = no->Pr[j-1];                
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

// Função para inserir ordenado em um nó interno, uma vez já determinado que ele não está cheio
void InternoInserirOrdenado(NoArvore* no, int chave, int pr, int direitoRRN){

    for (int i = 0; i < no->nroChaves; i++){
        if (no->Chaves[i] > chave){
            for (int j = no->nroChaves; j > i; j--){
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

ResultadoInsercao split(FILE* arv, CabecalhoArvore* cab, NoArvore* no, int chaveNova, int prNovo, int atualRRN, int direitoRRN){

    // Cria nó temporário e inicializa ele com tamanho maior e a chave que seria adicionada a mais
    int tempChaves[MAX_NOS+1];
    int tempPr[MAX_NOS+1];
    int tempP[MAX_PONTEIROS+1];

    for (int i = 0; i < MAX_NOS; i++){
        tempChaves[i] = no->Chaves[i];
        tempPr[i] = no->Pr[i];
        tempP[i] = no->P[i];
    }
    tempP[MAX_PONTEIROS-1] = no->P[MAX_PONTEIROS-1];

    bool achouChave = false;
    for (int i = 0; i < MAX_NOS; i++){
        if (tempChaves[i] > chaveNova){
            for (int j = MAX_NOS; j > i; j--){
                tempChaves[j] = tempChaves[j-1];        // Da shift nas chaves antigas pra encaixar a chave nova no lugar dela
                tempPr[j] = tempPr[j-1];
                tempP[j+1] = tempP[j];                // Da shift nos ponteiros pra acompanhar o shift das chaves
            }
            
            tempChaves[i] = chaveNova;
            tempPr[i] = prNovo;
            tempP[i+1] = direitoRRN;
            achouChave = true;
            break;
        }
    }

    // Se não achou nada no for, é por que a chave nova é maior que as outras no nó
    if (!achouChave){
        int pos = MAX_NOS;
        tempChaves[pos] = chaveNova;
        tempPr[pos] = prNovo;
        tempP[pos+1] = direitoRRN;
    }
    
    // Pega o nó anterior e mantém só os dois primeiros valores
    // Será o novo nó esquerdo
    for (int i = 0; i < MAX_NOS; i++){
        if (i < 2){
            no->Chaves[i] = tempChaves[i];
            no->Pr[i] = tempPr[i];
            no->P[i] = tempP[i];
        } else {
            no->Chaves[i] = -1;
            no->Pr[i] = -1;
            no->P[i+1] = -1;
        }
    }
    no->P[2] = tempP[2];
    no->nroChaves = 2;
    gravarNoArvore(arv, no, atualRRN);     // Grava no arquivo de indíces

    // Cria o novo nó que será o direito
    // Inicializa ele com a última chave do nó temporário
    NoArvore novoNo;
    NoCriar(&novoNo, no->tipoNo);
    cab->nroNos++;
    int novoRRN = alocaRRN(arv, cab);
    if (no->tipoNo != -1){
        novoNo.P[0] = tempP[3];   // filho esquerdo do nó direito
        novoNo.P[1] = tempP[4];   // filho direito do nó direito
    }
    novoNo.Chaves[0] = tempChaves[3];
    novoNo.Pr[0] = tempPr[3];
    novoNo.nroChaves = 1;
    gravarNoArvore(arv, &novoNo, novoRRN);  // Grava no arquivo de indíces
    gravarCabecalhoArvore(arv, cab);        // Atualiza o cabeçalho

    // Cria struct do resultado da inserção passando as informações do split pra fora da função
    ResultadoInsercao resultado;
    resultado.chavePromovida = tempChaves[2];
    resultado.houveSplit = true;
    resultado.novoRRN = novoRRN;
    resultado.PRpromovido = tempPr[2];

    return resultado;
}

ResultadoInsercao ArvoreInserirRecursiva(FILE* arv, CabecalhoArvore* cab, int atualRRN, int chave, int pr){

    ResultadoInsercao resultado;
    resultado.chavePromovida = -1;
    resultado.PRpromovido = -1;
    resultado.novoRRN = -1;
    resultado.houveSplit = false;

    if (atualRRN == -1) {
        return resultado;
    }

    NoArvore no;
    lerNoArvore(arv, &no, atualRRN);
    
    // 1º passo: ve se o nó atual é folha, se não for, ve por qual filho desce
    // Verifica se é nó folha
    if (no.tipoNo == -1) {
        // Se não estiver cheio, só adiciona na folha
        // printf("  folha RRN=%d nroChaves=%d\n", atualRRN, no.nroChaves);
        if (no.nroChaves < MAX_NOS){
            FolhaInserirOrdenado(&no, chave, pr);
            gravarNoArvore(arv, &no, atualRRN);
            return resultado;
        
        // Se estiver cheio, precisa tratar o split
        } else {
            return split(arv, cab, &no, chave, pr, atualRRN, -1);
        }
    
    // Se não for nó folha, encontra por onde ele desce
    } else {
        int filhoRRN = -1;
        bool desceu = false;

        for (int i = 0; i < no.nroChaves; i++){
            if (no.Chaves[i] == chave) {
                //printf("Chave já inserida na árvore");
                return resultado;
            }
            if (no.Chaves[i] > chave){
                filhoRRN = no.P[i];
                desceu = true;
                break;
            }
        }

        if (!desceu)
            filhoRRN = no.P[no.nroChaves];

        ResultadoInsercao resultadoFilho = ArvoreInserirRecursiva(arv, cab, filhoRRN, chave, pr);

        if (resultadoFilho.houveSplit){
            if (no.nroChaves < MAX_NOS){
                InternoInserirOrdenado(&no, resultadoFilho.chavePromovida, resultadoFilho.PRpromovido, resultadoFilho.novoRRN);
                gravarNoArvore(arv, &no, atualRRN);
                resultado.houveSplit = false;
                return resultado;
            } else {
                return split(arv, cab, &no, resultadoFilho.chavePromovida, resultadoFilho.PRpromovido, atualRRN, resultadoFilho.novoRRN);
            }
        }

        return resultado;
    }

}

void ArvoreInserir(FILE* arv, CabecalhoArvore* cab, int chave, int pr) {
    if (arv == NULL || cab == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // 1º caso: Árvore vazia
    if (cab->noRaiz == -1) {
        NoArvore novaRaiz;
        NoCriar(&novaRaiz, -1); // Cria o nó como sendo folha e raíz ao mesmo tempo
        
        // Inicializa o nó raiz
        novaRaiz.Chaves[0] = chave;
        novaRaiz.Pr[0] = pr;
        novaRaiz.nroChaves = 1;

        int novoRRN = alocaRRN(arv, cab);
        gravarNoArvore(arv, &novaRaiz, novoRRN);

        cab->noRaiz = novoRRN;
        cab->nroNos++;
        gravarCabecalhoArvore(arv, cab);
        return;
    }

    // 2º caso: A árvore já tem nós nela, então faz a inserção recursiva
    ResultadoInsercao res = ArvoreInserirRecursiva(arv, cab, cab->noRaiz, chave, pr);
    //printf("houveSplit=%d chavePromovida=%d novoRRN=%d\n", res.houveSplit, res.chavePromovida, res.novoRRN);
    
    // 3º casp: O split voltou propagando até a raiz
    // Precisa criar raiz nova
    if (res.houveSplit) {
        NoArvore novaRaiz;
        NoCriar(&novaRaiz, 0); // Cria como nó interno
        
        novaRaiz.Chaves[0] = res.chavePromovida;
        novaRaiz.Pr[0] = res.PRpromovido;
        novaRaiz.P[0] = cab->noRaiz; // O filho esquerdo é a raiz antiga
        novaRaiz.P[1] = res.novoRRN; // O filho direito é o nó recém criado no split
        novaRaiz.nroChaves = 1;

        NoArvore filhoEsquerdo;
        NoArvore filhoDireito;
        int esquerdoRRN = novaRaiz.P[0];
        int direitoRRN = novaRaiz.P[1];
        lerNoArvore(arv, &filhoEsquerdo, esquerdoRRN);
        lerNoArvore(arv, &filhoDireito, direitoRRN);
        filhoEsquerdo.tipoNo = (filhoEsquerdo.P[0] == -1) ? -1 : 1;
        filhoDireito.tipoNo = (filhoDireito.P[0] == -1) ? -1 : 1;
        gravarNoArvore(arv, &filhoEsquerdo, esquerdoRRN);
        gravarNoArvore(arv, &filhoDireito, direitoRRN);

        int novoRRN = alocaRRN(arv, cab);
        gravarNoArvore(arv, &novaRaiz, novoRRN);

        cab->noRaiz = novoRRN;
        cab->nroNos++;
        gravarCabecalhoArvore(arv, cab);
    }
}
// --------------------------------------------------
// ------------------ REMOÇÃO -----------------------
// --------------------------------------------------

/// @author Erik

/// @def struct que contem os parâmetros (estruturas, variáveis, etc) usadas na remoção na arvore
typedef struct {
    FILE* arv;
    CabecalhoArvore* cab;
    NoArvore* noAtual;
    NoArvore* noPai;
    NoArvore* noDestruido;
    int* rrnAtual;
    int* rrnPai;
    int* rrnDestruido;
    int* indiceFilho;
    int* chave;
} ParamRemocao;

// função para empilhar um nó destruído na pilha de registros logicamente removidos da árvore
static void empilharNoRemovido(FILE* arv, CabecalhoArvore* cab, int rrnDestruido, NoArvore* noDestruido) {
    noDestruido->removido = '1';
    noDestruido->proximo = cab->topo; // próximo -> antigo topo
    
    // grava apenas as alterações da lista encadeada, mantendo o lixo restante intacto
    gravarNoArvore(arv, noDestruido, rrnDestruido);
    
    // atualização do cabeçalho da árvore
    cab->topo = rrnDestruido;
    cab->nroNos--;
    gravarCabecalhoArvore(arv, cab);
}

static bool redistribuicaoDireita(ParamRemocao* args) {

    int rrnIrmaoDir = args->noPai->P[*args->indiceFilho + 1];
    NoArvore irmaoDir;
    lerNoArvore(args->arv, &irmaoDir, rrnIrmaoDir);
    
    if (irmaoDir.nroChaves <= 1) return false; // continua apenas se tiver chaves o suficiente

    // total de chaves a distribuir = 1 (pai) + as chaves do irmão
    int totalChaves = 1 + irmaoDir.nroChaves;
    int combC[5], combPr[5], combP[6];
    
    // puxa chave do pai
    combC[0] = args->noPai->Chaves[*args->indiceFilho];
    combPr[0] = args->noPai->Pr[*args->indiceFilho];
    combP[0] = args->noAtual->P[0];
    
    // puxa tudo do irmão da direita
    for (int i = 0; i < irmaoDir.nroChaves; i++) {
        combC[i + 1] = irmaoDir.Chaves[i];
        combPr[i + 1] = irmaoDir.Pr[i];
        combP[i + 1] = irmaoDir.P[i];
    }
    combP[irmaoDir.nroChaves + 1] = irmaoDir.P[irmaoDir.nroChaves];
    
    // pega tudo que pegou no passo antes e distribui METADE para o nó atual
    int numEsq = totalChaves / 2;
    args->noAtual->nroChaves = numEsq;
    for (int i=0; i<numEsq; i++) {
        args->noAtual->Chaves[i] = combC[i];
        args->noAtual->Pr[i] = combPr[i];
        args->noAtual->P[i] = combP[i];
    }
    args->noAtual->P[numEsq] = combP[numEsq];
    
    // limpa as posições restantes no nó atual
    for(int i=numEsq; i<MAX_NOS; i++) { args->noAtual->Chaves[i] = -1; args->noAtual->Pr[i] = -1; }
    for(int i=numEsq+1; i<MAX_PONTEIROS; i++) { args->noAtual->P[i] = -1; }
    
    // A chave do meio ascende e fica ao lado do Pai (na verdade vira o pai)
    args->noPai->Chaves[*args->indiceFilho] = combC[numEsq];
    args->noPai->Pr[*args->indiceFilho] = combPr[numEsq];
    
    // resto fica no irmão da direita
    int numDir = totalChaves - numEsq - 1;
    irmaoDir.nroChaves = numDir;
    for (int i=0; i<numDir; i++) {
        irmaoDir.Chaves[i] = combC[numEsq + 1 + i];
        irmaoDir.Pr[i] = combPr[numEsq + 1 + i];
        irmaoDir.P[i] = combP[numEsq + 1 + i];
    }
    irmaoDir.P[numDir] = combP[totalChaves];
    
    // Limpa as posições restantes no irmão direito
    for(int i=numDir; i<MAX_NOS; i++) { irmaoDir.Chaves[i] = -1; irmaoDir.Pr[i] = -1; }
    for(int i=numDir+1; i<MAX_PONTEIROS; i++) { irmaoDir.P[i] = -1; }
    
    gravarNoArvore(args->arv, args->noAtual, *args->rrnAtual);
    gravarNoArvore(args->arv, &irmaoDir, rrnIrmaoDir);
    gravarNoArvore(args->arv, args->noPai, *args->rrnPai);
    return true;
}

static bool redistribuicaoEsquerda(ParamRemocao* args) {

    // BASICAMENTE A MESMA COISA DE ANTES MAS EM VEZ DE PEGAR DA DIREITA E JOGAR NO ATUAL, PEGA DA ESQUERDA E FAZ OS BAGUI, SEGUE A MESMA LÓGICA
    int rrnIrmaoEsq = args->noPai->P[*(args->indiceFilho) - 1];
    NoArvore irmaoEsq;
    lerNoArvore(args->arv, &irmaoEsq, rrnIrmaoEsq);
    
    if (irmaoEsq.nroChaves <= 1) return false;
    
    // se chegou aqui, é porque tem como fazer a redistribuicao à esquerda
    int totalChaves = irmaoEsq.nroChaves + 1;
    int combC[5], combPr[5], combP[6];
    
    // pega todas as chaves e ponteiros do irmão da esquerda
    for (int i=0; i<irmaoEsq.nroChaves; i++) {
        combC[i] = irmaoEsq.Chaves[i];
        combPr[i] = irmaoEsq.Pr[i];
        combP[i] = irmaoEsq.P[i];
    }
    combP[irmaoEsq.nroChaves] = irmaoEsq.P[irmaoEsq.nroChaves];
    
    // puxa a chave do pai para o final do agrupamento
    combC[irmaoEsq.nroChaves] = args->noPai->Chaves[*args->indiceFilho - 1];
    combPr[irmaoEsq.nroChaves] = args->noPai->Pr[*args->indiceFilho - 1];
    combP[irmaoEsq.nroChaves + 1] = args->noAtual->P[0];
    
    // distribui a primeira metade para o irmão da esquerda
    int numEsq = totalChaves / 2;
    irmaoEsq.nroChaves = numEsq;
    for (int i=0; i<numEsq; i++) {
        irmaoEsq.Chaves[i] = combC[i];
        irmaoEsq.Pr[i] = combPr[i];
        irmaoEsq.P[i] = combP[i];
    }
    irmaoEsq.P[numEsq] = combP[numEsq];
    
    for(int i=numEsq; i<MAX_NOS; i++) { irmaoEsq.Chaves[i] = -1; irmaoEsq.Pr[i] = -1; }
    for(int i=numEsq + 1; i<MAX_PONTEIROS; i++) { irmaoEsq.P[i] = -1; }
    
    // chave do meio sobe pra virar pai
    args->noPai->Chaves[*args->indiceFilho - 1] = combC[numEsq];
    args->noPai->Pr[*args->indiceFilho - 1] = combPr[numEsq];
    
    // restante fica no atual (que seria da direita)
    int numDir = totalChaves - numEsq - 1;
    args->noAtual->nroChaves = numDir;
    for (int i = 0; i < numDir; i++) {
        args->noAtual->Chaves[i] = combC[numEsq + 1 + i];
        args->noAtual->Pr[i] = combPr[numEsq + 1 + i];
        args->noAtual->P[i] = combP[numEsq + 1 + i];
    }
    args->noAtual->P[numDir] = combP[totalChaves];
    
    for(int i=numDir; i<MAX_NOS; i++) { args->noAtual->Chaves[i] = -1; args->noAtual->Pr[i] = -1; }
    for(int i=numDir+1; i<MAX_PONTEIROS; i++) { args->noAtual->P[i] = -1; }
    
    gravarNoArvore(args->arv, args->noAtual, *args->rrnAtual);
    gravarNoArvore(args->arv, &irmaoEsq, rrnIrmaoEsq);
    gravarNoArvore(args->arv, args->noPai, *args->rrnPai);
    return true;
}

static bool concatenacaoEsquerda(ParamRemocao* args) {

    int rrnIrmaoEsq = args->noPai->P[*args->indiceFilho-1];
    NoArvore irmaoEsq;
    lerNoArvore(args->arv, &irmaoEsq, rrnIrmaoEsq);
    
    irmaoEsq.Chaves[irmaoEsq.nroChaves] = args->noPai->Chaves[*args->indiceFilho-1];
    irmaoEsq.Pr[irmaoEsq.nroChaves] = args->noPai->Pr[*args->indiceFilho-1];
    irmaoEsq.P[irmaoEsq.nroChaves+1] = args->noAtual->P[0];
    irmaoEsq.nroChaves++;
    
    for (int i=0; i<args->noAtual->nroChaves; i++) {
        irmaoEsq.Chaves[irmaoEsq.nroChaves] = args->noAtual->Chaves[i];
        irmaoEsq.Pr[irmaoEsq.nroChaves] = args->noAtual->Pr[i];
        irmaoEsq.P[irmaoEsq.nroChaves+1] = args->noAtual->P[i+1];
        irmaoEsq.nroChaves++;
    }
    
    for (int i=*args->indiceFilho-1; i<args->noPai->nroChaves - 1; i++) {
        args->noPai->Chaves[i] = args->noPai->Chaves[i+1];
        args->noPai->Pr[i] = args->noPai->Pr[i+1];
        args->noPai->P[i+1] = args->noPai->P[i+2];
    }

    args->noPai->Chaves[args->noPai->nroChaves - 1] = -1;
    args->noPai->Pr[args->noPai->nroChaves - 1] = -1;
    args->noPai->P[args->noPai->nroChaves] = -1;
    args->noPai->nroChaves--;
    
    gravarNoArvore(args->arv, &irmaoEsq, rrnIrmaoEsq);
    empilharNoRemovido(args->arv, args->cab, *args->rrnAtual, args->noAtual); 
    gravarNoArvore(args->arv, args->noPai, *args->rrnPai);
    return true;
}

static bool concatenacaoDireita(ParamRemocao* args) {
    int rrnIrmaoDir = args->noPai->P[*args->indiceFilho + 1];
    NoArvore irmaoDir;
    lerNoArvore(args->arv, &irmaoDir, rrnIrmaoDir);
    
    args->noAtual->Chaves[args->noAtual->nroChaves] = args->noPai->Chaves[*args->indiceFilho];
    args->noAtual->Pr[args->noAtual->nroChaves] = args->noPai->Pr[*args->indiceFilho];
    args->noAtual->P[args->noAtual->nroChaves + 1] = irmaoDir.P[0];
    args->noAtual->nroChaves++;
    
    for (int i = 0; i < irmaoDir.nroChaves; i++) {
        args->noAtual->Chaves[args->noAtual->nroChaves] = irmaoDir.Chaves[i];
        args->noAtual->Pr[args->noAtual->nroChaves] = irmaoDir.Pr[i];
        args->noAtual->P[args->noAtual->nroChaves + 1] = irmaoDir.P[i + 1];
        args->noAtual->nroChaves++;
    }
    
    for (int i = *args->indiceFilho; i < args->noPai->nroChaves - 1; i++) {
        args->noPai->Chaves[i] = args->noPai->Chaves[i + 1];
        args->noPai->Pr[i] = args->noPai->Pr[i + 1];
        args->noPai->P[i + 1] = args->noPai->P[i + 2];
    }
    args->noPai->Chaves[args->noPai->nroChaves - 1] = -1;
    args->noPai->Pr[args->noPai->nroChaves - 1] = -1;
    args->noPai->P[args->noPai->nroChaves] = -1;
    args->noPai->nroChaves--;
    
    gravarNoArvore(args->arv, args->noAtual, *args->rrnAtual);
    empilharNoRemovido(args->arv, args->cab, rrnIrmaoDir, &irmaoDir); 
    gravarNoArvore(args->arv, args->noPai, *args->rrnPai);
    return true;
}

/// @brief ROTINA DOS 4 CASOS PARA TRATAR UNDERFLOW
static void tratarUnderflow(ParamRemocao* args) {
    
    // 1) - TENTA REDISTRIBUIÇÃO A DIREITA
    if (*(args->indiceFilho) < args->noPai->nroChaves) {
        if (redistribuicaoDireita(args)) return;
    }
    
    // 2) - TENTA REDISTRIBUIÇÃO A ESQUERDA 
    if (*args->indiceFilho > 0) {
        if (redistribuicaoEsquerda(args)) return;
    }
    
    // 3) - TENTA CONCATENAÇÃO ESQUERDA
    if (*args->indiceFilho > 0) {
        if (concatenacaoEsquerda(args)) return;
    }
    
    // 4) - TENTA CONCATENAÇÃO DIREITA
    if (*args->indiceFilho < args->noPai->nroChaves) {
        if (concatenacaoDireita(args)) return;
    }
}

static bool remocaoDiretaSimples(FILE* arv, NoArvore* no, int i, int rrnAtual) {

    for (int j=i; j < no->nroChaves-1; j++) {
        no->Chaves[j] = no->Chaves[j+1];
        no->Pr[j] = no->Pr[j+1];
    }
    no->Chaves[no->nroChaves-1] = -1;
    no->Pr[no->nroChaves-1] = -1;
    no->nroChaves--;
    gravarNoArvore(arv, no, rrnAtual);
    return true;
}

bool ArvoreRemoverRecursiva(FILE* arv, CabecalhoArvore* cab, int rrnAtual, int chave); // manter ordem de principalidade

static bool substituiSucessorImediato(FILE* arv, CabecalhoArvore* cab, NoArvore* no, int i, int rrnAtual) {
    // Percorre o caminho pra achar o sucessor
    NoArvore noSucessor;
    int rrnSucessor = no->P[i+1];
    while (rrnSucessor != -1) {
        lerNoArvore(arv, &noSucessor, rrnSucessor);
        if (noSucessor.P[0] == -1) break; // achou a folha
        rrnSucessor = noSucessor.P[0];
    }
    int chaveSucessora = noSucessor.Chaves[0];
    int prSucessor = noSucessor.Pr[0];
    
    // Substitui a chave no nó atual e SALVA NO DISCO
    no->Chaves[i] = chaveSucessora;
    no->Pr[i] = prSucessor;
    gravarNoArvore(arv, no, rrnAtual); 
    
    // Remove o sucessor fisicamente lá na folha da subárvore direita
    int rrnFilhoDir = no->P[i + 1];
    bool removeu = ArvoreRemoverRecursiva(arv, cab, rrnFilhoDir, chaveSucessora);
    
    // Checa underflow no filho direito após deletar o sucessor
    if (!removeu) return true;

    NoArvore noFilhoDir;
    lerNoArvore(arv, &noFilhoDir, rrnFilhoDir);
    if (!noFilhoDir.nroChaves) {
        
        ParamRemocao args;
        args.arv = arv; args.cab = cab;
        args.noAtual = &noFilhoDir; args.rrnAtual = &rrnFilhoDir;
        args.noPai = no; args.rrnPai = &rrnAtual;
        int posFilho = i + 1;
        args.indiceFilho = &posFilho; 
        
        tratarUnderflow(&args); 
    }
    return true;
}

/// @brief função recursiva interna de remoção
/// @details Recebe os parâmetros por valor para isolar o escopo de cada nível da recursão!
bool ArvoreRemoverRecursiva(FILE* arv, CabecalhoArvore* cab, int rrnAtual, int chave) {
    
    if (rrnAtual == -1) return false;

    NoArvore no;
    lerNoArvore(arv, &no, rrnAtual);
    
    int i = 0;
    while (i<no.nroChaves && no.Chaves[i] < chave) i++;
    
    // 1) - Encontrou a chave neste nó
    // É nó folha? => remoção direta simples, se não (se for nó interno, subsituti pelo sucessor imeddiato)
    if (i<no.nroChaves && no.Chaves[i] == chave)
        return (no.tipoNo == -1 ? remocaoDiretaSimples(arv, &no, i, rrnAtual) : substituiSucessorImediato(arv, cab, &no, i, rrnAtual));
    
    // 2) Se não encontrou => continua descendo recursivamente
    int rrnFilho = no.P[i];
    bool removeu = ArvoreRemoverRecursiva(arv, cab, rrnFilho, chave);
    
    // Checa se o filho que acabou de voltar da recursão ficou com underflow
    if (removeu) {
        NoArvore noFilho;
        lerNoArvore(arv, &noFilho, rrnFilho);
        
        if (noFilho.nroChaves != 0) return removeu;

        // monta a struct de parâmetros só para o tratamento de underflow
        ParamRemocao args;
        args.arv = arv; args.cab = cab;
        args.noAtual = &noFilho; args.rrnAtual = &rrnFilho;
        args.noPai = &no; args.rrnPai = &rrnAtual;
        args.indiceFilho = &i;
        
        tratarUnderflow(&args);
    }
    
    return removeu;
}

static void trocaRaiz(FILE* arv, CabecalhoArvore* cab, NoArvore* raiz) {
    int antigaRaizRRN = cab->noRaiz;

    if (raiz->tipoNo != -1) { 
        // Se a antiga raiz era nó interno, o filho esquerdo vira a nova raiz
        cab->noRaiz = raiz->P[0];
        NoArvore novaRaiz;
        lerNoArvore(arv, &novaRaiz, cab->noRaiz);
        if (novaRaiz.tipoNo != -1) novaRaiz.tipoNo = 0; // status de raiz (0)
        gravarNoArvore(arv, &novaRaiz, cab->noRaiz);
    } else { 
        // Se era uma folha, então árvore ficou completamente vazia
        cab->noRaiz = -1;
    }
    
    // Finalmente, faz kaboom com a raíz antiga
    empilharNoRemovido(arv, cab, antigaRaizRRN, raiz);
}

/// @brief interface pública 
void ArvoreRemover(FILE* arv, CabecalhoArvore* cab, int chave) {

    if (arv == NULL || cab == NULL || cab->noRaiz == -1) return;
    
    // Inicia a recursão com os valores primários
    ArvoreRemoverRecursiva(arv, cab, cab->noRaiz, chave);
    
    // Trata queda de raíz
    if (cab->noRaiz != -1) {
        NoArvore raiz;
        lerNoArvore(arv, &raiz, cab->noRaiz);
        
        // Se raiz ficou vazia, troca raiz
        if (raiz.nroChaves == 0) trocaRaiz(arv, cab, &raiz);
    }
}