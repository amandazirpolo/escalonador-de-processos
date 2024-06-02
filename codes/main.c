#include "lib.c"

int main(){
    CPU cpu1, cpu2, cpu3, cpu4;
    MP ram;
    MS disco1, disco2, disco3, disco4;
    int tempo = 0;

    inicializa_hardware(&ram, &disco1, &disco2, &disco3, &disco4);

    FILE *arquivo = fopen("arquivo.txt", "r");
    if(!arquivo) exit(1);

    inicializa_processos(arquivo, &disco1, &disco2, &disco3, &disco4);

    fclose(arquivo);

    visualiza_MS(disco1, disco2, disco3, disco4);
    
    libera_MS(disco1, disco2, disco3, disco4);
    
    return 0;
}