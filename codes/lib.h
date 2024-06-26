#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // Comentar em caso de testes

#define QUANTUM 3
#define NOVO 0
#define PRONTO 1
#define PRONTO_SUSPENSO 2
#define EXECUTANDO 3
#define BLOQUEADO 4
#define BLOQUEADO_SUSPENSO 5
#define SAIDA 6

/*
Struct Tipo Página
------------------

### Atributos

- int endereco_inicial
- int presente_mp
- int modificado
*/
typedef struct pagina {
    int endereco_inicial;
    int presente_mp;
    int modificado;
} T_PAGINA;

/*
Struct Tipo Tabela de Páginas
------------------

### Atributos

- T_PAGINA* array_de_paginas
- int qtd_paginas
*/
typedef struct tabela_paginas {
    T_PAGINA* array_de_paginas; // array de paginas
    int qtd_paginas;
} T_TABELA_PAGINAS;


/* estruturas vão nesse arquivo */


/*
Struct Tipo Processos
------------------

### Atributos
#### Básicos

- int id_processo
- int tam
- int estado 
- int indice_fila
- T_TABELA_PAGINAS* tabela_paginas

#### Controle de Tempo

- int chegada
- int duracao_fase1
- int controle_fase1
- int duracao_es
- int controle_es
- int duracao_fase2
- int controle_fase2
- int tempoEmFila
- int numero_discos
*/
typedef struct processo {
    // basico
    int id_processo;
    int tam;
    int estado; // definidos pelas variáveis globais
    int indice_fila; // define qual fila do feedback ele se encontra
    T_TABELA_PAGINAS* tabela_paginas;
    // int qtd_paginas; // deprecado, agora dentro de tabela_paginas

    // contabilidade de tempo
    int chegada;
    int duracao_fase1;
    int controle_fase1;
    int duracao_es;
    int controle_es;
    int duracao_fase2;
    int controle_fase2;
    int tempoEmFila;
    int numero_discos;
} P;

/* estruturas vão nesse arquivo */


/*
Struct Tipo CPU
------------------

### Atributos

- P processo;
- int indice;
*/
typedef struct cpu {
    P *processo;
    int indice;
} CPU;

typedef struct cpus {
    CPU *cpu;
    struct cpus *prox;
} CPUS;

typedef struct fila {
    P *processo;
    struct fila *prox;
} F;

typedef struct ram {
    int tam_total;
    int controle_memoria;
    int tamanho_pagina; 
    int numero_paginas; 
    int paginas_disponiveis; 
    F *processos;
    F *prontosRQ0;
    F *prontosRQ1;
    F *prontosRQ2;
    F *bloqueados;
} MP;

typedef struct disco {
    int indice;
    P processo; 
} DMA;

typedef struct dmas {
    DMA *disco;
    struct dmas *prox;
} DMAS;

// essa struct nova vai servir como um hd, já que os discos usaremos para e/s


typedef struct armazenamento {
    F *processos;
    F *pronto_suspenso;
    F *bloqueado_suspenso;
} ARM;

/* cabeçalho das funções vão nesse arquivo */

// funções de inicialização
void inicializa_hardware (MP *ram, DMA *disco1, DMA *disco2, DMA *disco3,
    DMA *disco4, ARM *disco_rigido, CPU *cpu1, CPU *cpu2, CPU *cpu3, CPU *cpu4);
void inicializa_processos(FILE *arquivo, ARM *disco_rigido, MP *ram);

// funções de busca 
P *busca_processo_ARM(ARM *disco_rigido, P *processos);
F *busca_processo_fila(F *fila, P *processo);

// funções auxiliares
int fase_do_processo(P *processo);
int verifica_fila(P *processo);
CPU *cpu_disponivel(CPU *cpu1, CPU *cpu2, CPU *cpu3, CPU *cpu4);

// funções de verificação
void visualiza_DMA(DMAS *discos);
void visualiza_ARM (ARM *disco_rigido);
void visualiza_MP (MP *ram);
void visualiza_CPU(CPUS *cpus);
void apresentacao();
void resumo_processo(F *tmp);

// funções de criação
F* cria_processo(int id_processo, int chegada, int duracao_fase1,
                int duracao_es, int duracao_fase2, int tam, int numero_discos, ARM *disco_rigido, MP *ram);
F *insere_na_fila(F *fila, P *processo);
void insere_MP(ARM disco_rigido, MP *ram, P *processo);
void swapperMP(ARM *disco_rigido, MP *ram);
void swapperMS(ARM *disco_rigido, MP *ram);
void gerencia_filas_feedback(ARM *disco_rigido, MP *ram);
void execucao(ARM *disco_rigido, MP *ram, CPUS *cpus, int *tmp);
CPU *processo_alocado(CPUS *cpus, P *processo);
void retira_processo_da_CPU(P *processo, CPU *indice_cpu);
void insere_CPU(P *processo, CPU *indice_cpu, CPUS *cpus);
F *retira_da_fila(F *fila, P *processo);
int insere_processo_DMA(DMAS *dmas, P *processo);
void executa_DMA(DMAS *dmas, MP *ram, ARM *disco_rigido);

// funções de desalocação
void libera_fila(F *fila);

// codificação e decodificação de endereços virtuais
void* endereco_real(void* endereco_virtual, void* endereco_pagina, unsigned int tamanho_pagina_bytes);

//Funções lista encadeada CPUs

CPUS *insere_cpus(CPUS *lista, CPU *cpu);
void libera_cpus(CPUS *lista);

DMAS *insere_dma(DMAS *lista, DMA *disco);
void libera_dma(DMAS *lista);