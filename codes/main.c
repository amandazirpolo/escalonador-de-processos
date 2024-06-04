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

    inicializa_processos(arquivo, &disco_rigido);

    fclose(arquivo);

    visualiza_MS(disco1);
    visualiza_MS(disco2);
    visualiza_MS(disco3);
    visualiza_MS(disco4);

    visualiza_ARM(disco_rigido);
    
    libera_ARM(disco_rigido);
    
    return 0;
}