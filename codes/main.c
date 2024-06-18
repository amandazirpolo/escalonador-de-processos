#include "lib.c"

int main(){
    CPU cpu1, cpu2, cpu3, cpu4;
    MP ram;
    DMA disco1, disco2, disco3, disco4;
    ARM disco_rigido;
    int tempo = 0;
    int n_discos = 4;
    
    CPUS *cpus = NULL;
    DMAS *dmas = NULL;

    inicializa_hardware(&ram, &disco1, &disco2, &disco3, &disco4,
        &disco_rigido, &cpu1, &cpu2, &cpu3, &cpu4);
    printf("hardware inicializado \n");
    //adicionar cpus a lista encadeada
    cpus = insere_cpus(cpus,&cpu1);
    cpus = insere_cpus(cpus,&cpu2);
    cpus = insere_cpus(cpus,&cpu3);
    cpus = insere_cpus(cpus,&cpu4);
    printf("cpus inseridas \n");

    //adicionar discos a lista encadeada
    dmas = insere_dma(dmas, &disco1);
    dmas = insere_dma(dmas, &disco2);
    dmas = insere_dma(dmas, &disco3);
    dmas = insere_dma(dmas, &disco4);
    printf("dmas inseridos \n");

    FILE *arquivo = fopen("arquivo.txt", "r");
    if(!arquivo) exit(1);
    printf("Inicializando processos...\n");
    inicializa_processos(arquivo, &disco_rigido, ram);
    printf("processos inicializados!\n");
    fclose(arquivo);

    apresentacao();
    visualiza_DMA(dmas);

    printf("Preenchendo a Memória Principal... \n\n");
    visualiza_CPU(cpus);

    usleep(2000000);
    visualiza_ARM(disco_rigido);
    visualiza_MP(ram);

    F *tmp = disco_rigido.processos;
    while(tmp){
        // insere_MP(disco_rigido, &ram, &tmp->processo);
        tmp->processo.estado = NOVO;
        tmp = tmp->prox;
    }
    tmp = disco_rigido.processos;
    usleep(1000000);
    resumo_processo(tmp);
    
    printf("\n\n     Memória Principal preenchida com sucesso! \n\n");
    // printf("\n\nDEPOIS DE PREENCHER A MP \n");
    execucao(&disco_rigido, &ram, cpus, &tempo);
    usleep(3000000);
    visualiza_ARM(disco_rigido);
    visualiza_MP(ram);


    
    usleep(1000000);
    visualiza_ARM(disco_rigido);
    usleep(1000000);
    visualiza_MP(ram);
    usleep(1000000);
    visualiza_CPU(cpus);
    int total_paginas;
    int* resultado = paginas_na_memoria(ram.processos, &total_paginas);
    

    libera_fila(ram.processos);
    libera_fila(ram.prontosRQ0);
    libera_fila(ram.prontosRQ1);
    libera_fila(ram.prontosRQ2);
    libera_fila(ram.bloqueados);
    libera_fila(disco_rigido.processos);
    libera_fila(disco_rigido.pronto_suspenso);
    libera_fila(disco_rigido.bloqueado_suspenso);
    libera_cpus(cpus);
    libera_dma(dmas);
    
    return 0;
}