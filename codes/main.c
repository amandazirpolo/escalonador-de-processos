#include "lib.c"

int main(){
    CPU cpu1, cpu2, cpu3, cpu4;
    MP ram;
    MS disco1, disco2, disco3, disco4;
    int count = 0;
    // Inicializamos nossas variaveis com seus respectivos valores iniciais
    inicializa(&ram, &disco1, &disco2, &disco3, &disco4);

    // Abre-se o arquivo para leitura e obtencao dos dados. Depois fecha-se o mesmo.
    // <Momento Chegada>, <duração de CPU fase 1>, <Duração de  I/O>, <duração de CPU fase 2>, <#Mbytes>, <# disco>
    FILE *arquivo = fopen("arquivo.txt", "r");
    if(!arquivo) exit(1);

    P processos[MAX_PROCESSOS];
    
    int num_processos = ler_processos(arquivo, processos);
    if( num_processos == -1) return 1;

    imprime_processos(processos, num_processos);    
    
    fclose(arquivo);
    
    return 0;
}