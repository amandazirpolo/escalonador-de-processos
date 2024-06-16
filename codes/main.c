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

    cpus = insere_cpus(cpus,&cpu1);
    cpus = insere_cpus(cpus,&cpu2);
    cpus = insere_cpus(cpus,&cpu3);
    cpus = insere_cpus(cpus,&cpu4);

    dmas = insere_dma(dmas, &disco1);
    dmas = insere_dma(dmas, &disco2);
    dmas = insere_dma(dmas, &disco3);
    dmas = insere_dma(dmas, &disco4);

    FILE *arquivo = fopen("arquivo.txt", "r");
    if(!arquivo) exit(1);

    inicializa_processos(arquivo, &disco_rigido, ram);

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
        insere_MP(disco_rigido, &ram, &tmp->processo);
        tmp = tmp->prox;
    }
    tmp = disco_rigido.processos;
    usleep(1000000);
    resumo_processo(tmp);
    
    printf("\n\n     Memória Principal preenchida com sucesso! \n\n");
    // printf("\n\nDEPOIS DE PREENCHER A MP \n");
    usleep(1000000);
    visualiza_ARM(disco_rigido);
    visualiza_MP(ram);

   
    execucao(disco_rigido, &ram, disco_rigido.processos->processo, &cpu3);
    execucao(disco_rigido, &ram, disco_rigido.processos->prox->processo, &cpu1);
    
    usleep(1000000);
    visualiza_ARM(disco_rigido);
    usleep(1000000);
    visualiza_MP(ram);
    usleep(1000000);
    visualiza_CPU(cpus);


    libera_fila(ram.processos);
    libera_fila(ram.prontosRQ0);
    libera_fila(ram.prontosRQ1);
    libera_fila(ram.prontosRQ2);
    libera_fila(ram.bloqueados);
    libera_fila(disco_rigido.processos);
    libera_fila(disco_rigido.suspensos);
    libera_cpus(cpus);
    libera_dma(dmas);
    
    return 0;
}