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
    printf("cu \n");
}