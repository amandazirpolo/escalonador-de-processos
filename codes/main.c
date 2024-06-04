#include "lib.c"

int main(){
    CPU cpu1, cpu2, cpu3, cpu4;
    MP ram;
    MS disco1, disco2, disco3, disco4;
    ARM disco_rigido;
    int tempo = 0;

    inicializa_hardware(&ram, &disco1, &disco2, &disco3, &disco4, &disco_rigido);

    FILE *arquivo = fopen("arquivo.txt", "r");
    if(!arquivo) exit(1);

    inicializa_processos(arquivo, &disco_rigido, ram);

    fclose(arquivo);

    visualiza_MS(disco1);
    visualiza_MS(disco2);
    visualiza_MS(disco3);
    visualiza_MS(disco4);

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
    
    libera_fila(disco_rigido.processos);
    
    return 0;
}