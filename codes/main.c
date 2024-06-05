#include "lib.c"

int main(){
    CPU cpu1, cpu2, cpu3, cpu4;
    MP ram;
    DMA disco1, disco2, disco3, disco4;
    ARM disco_rigido;
    int tempo = 0;

    inicializa_hardware(&ram, &disco1, &disco2, &disco3, &disco4,
        &disco_rigido, &cpu1, &cpu2, &cpu3, &cpu4);

    FILE *arquivo = fopen("arquivo.txt", "r");
    if(!arquivo) exit(1);

    inicializa_processos(arquivo, &disco_rigido, ram);

    fclose(arquivo);

    visualiza_DMA(disco1);
    visualiza_DMA(disco2);
    visualiza_DMA(disco3);
    visualiza_DMA(disco4);

    printf("ANTES DE PREENCHER A MP \n");
    visualiza_ARM(disco_rigido);
    visualiza_MP(ram);

    F *tmp = disco_rigido.processos;
    while(tmp){
        insere_MP(disco_rigido, &ram, tmp->processo);
        tmp = tmp->prox;
    }
    tmp = disco_rigido.processos;
    while(tmp){
        printf("processo %d: ", tmp->processo.id_processo);
        printf("numero de paginas que ele precisa: %d - ", tmp->processo.qtd_paginas);
        printf("tam do processo: %d \n\n", tmp->processo.tam);
        tmp = tmp->prox;
    }
    
    printf("\n\nDEPOIS DE PREENCHER A MP \n");
    visualiza_ARM(disco_rigido);
    visualiza_MP(ram);

    //execucao(disco_rigido, &ram, disco_rigido.processos->processo, &cpu1, &cpu2, &cpu3, &cpu4);
    //execucao(disco_rigido, &ram, disco_rigido.processos->prox->processo, &cpu1, &cpu2, &cpu3, &cpu1);

    visualiza_CPU(cpu1);
    visualiza_CPU(cpu2);
    visualiza_CPU(cpu3);
    visualiza_CPU(cpu4);
    
    libera_fila(ram.processos);
    libera_fila(ram.prontos);
    libera_fila(ram.bloqueados);
    libera_fila(disco_rigido.processos);
    libera_fila(disco_rigido.suspensos);
    
    return 0;
}