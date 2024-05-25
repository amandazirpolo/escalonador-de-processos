#include "lib.h"

/* funções vão nesse arquivo */
void inicializa (MP *ram, MS *disco1, MS *disco2, MS *disco3, MS *disco4){
    // inicializa a ram
    ram->tam_total = 32000;
    ram->controle_memoria = 0;
    ram->processos = NULL;
    ram->prontos = NULL;
    ram->bloqueados = NULL;

    // inicializa os discos
    disco1->indice = 1;
    disco1->processos = NULL;

    disco2->indice = 2;
    disco2->processos = NULL;

    disco3->indice = 3;
    disco3->processos = NULL;

    disco4->indice = 4;
    disco4->processos = NULL;
}

void operacao (P processo){
    return;
}

int ler_processos(FILE *file, P *processos) {
    /*
    Def.: Método responsável pela construção do lista de processos

    Parametros:
    - file: FILE - arquivo de entrada, já aberto e pronto para leitura.
    - processos: - Vetor de processos

    Retorno:
    número de processos computados
    */
    int count = 0;
    while (count < MAX_PROCESSOS && fscanf(file, "%d, %d, %d, %d, %d, %d\n",
                &processos[count].tam, 
                &processos[count].chegada, 
                &processos[count].estado,
                &processos[count].indice_fila,
                &processos[count].indice_cpu,
                &processos[count].indice_disco) == 6) {
        count++;
    }
    return count;
}

void imprime_processos(P *processo, int nprocessos){
    /*
    Def.:

    Parametros:

    Retorno:

    */
    printf("------------- INFORMAÇÕES GERAIS DOS PROCESSOS: ------------- ");
    for (int i = 0; i < nprocessos; i++) {
        printf("\nPROCESSO %d:\nTamanho: %d\nChegada: %d\nEstado: %d\nIndice Fila: [%d]\nIndice CPU: [%d]\nIndice Disco: [%d]\n\n",
            i,
            processo[i].tam,
            processo[i].chegada, 
            processo[i].estado, 
            processo[i].indice_fila, 
            processo[i].indice_cpu, 
            processo[i].indice_disco);
    }
}