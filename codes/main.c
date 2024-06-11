#include "lib.c"

int main(){
    CPU cpu1, cpu2, cpu3, cpu4;
    MP ram;
    DMA disco1, disco2, disco3, disco4;
    ARM disco_rigido;
    int tempo = 0;
    int n_discos = 4;
    

    inicializa_hardware(&ram, &disco1, &disco2, &disco3, &disco4,
        &disco_rigido, &cpu1, &cpu2, &cpu3, &cpu4);

    FILE *arquivo = fopen("arquivo.txt", "r");
    if(!arquivo) exit(1);

    inicializa_processos(arquivo, &disco_rigido, ram);

    fclose(arquivo);

    DMA discos[] = {disco1, disco2, disco3, disco4};
    apresentacao();
    visualiza_DMA(discos, n_discos);

    printf("Preenchendo a Memória Principal... \n\n");
    usleep(1000000);
    visualiza_ARM(disco_rigido);
    visualiza_MP(ram);

    F *tmp = disco_rigido.processos;
    while(tmp){
        insere_MP(disco_rigido, &ram, &tmp->processo);
        tmp = tmp->prox;
    }
    tmp = disco_rigido.processos;
    resumo_processo(tmp);
    
    printf("\n\n     Memória Principal preenchida com sucesso! \n\n");
    // printf("\n\nDEPOIS DE PREENCHER A MP \n");
    visualiza_ARM(disco_rigido);
    visualiza_MP(ram);

    CPU x = cpu_disponivel(cpu1, cpu2, cpu3, cpu4);
    printf("\nCPU DISPONIVEL: CPU %d \n\n", x.indice);

    CPU aux1 = cpu_disponivel(cpu1, cpu2, cpu3, cpu4);
    CPU aux2 = cpu_disponivel(cpu1, cpu2, cpu3, cpu4);

    execucao(disco_rigido, &ram, disco_rigido.processos->processo, &cpu3);
    execucao(disco_rigido, &ram, disco_rigido.processos->prox->processo, &cpu1);
    
    CPU cpus[] = {cpu1, cpu2,cpu3,cpu4};
    int n_cpu = 4;
    
    visualiza_ARM(disco_rigido);
    visualiza_MP(ram);
    visualiza_CPU(cpus, n_cpu);


    libera_fila(ram.processos);
    libera_fila(ram.prontosRQ0);
    libera_fila(ram.prontosRQ1);
    libera_fila(ram.prontosRQ2);
    libera_fila(ram.bloqueados);
    libera_fila(disco_rigido.processos);
    libera_fila(disco_rigido.suspensos);
    
    return 0;
}