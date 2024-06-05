#include <stdlib.h>
#include <stdio.h>

#define QUANTUM 3
#define NOVO 0
#define PRONTO 1
#define PRONTO_SUSPENSO 2
#define EXECUTANDO 3
#define BLOQUEADO 4
#define BLOQUEADO_SUSPENSO 5
#define SAIDA 6

/* estruturas vão nesse arquivo */
typedef struct processo {
    int id_processo;
    int chegada;
    int duracao_fase1;
    int controle_fase1;
    int duracao_es;
    int controle_es;
    int duracao_fase2;
    int controle_fase2;
    int tam;
    int estado; // definidos pelas variáveis globais
    //int indice_fila;
    //int indice_cpu;
    //int indice_disco; 
    int numero_discos;
    int qtd_paginas;
} P;

typedef struct cpu {
    P processo;
    int indice;
} CPU;

typedef struct fila {
    P processo;
    struct fila *prox;
} F;

typedef struct ram {
    int tam_total;
    int controle_memoria;
    int tamanho_pagina;
    int numero_paginas; // para controlar o número de processos alocados na memória
    int paginas_disponiveis;
    F *processos;
    F *prontos;
    F *bloqueados;
} MP;

typedef struct disco {
    int indice;
    P processo; // vai funcionar igual a cpu, apenas 1 processo alocado por vez
} DMA;

// essa struct nova vai servir como um hd, já que os discos usaremos para e/s
typedef struct armazenamento {
    F *processos;
    F *suspensos;
} ARM;

/* cabeçalho das funções vão nesse arquivo */

// funções de inicialização
void inicializa_hardware (MP *ram, DMA *disco1, DMA *disco2, DMA *disco3,
    DMA *disco4, ARM *disco_rigido, CPU *cpu1, CPU *cpu2, CPU *cpu3, CPU *cpu4);
void inicializa_processos(FILE *arquivo, ARM *disco_rigido, MP ram);

// funções de busca 
P busca_processo_ARM(ARM disco_rigido, P processos);
F *busca_processo_fila(F *fila, P processo);

// funções auxiliares
int fase_do_processo(P processo);
CPU cpu_disponivel(CPU cpu1, CPU cpu2, CPU cpu3, CPU cpu4);

// funções de verificação
void visualiza_DMA(DMA id_disco);
void visualiza_ARM (ARM disco_rigido);
void visualiza_MP (MP ram);
void visualiza_CPU (CPU indice_cpu);

// funções de criação
F* cria_processo(int id_processo, int chegada, int duracao_fase1,
    int duracao_es, int duracao_fase2, int tam, int numero_discos, ARM disco_rigido, MP ram);
F *insere_na_fila(F *fila, P processo);
void insere_MP(ARM disco_rigido, MP *ram, P processo);
void execucao(ARM disco_rigido, MP *ram, P processo, CPU *indice_cpu);
void insere_CPU(ARM disco_rigido, MP *ram, P processo, CPU *indice_cpu);
F *retira_da_fila(F *fila, P processo);

// funções de desalocação
void libera_fila(F *fila);