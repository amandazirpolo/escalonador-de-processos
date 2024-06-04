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
    int duracao_es;
    int duracao_fase2;
    int tam;
    int estado; // definidos pelas variáveis globais
    int indice_fila;
    int indice_cpu;
    int indice_disco; 
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
} MS;

// essa struct nova vai servir como um hd, já que os discos usaremos para e/s
typedef struct armazenamento {
    F *processos;
} ARM;

/* cabeçalho das funções vão nesse arquivo */

// funções de inicialização
void inicializa_hardware (MP *ram, MS *disco1, MS *disco2, MS *disco3, MS *disco4, ARM *disco_rigido);
void inicializa_processos(FILE *arquivo, ARM *disco_rigido, MP ram);

// funções de busca 
P busca_processo_ARM(ARM disco_rigido, P processos);

// funções de verificação
void visualiza_MS(MS id_disco);
void visualiza_ARM (ARM disco_rigido);
void visualiza_MP (MP ram);

// funções de criação
F* cria_processo(int id_processo, int chegada, int duracao_fase1,
    int duracao_es, int duracao_fase2, int tam, int numero_discos, ARM disco_rigido, MP ram);
F *insere_na_fila(F *fila, P processo);
void insere_MP(ARM disco_rigido, MP *ram, P processo);

// funções de desalocação
void libera_fila(F *fila);