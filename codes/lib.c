#include "lib.h"

/* funções vão nesse arquivo */
void inicializa_hardware(MP *ram, MS *disco1, MS *disco2, MS *disco3, MS *disco4){
    // inicializa a ram
    ram->tam_total = 32000;
    ram->controle_memoria = 0;
    ram->processos = NULL;
    ram->prontos = NULL;
    ram->bloqueados = NULL;

    // inicializa os discos
    disco1->indice = 1;
    disco1->processos = NULL;

    disco2->indice = 2;
    disco2->processos = NULL;

    disco3->indice = 3;
    disco3->processos = NULL;

    disco4->indice = 4;
    disco4->processos = NULL;
}

void inicializa_processos(FILE *arquivo, MS *disco1, MS *disco2, MS *disco3, MS *disco4){
    P aux;
    int id = 1;
    while(fscanf(arquivo, "%d, %d, %d, %d, %d, %d\n",
        &aux.chegada,
        &aux.duracao_fase1,
        &aux.duracao_es,
        &aux.duracao_fase2,
        &aux.tam,
        &aux.indice_disco) == 6){
            switch (aux.indice_disco){
            case 1:
                disco1->processos = cria_processo(id, aux.chegada, aux.duracao_fase1,
                aux.duracao_es, aux.duracao_fase2, aux.tam, *disco1);
                break;
            case 2:
                disco2->processos = cria_processo(id, aux.chegada, aux.duracao_fase1,
                aux.duracao_es, aux.duracao_fase2, aux.tam, *disco2);
                break;
            case 3:
                disco3->processos = cria_processo(id, aux.chegada, aux.duracao_fase1,
                aux.duracao_es, aux.duracao_fase2, aux.tam, *disco3);
                break;
            case 4:
                disco4->processos = cria_processo(id, aux.chegada, aux.duracao_fase1,
                aux.duracao_es, aux.duracao_fase2, aux.tam, *disco4);
                break;
            default:
                printf("UNIDADE DE DISCO INEXISTENTE. \n");
                exit(1);
                break;
            }
        id++;
        }
}

/*
int ler_processos(FILE *file, P *processos) {
    
    Def.: Método responsável pela construção do lista de processos

    Parametros:
    - file: FILE - arquivo de entrada, já aberto e pronto para leitura.
    - processos: - Vetor de processos

    Retorno:
    número de processos computados
    
    int count = 0;
    while (count < MAX_PROCESSOS && fscanf(file, "%d, %d, %d, %d, %d, %d\n",
                &processos[count].tam, 
                &processos[count].chegada, 
                &processos[count].estado,
                &processos[count].indice_fila,
                &processos[count].indice_cpu,
                &processos[count].indice_disco) == 6) {
        count++;
    }
    return count;
}

void imprime_processos(P *processo, int nprocessos){
    /*
    Def.:

    Parametros:

    Retorno:

    
    printf("------------- INFORMAÇÕES GERAIS DOS PROCESSOS: ------------- ");
    for (int i = 0; i < nprocessos; i++) {
        printf("\nPROCESSO %d:\nTamanho: %d\nChegada: %d\nEstado: %d\nIndice Fila: [%d]\nIndice CPU: [%d]\nIndice Disco: [%d]\n\n",
            i,
            processo[i].tam,
            processo[i].chegada, 
            processo[i].estado, 
            processo[i].indice_fila, 
            processo[i].indice_cpu, 
            processo[i].indice_disco);
    }
}
*/


// insere os processos na memória secundária
F* cria_processo(int id_processo, int chegada, int duracao_fase1,
    int duracao_es, int duracao_fase2, int tam, MS id_disco){
    F *novo = (F*)malloc(sizeof(F));
    if(!novo) exit(1);
    // guarda as infos nas estruturas necessárias
    novo->processo.id_processo = id_processo;
    novo->processo.chegada = chegada;
    novo->processo.duracao_fase1 = duracao_fase1;
    novo->processo.duracao_es = duracao_es;
    novo->processo.duracao_fase2 = duracao_fase2;
    novo->processo.tam = tam;
    // foi carregado em memória, vai para o estado novo
    novo->processo.estado = NOVO;
    novo->processo.indice_disco = id_disco.indice;
    // Se a fila estiver nula, o novo processo se torna o primeiro da fila e retornamos ele
    if(!id_disco.processos) {
        novo->prox = id_disco.processos;
        return novo;
    }
    F *aux = id_disco.processos;
    while(aux->prox != NULL){
        aux = aux->prox;
    }
    aux->prox = novo;
    novo->prox = NULL;
    return id_disco.processos;
}

void visualiza_MS(MS disco1, MS disco2, MS disco3, MS disco4){
    F *aux1 = disco1.processos, *aux2 = disco2.processos,
    *aux3 = disco3.processos, *aux4 = disco4.processos;

    printf("ESTADO DA MEMORIA SECUNDARIA: \n\n");

    printf("DISCO 1: \n");
    while(aux1 != NULL){
        printf("PROCESSO [%d]: \n", aux1->processo.id_processo);
        printf("tempo de chegada: %d, tempo de cpu: %d, tamanho do processo: %d MB, estado do processo: %d \n",
            aux1->processo.chegada, (aux1->processo.duracao_fase1 + aux1->processo.duracao_fase2),
            aux1->processo.tam, aux1->processo.estado);
        aux1 = aux1->prox;
    }

    printf("\nDISCO 2: \n");
    while(aux2 != NULL){
        printf("PROCESSO [%d]: \n", aux2->processo.id_processo);
        printf("tempo de chegada: %d, tempo de cpu: %d, tamanho do processo: %d MB, estado do processo: %d \n",
            aux2->processo.chegada, (aux2->processo.duracao_fase1 + aux2->processo.duracao_fase2),
            aux2->processo.tam, aux2->processo.estado);
        aux2 = aux2->prox;
    }

    printf("\nDISCO 3: \n");
    while(aux3 != NULL){
        printf("PROCESSO [%d]: \n", aux3->processo.id_processo);
        printf("tempo de chegada: %d, tempo de cpu: %d, tamanho do processo: %d MB, estado do processo: %d \n",
            aux3->processo.chegada, (aux3->processo.duracao_fase1 + aux3->processo.duracao_fase2),
            aux3->processo.tam, aux3->processo.estado);
        aux3 = aux3->prox;
    }

    printf("\nDISCO 4: \n");
    while(aux4 != NULL){
        printf("PROCESSO [%d]: \n", aux4->processo.id_processo);
        printf("tempo de chegada: %d, tempo de cpu: %d, tamanho do processo: %d MB, estado do processo: %d \n",
            aux4->processo.chegada, (aux4->processo.duracao_fase1 + aux4->processo.duracao_fase2),
            aux4->processo.tam, aux4->processo.estado);
        aux4 = aux4->prox;
    }  
}

void libera_MS (MS disco1, MS disco2, MS disco3, MS disco4){
    F *aux1 = disco1.processos, *aux2 = disco2.processos,
    *aux3 = disco3.processos, *aux4 = disco4.processos, *tmp;

    while(aux1 != NULL){
        tmp = aux1;
        aux1 = aux1->prox;
        free(tmp);
    }

    while(aux2 != NULL){
        tmp = aux2;
        aux2 = aux2->prox;
        free(tmp);
    }

    while(aux3 != NULL){
        tmp = aux3;
        aux3 = aux3->prox;
        free(tmp);
    }

    while(aux4 != NULL){
        tmp = aux4;
        aux4 = aux4->prox;
        free(tmp);
    }
}

/*
ATENÇÃO A QUEM FOR USAR ESSA FUNÇÃO:
PARA SABER SE O PROCESSO REALMENTE EXISTE, USE
UMA VARIAVEL COMUM (SEM SER PONTEIRO) DO TIPO P
E CHEQUE O INDICE DO PROCESSO E DO DISCO!!!!!!
SE FOR -1, QUER DIZER QUE NAO EXISTE PROCESSO
CASO CONTRARIO O INDICE DO DISCO TE DIRÁ AONDE O PROCESSO
ESTÁ
*/
P busca_processo_MS(MS disco1, MS disco2, MS disco3, MS disco4, P processos){
    F *aux1 = disco1.processos, *aux2 = disco2.processos,
    *aux3 = disco3.processos, *aux4 = disco4.processos;

    // procura no disco 1
    while(aux1){
        if(aux1->processo.id_processo == processos.id_processo) return aux1->processo;
        aux1 = aux1->prox;
    }

    // procura no disco 2
    while(aux2){
        if(aux2->processo.id_processo == processos.id_processo) return aux2->processo;
        aux2 = aux2->prox;
    }

    // procura no disco 3
    while(aux3){
        if(aux3->processo.id_processo == processos.id_processo) return aux3->processo;
        aux3 = aux3->prox;
    }

    // procura no disco 4
    while(aux4){
        if(aux4->processo.id_processo == processos.id_processo) return aux4->processo;
        aux4 = aux4->prox;
    }

    // se não retornou ainda, então o elemento não está em MS
    P proc;
    // se o id_processo e o indice_disco forem -1, elemento não existe
    proc.id_processo = -1;
    proc.indice_disco = -1;

    return proc;
}
