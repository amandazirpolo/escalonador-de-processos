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
    int controle_memoria; // para controlar o número de processos alocados na memória
    F *processos;
    F *prontos;
    F *bloqueados;
} MP;

typedef struct disco {
    int indice;
    F *processos;
} MS;

/* cabeçalho das funções vão nesse arquivo */

// funções de inicialização
void inicializa_hardware (MP *ram, MS *disco1, MS *disco2, MS *disco3, MS *disco4);
void inicializa_processos(FILE *arquivo, MS *disco1, MS *disco2, MS *disco3, MS *disco4);

// funções de busca 
P busca_processo_MS(MS id_disco, P processos);

// funções de verificação
void visualiza_MS(MS disco1, MS disco2, MS disco3, MS disco4);

// funções de criação
F* cria_processo(int id_processo, int chegada, int duracao_fase1, int duracao_es, int duracao_fase2, int tam, MS id_disco);

// funções de desalocação
void libera_MS (MS disco1, MS disco2, MS disco3, MS disco4);