#include "lib.h"

int main(){
    CPU cpu1, cpu2, cpu3, cpu4;
    MP ram;
    MS disco1, disco2, disco3, disco4;
    
    // Inicializamos nossas variaveis com seus respectivos valores iniciais
    inicializa(&ram, &disco1, &disco2, &disco3, &disco4);

    // Abre-se o arquivo para leitura e obtencao dos dados. Depois fecha-se o mesmo.
    FILE *arquivo = fopen("arquivo.txt", "r");
    if(!arquivo) exit(1);

    P tmp;
    while(fprintf(arquivo, "%d, %d, %d, %d, %d, %d", tmp) == 6){
        //fprintf(arquivo, "%d, %d, %d, %d, %d, %d", tmp);
        operacao(tmp);
    }
    
    fclose(arquivo);
    
    return 0;
}