#include "lib.h"

/* funções vão nesse arquivo */
void inicializa_hardware(MP *ram, MS *disco1, MS *disco2, MS *disco3, MS *disco4, ARM *disco_rigido){
    // inicializa a ram
    ram->tam_total = 32000;
    ram->controle_memoria = 0;
    ram->processos = NULL;
    ram->prontos = NULL;
    ram->bloqueados = NULL;

    // inicializa os discos
    disco1->indice = 1;
    disco1->processo.id_processo = -1;

    disco2->indice = 2;
    disco2->processo.id_processo = -1;

    disco3->indice = 3;
    disco3->processo.id_processo = -1;

    disco4->indice = 4;
    disco4->processo.id_processo = -1;

    // inicializa o disco rígido
    disco_rigido->processos = NULL;

}

void inicializa_processos(FILE *arquivo, ARM *disco_rigido){
    P aux, tmp;
    int id = 1;
    while(fscanf(arquivo, "%d, %d, %d, %d, %d, %d\n",
        &aux.chegada,
        &aux.duracao_fase1,
        &aux.duracao_es,
        &aux.duracao_fase2,
        &aux.tam,
        &aux.numero_discos) == 6){
            tmp = busca_processo_ARM(*disco_rigido, aux);
            if(tmp.id_processo == -1){
                disco_rigido->processos = cria_processo(id, aux.chegada, aux.duracao_fase1,
                    aux.duracao_es, aux.duracao_fase2, aux.tam, aux.numero_discos, *disco_rigido);
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


// insere os processos no disco de armazenamento
F* cria_processo(int id_processo, int chegada, int duracao_fase1,
    int duracao_es, int duracao_fase2, int tam, int numero_discos, ARM disco_rigido){
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
    novo->processo.numero_discos = numero_discos;
    // Se a fila estiver nula, o novo processo se torna o primeiro da fila e retornamos ele
    if(!disco_rigido.processos) {
        novo->prox = disco_rigido.processos;
        return novo;
    }
    F *aux = disco_rigido.processos;
    while(aux->prox != NULL){
        aux = aux->prox;
    }
    aux->prox = novo;
    novo->prox = NULL;
    return disco_rigido.processos;
}

void visualiza_MS(MS id_disco){
    printf("DISCO %d: \n", id_disco.indice);
    if(id_disco.processo.id_processo == -1){
        printf("DISCO LIVRE \n");
    } else {
        printf("Processo %d em disco. \n", id_disco.processo.id_processo);
    }
}

void libera_ARM (ARM disco_rigido){
    F *aux = disco_rigido.processos, *tmp;

    while(aux != NULL){
        tmp = aux;
        aux = aux->prox;
        free(tmp);
    }
}

void visualiza_ARM (ARM disco_rigido){
    F *aux = disco_rigido.processos;

    printf("Lista de processos existentes: \n");
    while(aux){
        printf("id_processo: %d, tam_processo: %d MB, estado_processo: %d \n", 
        aux->processo.id_processo, aux->processo.tam, aux->processo.estado);
        aux = aux->prox;
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
P busca_processo_ARM(ARM disco_rigido, P processos){
    F *aux = disco_rigido.processos;

    // procura no disco
    while(aux){
        if(aux->processo.id_processo == processos.id_processo) return aux->processo;
        aux= aux->prox;
    }

    // se não retornou, então o elemento não existe
    P proc;
    // se o id_processo for -1, elemento não existe
    proc.id_processo = -1;

    return proc;
}

