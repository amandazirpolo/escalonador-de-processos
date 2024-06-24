#include "lib.c"
#include "plot_memoria.c"
int main(){
    CPU cpu1, cpu2, cpu3, cpu4;
    MP ram;
    DMA disco1, disco2, disco3, disco4;
    ARM disco_rigido;
    int tempo = 0;
    // int n_discos = 4;

    CPUS *cpus = NULL;
    DMAS *dmas = NULL;

    inicializa_hardware(&ram, &disco1, &disco2, &disco3, &disco4,
        &disco_rigido, &cpu1, &cpu2, &cpu3, &cpu4);
    cpus = insere_cpus(cpus,&cpu1);
    cpus = insere_cpus(cpus,&cpu2);
    cpus = insere_cpus(cpus,&cpu3);
    cpus = insere_cpus(cpus,&cpu4);
    dmas = insere_dma(dmas, &disco1);
    dmas = insere_dma(dmas, &disco2);
    dmas = insere_dma(dmas, &disco3);
    dmas = insere_dma(dmas, &disco4);

    FILE *arquivo = fopen("arquivo.txt", "r");
    if(!arquivo) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }
    inicializa_processos(arquivo, &disco_rigido, &ram);
    fclose(arquivo);

    apresentacao();
    visualiza_DMA(dmas);
    usleep(1000000);

    printf("Preenchendo a Memória Principal... \n\n");

    visualiza_MP(&ram);
    visualiza_ARM(&disco_rigido);
    visualiza_CPU(cpus);

    printf("\n\n     Memória Principal preenchida com sucesso! \n\n");
    visualiza_ARM(&disco_rigido);
    usleep(2500000);

    execucao(&disco_rigido, &ram, cpus, &tempo);
    //int total_paginas;
    //int* resultado = paginas_na_memoria(ram.processos, &total_paginas);


    printf("\n\n------------------------------------------------------------------------------\n\n");
    printf("\n\n                TODOS OS PROCESSOS FORMA EXECUTADOS COM SUCESSO!                 \n");
    printf("\n\n------------------------------------------------------------------------------\n\n");

    visualiza_MP(&ram);
    // visualiza_ARM(&disco_rigido);
    visualiza_CPU(cpus);
    int* resultado = paginas_na_memoria(ram.processos, ram.numero_paginas, ram.tamanho_pagina);
	plot_memory(resultado, ram.numero_paginas);

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