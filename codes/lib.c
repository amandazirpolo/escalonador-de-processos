#include "lib.h"
/* funções vão nesse arquivo */

// int tem_memoria(int total_ram, int tamanho_processo){
//     return total_ram - tamanho_processo;
// }


/*
Calcula quantidade de páginas por processo
---------------------------------

Observações:


### Parâmetros:

- int tamanho_processo
- int tamanho_pagina

### Retorno:

- Média de páginas do processo
    
*/
int calcula_paginas_processo(int tamanho_processo, int tamanho_pagina) {
    return (tamanho_processo + tamanho_pagina - 1) / tamanho_pagina;
}


/*
Cria uma nova tabela de páginas
---------------------------------

Observações:


### Parâmetros:

- int qtd_paginas: Quantidade de páginas desejadas na tabela.

### Retorno:

Tabela de páginas devidamente incializada
*/
T_TABELA_PAGINAS* new_tabela_paginas(int qtd_paginas) {
    T_TABELA_PAGINAS *tabela = (T_TABELA_PAGINAS *) malloc(sizeof(T_TABELA_PAGINAS));
    if (!tabela) {
        perror("Erro ao alocar memória para tabela de páginas");
        exit(-1);
    }
    tabela->array_de_paginas = (T_PAGINA *) malloc(sizeof(T_PAGINA) * qtd_paginas);
    if (!tabela->array_de_paginas) {
        free(tabela);
        perror("Erro ao alocar memória para array de páginas");
        exit(-1);
    }
    
    tabela->qtd_paginas = qtd_paginas;
    
    for (int i = 0; i < qtd_paginas; i++) {
        tabela->array_de_paginas[i].endereco_inicial = -1;
        tabela->array_de_paginas[i].presente_mp = 0;
        tabela->array_de_paginas[i].modificado = 0;
    }
    return tabela;
}


T_TABELA_PAGINAS* cria_paginas_processo(P* processo, int tamanho_pagina){
    int qtd_paginas = calcula_paginas_processo(processo->tam, tamanho_pagina);
    T_TABELA_PAGINAS* tabela = new_tabela_paginas(qtd_paginas);
    return tabela;
}


/*
Testa se há espaço na memória
---------------------------------

Observações: Se há página disponível ainda há espaço livre na memória.


### Parâmetros:

* tamanho_processo: 

### Retorno:

    
*/
int tem_pagina_disponivel(int tamanho_processo, int total_ram, int disponivel_ram, int tamanho_pagina, int paginas_disponiveis){
    
    return paginas_disponiveis > calcula_paginas_processo(tamanho_processo, tamanho_pagina);
}

void inicializa_hardware(MP *ram, DMA *disco1, DMA *disco2, DMA *disco3, DMA *disco4, ARM *disco_rigido, CPU *cpu1, CPU *cpu2, CPU *cpu3, CPU *cpu4){
    // inicializa a ram
    ram->tam_total = 32768;
    ram->tamanho_pagina = 1024; // cada pagina tem 1024 MB
    ram->numero_paginas = ram->tam_total / ram->tamanho_pagina; // ao todo são 32 paginas
    ram->paginas_disponiveis = ram->numero_paginas;
    ram->processos = NULL;
    ram->prontosRQ0 = NULL;
    ram->prontosRQ1 = NULL;
    ram->prontosRQ2 = NULL;
    ram->bloqueados = NULL;

    // inicializa os discos
    disco1->indice = 1;
    disco1->processo.id_processo = -1;

    disco2->indice = 2;
    disco2->processo.id_processo = -1;

    disco3->indice = 3;
    disco3->processo.id_processo = -1;

    disco4->indice = 4;
    disco4->processo.id_processo = -1;

    // inicializa o disco rígido
    disco_rigido->processos = NULL;
    disco_rigido->pronto_suspenso = NULL;
    disco_rigido->bloqueado_suspenso = NULL;

    // inicializa as cpus
    cpu1->indice = 1;
    cpu1->processo = (P*)malloc(sizeof(P));
    if (!cpu1->processo) {
        perror("Erro ao alocar memória para CPU 1");
        exit(1);
    }
    cpu1->processo->id_processo = -1;

    cpu2->indice = 2;
    cpu2->processo = (P*)malloc(sizeof(P));
    if (!cpu2->processo) {
        perror("Erro ao alocar memória para CPU 2");
        exit(1);
    }
    cpu2->processo->id_processo = -1;

    cpu3->indice = 3;
    cpu3->processo = (P*)malloc(sizeof(P));
    if (!cpu3->processo) {
        perror("Erro ao alocar memória para CPU 3");
        exit(1);
    }
    cpu3->processo->id_processo = -1;

    cpu4->indice = 4;
    cpu4->processo = (P*)malloc(sizeof(P));
    if (!cpu4->processo) {
        perror("Erro ao alocar memória para CPU 4");
        exit(1);
    }
    cpu4->processo->id_processo = -1;
}

void inicializa_processos(FILE *arquivo, ARM *disco_rigido, MP *ram) {
    P *aux = (P *)malloc(sizeof(P));
    if (!aux) {
        perror("Erro ao alocar memória para processo auxiliar");
        exit(1);
    }
    int id = 1;

    while (fscanf(arquivo, "%d, %d, %d, %d, %d, %d\n",
                  &aux->chegada,
                  &aux->duracao_fase1,
                  &aux->duracao_es,
                  &aux->duracao_fase2,
                  &aux->tam,
                  &aux->numero_discos) == 6) {

        P *tmp = busca_processo_ARM(disco_rigido, aux);
        
        if (tmp->id_processo == -1) {
            F *novo_processo = cria_processo(id, aux->chegada, aux->duracao_fase1, aux->duracao_es, aux->duracao_fase2, aux->tam, aux->numero_discos, disco_rigido, ram);
            
            if (ram->paginas_disponiveis < calcula_paginas_processo(aux->tam, ram->tamanho_pagina)) {
                swapperMS(disco_rigido, ram);  // Chama o swapperMS para liberar memória
            }
        }
        id++;
    }
    free(aux);
}


//void insere_bloqueados(ARM disco_rigido, MP *ram, P processo){}


/*
Insere Processo na Mem. Principal
---------------------------------

Observações:


### Parâmetros:

- ARM disco_rigido: Disco Rigido.
- MP * ram: Ponteiro de Memória Ram ( Secundária ).
- P * processo: Ponteiro de Processo a ser inserido.

### Retorno:

    
*/
void insere_MP(ARM disco_rigido, MP *ram, P *processo){
    // se essa condição for maior que zero, entao tem espaço disponivel na memoria
    if(tem_pagina_disponivel(processo->tam, ram->tam_total, ram->controle_memoria, ram->tamanho_pagina, ram->paginas_disponiveis)){ // se tem paginas tem memoria
        // atualiza o contexto do processo
        processo->estado = PRONTO;
        // atualiza o contexto da mp
        T_TABELA_PAGINAS* tabela_paginas = processo->tabela_paginas;
        for (int i = 0; i < tabela_paginas->qtd_paginas; i++) {
            tabela_paginas->array_de_paginas[i].presente_mp = 1;
        }
        
        ram->controle_memoria = ram->controle_memoria - processo->tam;
        ram->paginas_disponiveis = ram->paginas_disponiveis - processo->tabela_paginas->qtd_paginas;
        
        int tmp = atualiza_fila(processo);
        processo->indice_fila = tmp;
        switch (tmp){
            case 0:
                processo->indice_fila = 0;
                ram->prontosRQ0 = insere_na_fila(ram->prontosRQ0, processo);
                break;
            case 1:
                processo->indice_fila = 1;
                ram->prontosRQ1 = insere_na_fila(ram->prontosRQ1, processo);
                break;
            case 2:
                processo->indice_fila = 2;
                ram->prontosRQ2 = insere_na_fila(ram->prontosRQ2, processo);
                break;
            default:
                break;
        }
        ram->processos = insere_na_fila(ram->processos, processo);
        // atualiza o contexto do processo no armazenamento
        F *aux = disco_rigido.processos;
        while(aux){
            if(aux->processo->id_processo == processo->id_processo){
                aux->processo->estado = PRONTO;
                break;
            }
            aux = aux->prox;
        }
    }
    else {
        swapperMP(&disco_rigido, ram);
        insere_MP(disco_rigido, ram, processo);
    }
}

void swapperMP(ARM *disco_rigido, MP *ram) {
    // Primeiro, pega um processo na memória principal para ser suspenso
    F *aux_ram = ram->processos;
    P *aux_processo;

    if (aux_ram) {
        ram->processos = retira_da_fila(ram->processos, aux_processo);

        if (aux_processo->estado == PRONTO){
            aux_processo = aux_ram->processo;
            ram->processos = retira_da_fila(ram->prontosRQ0, aux_processo);
            
            // Depois, atualiza o contexto do processo suspenso
            aux_processo->estado = PRONTO_SUSPENSO;
            ram->controle_memoria += aux_processo->tam;
            ram->paginas_disponiveis += aux_processo->tabela_paginas->qtd_paginas;

            // Insere o processo suspenso no disco rígido
            disco_rigido->pronto_suspenso = insere_na_fila(disco_rigido->pronto_suspenso, aux_processo);

            // Atualiza o contexto do processo no armazenamento
            F *aux_processo_disco = busca_processo_fila(disco_rigido->processos, aux_processo);
            if (aux_processo_disco) aux_processo_disco->processo->estado = PRONTO_SUSPENSO;
        }
        else {
            aux_processo = aux_ram->processo;
            ram->processos = retira_da_fila(ram->bloqueados, aux_processo);
            
            // Depois, atualiza o contexto do processo suspenso
            aux_processo->estado = BLOQUEADO_SUSPENSO;
            ram->controle_memoria += aux_processo->tam;
            ram->paginas_disponiveis += aux_processo->tabela_paginas->qtd_paginas;

            // Insere o processo suspenso no disco rígido
            disco_rigido->bloqueado_suspenso = insere_na_fila(disco_rigido->bloqueado_suspenso, aux_processo);

            // Atualiza o contexto do processo no armazenamento
            F *aux_processo_disco = busca_processo_fila(disco_rigido->processos, aux_processo);
            if (aux_processo_disco) aux_processo_disco->processo->estado = BLOQUEADO_SUSPENSO;
        }
    }
}

void swapperMS(ARM *disco_rigido, MP *ram) {
    // Primeiro, verifica se há processos suspensos no disco rígido
    F *aux_disco_pronto = disco_rigido->pronto_suspenso;
    F *aux_disco_bloqueado = disco_rigido->bloqueado_suspenso;

    if (aux_disco_pronto || aux_disco_bloqueado) {
        P *aux_processo_bloqueado = aux_disco_bloqueado->processo;
        P *aux_processo_pronto = aux_disco_pronto->processo;
        
        // Verifica se há memória disponível na RAM para o processo
        if (ram->controle_memoria >= aux_processo_pronto->tam && ram->paginas_disponiveis >= aux_processo_pronto->tabela_paginas->qtd_paginas) {
            // Verifico o contexto do processo
            if (aux_processo_pronto->estado == PRONTO_SUSPENSO) {
                aux_processo_pronto->estado = PRONTO;
                // Retira o processo suspenso do disco rígido
                disco_rigido->pronto_suspenso = retira_da_fila(disco_rigido->pronto_suspenso, aux_processo_pronto);

                // Insere de volta na fila correspondente a sua fila do feedback de antes de ter sido suspenso
                if (aux_processo_pronto->indice_fila == 0)
                    ram->prontosRQ0 = insere_na_fila(ram->prontosRQ0, aux_processo_pronto);
                if (aux_processo_pronto->indice_fila == 1)
                    ram->prontosRQ1 = insere_na_fila(ram->prontosRQ1, aux_processo_pronto);
                if (aux_processo_pronto->indice_fila == 2)
                    ram->prontosRQ2 = insere_na_fila(ram->prontosRQ2, aux_processo_pronto);
            }

            // Atualiza o controle da RAM
            ram->controle_memoria -= aux_processo_pronto->tam;
            ram->paginas_disponiveis -= aux_processo_pronto->tabela_paginas->qtd_paginas;

            // Insere o processo na RAM
            ram->processos = insere_na_fila(ram->processos, aux_processo_pronto);

            // Atualiza o contexto do processo no armazenamento
            F *aux_processo_disco = busca_processo_fila(disco_rigido->processos, aux_processo_pronto);
            if (aux_processo_disco) {
                if (aux_processo_pronto->estado == PRONTO) 
                    aux_processo_disco->processo->estado = PRONTO;
            }
        }
        else if (ram->controle_memoria >= aux_processo_bloqueado->tam && ram->paginas_disponiveis >= aux_processo_bloqueado->tabela_paginas->qtd_paginas) {
            // Atualiza o contexto do processo
            if (aux_processo_bloqueado->estado == BLOQUEADO_SUSPENSO) {
                // Retira o processo suspenso do disco rígido
                disco_rigido->bloqueado_suspenso = retira_da_fila(disco_rigido->bloqueado_suspenso, aux_processo_pronto);

                aux_processo_bloqueado->estado = BLOQUEADO;
                ram->bloqueados = insere_na_fila(ram->bloqueados, aux_processo_bloqueado);
            }

            // Atualiza o controle da RAM
            ram->controle_memoria -= aux_processo_bloqueado->tam;
            ram->paginas_disponiveis -= aux_processo_bloqueado->tabela_paginas->qtd_paginas;

            // Insere o processo na RAM
            ram->processos = insere_na_fila(ram->processos, aux_processo_bloqueado);

            // Atualiza o contexto do processo no armazenamento
            F *aux_processo_disco = busca_processo_fila(disco_rigido->processos, aux_processo_bloqueado);
            if (aux_processo_disco) {
                if (aux_processo_bloqueado->estado == BLOQUEADO) 
                    aux_processo_disco->processo->estado = BLOQUEADO;
            }
        }
    }
}

void gerencia_filas_feedback(ARM *disco_rigido, MP *ram) {
    // Processa a fila prontosRQ0
    F *aux = ram->prontosRQ0;
    P *processo;
    while (aux) {
        processo = aux->processo;
        // Se o processo tiver excedido seu quantum na fila RQ0, movê-lo para RQ1
        if (processo->tempoEmFila >= QUANTUM) {
            processo->tempoEmFila = 0;
            processo->indice_fila = atualiza_fila(processo);
            ram->prontosRQ0 = retira_da_fila(ram->prontosRQ0, processo);
            ram->prontosRQ1 = insere_na_fila(ram->prontosRQ1, processo);
        }
        aux = aux->prox;
    }

    // Processa a fila prontosRQ1
    aux = ram->prontosRQ1;
    while (aux) {
        processo = aux->processo;

        // Se o processo tiver excedido seu quantum na fila RQ1, movê-lo para RQ2
        if (processo->tempoEmFila >= QUANTUM) {
            processo->tempoEmFila = 0;
            processo->indice_fila = atualiza_fila(processo);
            ram->prontosRQ1 = retira_da_fila(ram->prontosRQ1, processo);
            ram->prontosRQ2 = insere_na_fila(ram->prontosRQ2, processo);
        }
        aux = aux->prox;
    }

    // Processa a fila prontosRQ2
    aux = ram->prontosRQ2;
    while (aux) {
        processo = aux->processo;

        // Se o processo tiver excedido seu quantum na fila RQ2, mantê-lo na mesma fila porem joga ele pro final da fila
        if (processo->tempoEmFila >= QUANTUM) {
            processo->tempoEmFila = 0;
            // Retirar o processo da posição atual
            ram->prontosRQ2 = retira_da_fila(ram->prontosRQ2, processo);
            // Inserir o processo no final da fila
            ram->prontosRQ2 = insere_na_fila(ram->prontosRQ2, processo);   
        }
        aux = aux->prox;
    }
    // Após mover processos entre as filas, chama o swapperMS para gerenciar a memória
    swapperMS(disco_rigido, ram);
}


/*
Verifica em qual fila o processo deve ser inserido.
---------------------------------------------------

Observações:

### Parâmetros

- P *processo: Ponteiro para Processo a ser verificado o índice atual.

### Retorno

- 0 caso não esteja alocado a nenhuma fila e precise ser mudado para RQ0.
- 1 caso esteja alocado na fila RQ0 e precise ser mudado pra RQ1
- 2 caso esteja alocado na fila RQ1 e precise ser mudado pra RQ2
*/
int atualiza_fila(P *processo){
    if (processo->indice_fila == -1) return 0;
    if (processo->indice_fila == 0) return 1;
    return 2;
}

void visualiza_CPU(CPUS *cpus) {
    if(!cpus) return;
    CPUS *aux = cpus;
    printf("------------------------------------------------------------------------------\n");
    printf("                              ESTADO DAs CPUs                                  \n");
    printf("------------------------------------------------------------------------------\n");
    printf(" CPU |   Processo   | Tmp. Rest. Fase 1 | Tmp.  Rest. E/S | Tmp. Rest. Fase 2 \n");
    printf("-----+--------------+-------------------+-----------------+-------------------\n");
    while (aux){
        CPU *indice_cpu = aux->cpu;
        
        if (indice_cpu->processo->id_processo == -1) {
            printf(" %d   |     -1       |       -1          |       -1        |       -1        \n", indice_cpu->indice);
            printf("------------------------------------------------------------------------------\n");

        } else {
            printf(" %d   |      %d       |        %d          |        %d        |        %d        \n",
                    indice_cpu->indice,
                    indice_cpu->processo->id_processo,
                    indice_cpu->processo->controle_fase1, 
                    indice_cpu->processo->controle_es, 
                    indice_cpu->processo->controle_fase2);
            printf("------------------------------------------------------------------------------\n");
        }
        aux = aux->prox;
    }
}

F *retira_da_fila(F *fila, P *processo){
    processo->indice_fila = -1;
    F *atual = fila, *anterior = NULL;
    while((atual) && (atual->processo->id_processo != processo->id_processo)){
        anterior = atual;
        atual = atual->prox;
    }
    if(!atual) return fila;
    if(!anterior) fila = fila->prox;
    else anterior->prox = atual->prox;
    free(atual);
    return fila;
}

CPU *cpu_disponivel(CPU *cpu1, CPU *cpu2, CPU *cpu3, CPU *cpu4){
    if(cpu1->processo->id_processo == -1) return cpu1;
    if(cpu2->processo->id_processo == -1) return cpu2;
    if(cpu3->processo->id_processo == -1) return cpu3;
    if(cpu4->processo->id_processo == -1) return cpu4;
    
    // Corrigindo a alocação de memória para tmp
    CPU *tmp = (CPU *)malloc(sizeof(CPU));
    if (!tmp) {
        perror("Erro ao alocar memória para CPU temporária");
        exit(1);
    }
    
    tmp->indice = -1;
    tmp->processo = (P *)malloc(sizeof(P));
    if (!tmp->processo) {
        perror("Erro ao alocar memória para o processo da CPU temporária");
        free(tmp);
        exit(1);
    }
    tmp->processo->id_processo = -1;
    
    return tmp;
}


int fase_do_processo(P *processo){
    // retorna 1 se for a fase 1, 2 se for a fase de e/s e 3 se for a fase 2
    if((processo->controle_fase1 > 0) && (processo->controle_es > 0) 
        && (processo->controle_fase2 > 0)) return 1;
    if((processo->controle_fase1 == 0) && (processo->controle_es > 0)
        && (processo->controle_fase2 > 0)) return 2;
    if((processo->controle_fase1 == 0) && (processo->controle_es == 0) 
        && (processo->controle_fase2 > 0)) return 3;
}

void execucao(ARM *disco_rigido, MP *ram, CPUS *cpus, int *tmp){
    while (disco_rigido->processos) {
        F *aux = disco_rigido->processos;

        while (aux) {
            P *processo = aux->processo;
            printf("\n\nTEMPO %d ------------------------------------------------------\n\n", *tmp);
            // Verifica o estado atual do processo e decide a ação apropriada
            printf("\nINICIANDO EXECUÇÃO DO PROCESSO %d...\n\n", processo->id_processo);
            printf("-------------------------------------------\n");
            printf("INFORMAÇÃO        |  Processo Nº %d\n", processo->id_processo);
            printf("------------------+------------------------\n");
            printf("chegada           |  No tempo %d\n", processo->chegada);
            printf("duracao_fase1     |  %d Unidades de tempo\n", processo->duracao_fase1);
            printf("controle_fase1    |  %d Unidades de tempo\n", processo->controle_fase1);
            printf("duracao_es        |  %d Unidades de tempo\n", processo->duracao_es);
            printf("controle_es       |  %d Unidades de tempo\n", processo->controle_es);
            printf("duracao_fase2     |  %d Unidades de tempo\n", processo->duracao_fase2);
            printf("controle_fase2    |  %d Unidades de tempo\n", processo->controle_fase2);
            printf("tam               |  %d Mb\n", processo->tam);
            printf("estado            |  %d\n", processo->estado);
            printf("indice_fila       |  %d\n", processo->indice_fila);
            printf("tempoEmFila       |  %d Unidades de tempo\n", processo->tempoEmFila);
            printf("numero_discos     |  %d Discos alocados\n", processo->numero_discos);
            printf("------------------+------------------------\n");

            CPU *cpu_alocada = processo_alocado(cpus, processo);
            switch (processo->estado) {
                case NOVO:
                    insere_MP(*disco_rigido, ram, processo);
                    
                    resumo_processo(aux);
                    break;

                case PRONTO:
                    if(processo->chegada == *tmp || processo->controle_fase1 >= 0 ){
                        CPU *cpu1 = cpus->cpu;
                        CPU *cpu2 = cpus->prox->cpu;
                        CPU *cpu3 = cpus->prox->prox->cpu;
                        CPU *cpu4 = cpus->prox->prox->prox->cpu;

                        CPU *cpu_disp = cpu_disponivel(cpu1, cpu2, cpu3, cpu4);
                        if (cpu_disp->indice != -1) {
                            insere_CPU(disco_rigido, ram, processo, cpu_disp, cpus);
                        }
                    }
                    break;

                case PRONTO_SUSPENSO:
                    // Tenta mover o processo de PRONTO_SUSPENSO para PRONTO
                    swapperMS(disco_rigido, ram);
                    break;

                case BLOQUEADO:
                    // Se o processo está bloqueado, verifica se deve ser movido para BLOQUEADO_SUSPENSO
                    if (processo->controle_es > 0) {
                        processo->controle_es--;
                    } else {
                        ram->bloqueados = retira_da_fila(ram->bloqueados, processo);
                        insere_MP(*disco_rigido, ram, processo);
                    }
                    break;

                case BLOQUEADO_SUSPENSO:
                    // Tenta mover o processo de BLOQUEADO_SUSPENSO para BLOQUEADO
                    swapperMS(disco_rigido, ram);
                    break;

                case EXECUTANDO:
                    // Se o processo está executando, atualiza seu estado
                    if (processo->controle_fase1 > 0) {
                        processo->controle_fase1--;
                    } else if (processo->controle_es > 0) {
                        retira_processo_da_CPU(processo,cpu_alocada);

                        processo->controle_es--;
                        processo->estado = BLOQUEADO;
                        switch (processo->indice_fila){
                            case 0:
                                processo->indice_fila = 0;
                                ram->prontosRQ0 = retira_da_fila(ram->prontosRQ0, processo);
                                break;
                            case 1:
                                processo->indice_fila = 1;
                                ram->prontosRQ1 = retira_da_fila(ram->prontosRQ1, processo);
                                break;
                            case 2:
                                processo->indice_fila = 2;
                                ram->prontosRQ2 = retira_da_fila(ram->prontosRQ2, processo);
                                break;
                            default:
                                break;
                        }
                        processo->indice_fila = -1;
                        ram->bloqueados = insere_na_fila(ram->bloqueados, processo);
                    } else if (processo->controle_fase2 > 0) {
                        // retira_processo_da_CPU(processo,cpu_alocada);

                        processo->controle_fase2--;
                    } else {

                        ram->paginas_disponiveis += processo->tabela_paginas->qtd_paginas;
                        processo->estado = SAIDA;
                        ram->processos = retira_da_fila(ram->processos, processo);
                        processo->indice_fila = -1;
                        // ram->bloqueados = insere_na_fila(ram->bloqueados, processo);
                        disco_rigido->processos = retira_da_fila(disco_rigido->processos, processo);
                        aux = disco_rigido->processos;
                        retira_processo_da_CPU(processo,cpu_alocada);
                    }
                    break;

                case SAIDA:
                    // // Se o processo está terminado, libera seus recursos
                    // ram->processos = retira_da_fila(ram->processos, processo);
                    // ram->bloqueados = insere_na_fila(ram->bloqueados, processo);
                    // // disco_rigido->processos = retira_da_fila(disco_rigido->processos, processo);
                    break;

                default:
                    printf("Estado desconhecido do processo!\n");
                    break;
            }
            visualiza_ARM(disco_rigido);
            if(!disco_rigido->processos) break;;
            aux = aux->prox;
            visualiza_MP(ram);
        }
        printf("\n\nTEMPO %d ------------------------------------------------------\n\n", *tmp);
        visualiza_CPU(cpus);

        (*tmp)++;
        usleep(1000000);

        // Atualiza as filas de feedback
        gerencia_filas_feedback(disco_rigido, ram);
    }
}


F *busca_processo_fila(F *fila, P *processo){
    F *aux = fila;
    while((aux) && (aux->processo->id_processo != processo->id_processo)){
        aux = aux->prox;
    }
    return aux;
}

CPU *processo_alocado(CPUS *cpus, P *processo) {
    CPUS *aux = cpus;
    while (aux) {
        if (aux->cpu->processo->id_processo == processo->id_processo) {
            return aux->cpu;
        }
        aux = aux->prox;
    }
    return NULL;
}

void insere_CPU(ARM *disco_rigido, MP *ram, P *processo, CPU *indice_cpu, CPUS* cpus) {
    if (processo && indice_cpu && !processo_alocado(cpus, processo)) {
        processo->estado = EXECUTANDO;
        indice_cpu->processo = processo;
    } else {
        printf("Erro: Parâmetros inválidos fornecidos para insere_CPU\n");
    }
}

void retira_processo_da_CPU(P *processo, CPU *indice_cpu) {
    if (processo && indice_cpu) {
        indice_cpu->processo->estado = PRONTO; 
        indice_cpu->processo = (P*)malloc(sizeof(P));
        if (!indice_cpu->processo) {
            perror("Erro ao alocar memória para CPU 2");
            exit(1);
        }
        indice_cpu->processo->id_processo = -1;
    } else {
        printf("Erro: Parâmetros inválidos fornecidos para retira_processo_da_CPU\n");
    }
}

F* insere_na_fila(F *fila, P *processo){
    if(!busca_processo_fila(fila, processo)){
        F *novo = (F*)malloc(sizeof(F));
        if(!novo) exit(1);
        novo->processo = processo;
        novo->prox = fila;
        return novo;
    }
    return fila;
}

// insere os processos no disco de armazenamento
F* cria_processo(int id_processo, int chegada, int duracao_fase1,
                 int duracao_es, int duracao_fase2, int tam, int numero_discos, ARM *disco_rigido, MP *ram) {
    F *novo = (F *)malloc(sizeof(F));
    if (!novo) {
        fprintf(stderr, "Erro ao alocar memória para novo processo\n");
        exit(EXIT_FAILURE);
    }

    novo->processo = (P *)malloc(sizeof(P)); // Inicializando novo->processo
    if (!novo->processo) {
        fprintf(stderr, "Erro ao alocar memória para o processo\n");
        free(novo);
        exit(EXIT_FAILURE);
    }

    novo->processo->id_processo = id_processo;
    novo->processo->chegada = chegada;
    novo->processo->duracao_fase1 = duracao_fase1;
    novo->processo->controle_fase1 = duracao_fase1;
    novo->processo->duracao_es = duracao_es;
    novo->processo->controle_es = duracao_es;
    novo->processo->duracao_fase2 = duracao_fase2;
    novo->processo->controle_fase2 = duracao_fase2;
    novo->processo->tam = tam;
    novo->processo->numero_discos = numero_discos;
    novo->processo->estado = NOVO;
    novo->processo->indice_fila = -1;
    novo->processo->tempoEmFila = 0;

    // Inicializa a tabela de páginas
    novo->processo->tabela_paginas = cria_paginas_processo(novo->processo, ram->tamanho_pagina);
    if (!novo->processo->tabela_paginas) {
        fprintf(stderr, "Erro ao alocar memória para a tabela de páginas do processo\n");
        free(novo->processo);
        free(novo);
        exit(EXIT_FAILURE);
    }

    // Adiciona o novo processo ao disco rígido
    if (!disco_rigido->processos) {
        disco_rigido->processos = novo;
        novo->prox = NULL;
    } else {
        F *aux = disco_rigido->processos;
        while (aux->prox) {
            aux = aux->prox;
        }
        aux->prox = novo;
        novo->prox = NULL;
    }

    return disco_rigido->processos;
}



void visualiza_DMA(DMAS *discos) {
    if(!discos) return;
    DMAS *aux = discos;
    int i = 0;
    // Imprimindo o cabeçalho da tabela
    printf("-------------------------------------------------------\n");
    printf("              TABELA DE ESTADOS DOS DISCOS              \n");
    printf("---------+---------------------------------------------\n");
    printf("DISCO    |       STATUS\n");
    printf("---------+---------------------------------------------\n");

    // Imprimindo o status de cada disco
    while(aux) {
        DMA disco = *aux->disco;

        printf("Disco %d  | ", i+1);
        if (disco.processo.id_processo == -1) {
            printf("Livre\n");
        } else {
            printf("Processo %d em disco\n", disco.processo.id_processo);
        }
        i++;
        aux = aux->prox;
    }
    printf("-------------------------------------------------------\n\n");
    
}

void apresentacao(){
    printf("=======================================================\n");
    printf("               ________________________               \n");
    printf("\n");
    printf("                      SIMULADOR                       \n");
    printf("               Escalonador de Processos               \n");
    printf("               ________________________               \n");
    printf("\n");
    printf("                       GRUPO 5                        \n\n");
    printf("--------------------------+----------------------------\n");
    printf(" Amanda S. Zírpolo        | amandazirpolo@id.uff.br\n");
    printf(" Gabriel J. Panza         | gabrieljp@id.uff.br\n");
    printf(" Gustavo C.M. de Medeiros | gustavomedeiros@id.uff.br\n");
    printf(" João Vitor M. de Moraes  | joaovitormoraes@id.uff.br\n");
    printf(" João Vitor de S.         | santanajoao@id.uff.br\n");
    printf("--------------------------+----------------------------\n");
    printf("\n");
    printf("=======================================================\n\n\n");
}


void libera_fila(F *fila){
    F *aux = fila, *tmp;

    while(aux != NULL){
        tmp = aux;
        aux = aux->prox;
        free(tmp);
    }
}

void visualiza_ARM(ARM *disco_rigido) {
    F* aux = disco_rigido->processos;

    printf("-------------------------------------------------------\n");
    printf("             LISTA DE PROCESSOS EXISTENTES             \n");
    printf("------------+--------------+--------+------------------\n");
    printf("ID Processo | Tamanho (MB) | Estado | Índice da Fila\n");
    printf("------------+--------------+--------+------------------\n");

    while(aux) {
        printf("%11d | %12d | %6d | %13d\n", 
               aux->processo->id_processo, 
               aux->processo->tam, 
               aux->processo->estado, 
               aux->processo->indice_fila);
        aux = aux->prox;
    }

    printf("-------------------------------------------------------\n\n");
}

void resumo_processo(F *tmp){
     // Cabeçalho para a lista de processos
    printf("-------------------------------------------------------\n");
    printf("             ESTADO DOS PROCESSOS NO DISCO             \n");
    printf("-------------------------------------------------------\n");
    printf("ID Processo | Qtde Páginas | Tamanho (MB) | Fase\n");
    printf("------------+--------------+--------------+------\n");
    
    // Listando os processos
    while(tmp) {
    printf("     %d      |      %d       |      %d       |  %d\n", 
               tmp->processo->id_processo, 
               tmp->processo->tabela_paginas->qtd_paginas, 
               tmp->processo->tam, 
               fase_do_processo(tmp->processo));
        tmp = tmp->prox;
    }
    printf("-------------------------------------------------------\n\n");
}


void visualiza_MP(MP *ram) {
    F* aux = ram->processos;

    printf("-------------------------------------------------------\n\n");
    printf("              ESTADO DA MEMORIA PRINCIPAL              \n\n");
    printf("-------------------------------------------------------\n");

    // Lista de processos em MP
    printf("               LISTA DE PROCESSOS EM MP                \n");
    printf("-------------------------------------------------------\n");
    printf("     ID Processo   |    Qtde Páginas   | Tamanho (MB) \n");
    printf("-------------------+-------------------+---------------\n");
    if(!aux) {
        printf("        -1         |         0         |      0       \n");

    } else {
        while(aux) {
            printf("        %d          |         %d         |      %d       \n", 
                   aux->processo->id_processo, 
                   aux->processo->tabela_paginas->qtd_paginas, 
                   aux->processo->tam);
            aux = aux->prox;
        }
        printf("-------------------------------------------------------\n");
    }

    // Lista de prontos
    printf("-------------------------------------------------------\n");
    printf("\n\n                 LISTA DE PRONTOS:                 \n");
    printf("-------------------------------------------------------\n");
    printf("     ID Processo   |    Fila   \n");
    printf("-------------------+-----------------------------------\n");
    aux = ram->prontosRQ0;
    if(!aux) {
        printf("        -1         |    RQ0   \n");
    } else {
        while(aux) {
            printf("         %d         |    RQ0   \n",aux->processo->id_processo);
            aux = aux->prox;
        }
    }

    aux = ram->prontosRQ1;
    if(!aux) {
        printf("        -1         |    RQ1   \n");
    } else {
        while(aux) {
            printf("         %d         |    RQ1   \n",aux->processo->id_processo);
            aux = aux->prox;
        }
    }

    aux = ram->prontosRQ2;
    if(!aux) {
        printf("        -1         |    RQ2   \n");
    } else {
        while(aux) {
            printf("         %d         |    RQ2   \n",aux->processo->id_processo);
            aux = aux->prox;
        }
    }
    printf("-------------------------------------------------------\n");

    // Lista de bloqueados
    printf("\n\n                LISTA DE BLOQUEADOS:                \n");

    printf("-------------------------------------------------------\n");
    printf("  ID Processo |\n");
    printf("--------------+----------------------------------------\n");
    aux = ram->bloqueados;
    if(!aux) {
        printf("     -1       |\n");
    } else {
        while(aux) {
            printf("       %d     |\n", aux->processo->id_processo);
            aux = aux->prox;
        }
    }
    printf("-------------------------------------------------------\n\n");

    // Informações sobre páginas
    //printf("Numero total de paginas:        %d\n", ram->numero_paginas);
    //printf("Numero de paginas disponiveis:  %d\n\n", ram->paginas_disponiveis);
    printf("-------------------------------------------------------\n");
}

P *busca_processo_ARM(ARM *disco_rigido, P *processos){
    F *aux = disco_rigido->processos;

    // procura no disco
    while(aux){
        if(aux->processo->id_processo == processos->id_processo) return aux->processo;
        aux= aux->prox;
    }

    // se não retornou, então o elemento não existe
    P *proc = (P *)malloc(sizeof(P));  // Aloca memória para proc
    if (!proc) {
        fprintf(stderr, "Erro de alocação de memória\n");
        exit(1); 
    }
    proc->id_processo = -1;

    return proc;
}

void* endereco_real(void* endereco_virtual, void* endereco_pagina, unsigned int tamanho_pagina_bytes) {
    uintptr_t virtual = (uintptr_t) endereco_virtual;
    uintptr_t pagina = (uintptr_t) endereco_pagina;
    uintptr_t offset = virtual % tamanho_pagina_bytes;
    return (void*)(pagina + offset);
}

int* paginas_na_memoria(F* fila, int* total_paginas) {
    // Primeiro, determinar o número total de páginas
    int count = 0;
    F* temp = fila;
    while (temp != NULL) {
        count += temp->processo->tabela_paginas->qtd_paginas;
        temp = temp->prox;
    }

    // Alocar memória para o array de inteiros
    int* paginas = (int*)malloc(count * sizeof(int));
    if (paginas == NULL) {
        fprintf(stderr, "Erro ao alocar memória\n");
        return NULL;
    }

    // Inicializar o array de páginas com 0
    for (int i = 0; i < count; i++) {
        paginas[i] = 0;
    }

    // Preencher o array de páginas com o ID do processo se a página estiver na memória
    int index = 0;
    temp = fila;
    while (temp != NULL) {
        P *processo_atual = temp->processo;
        for (int i = 0; i < processo_atual->tabela_paginas->qtd_paginas; i++) {
            if (processo_atual->tabela_paginas->array_de_paginas[i].presente_mp) {
                paginas[index] = processo_atual->id_processo;
            }
            index++;
        }
        temp = temp->prox;
    }

    *total_paginas = count; // Atualiza o total de páginas
    return paginas;
}

// Inserção das listas encaedadas

// Listas encadeadas CPUS
CPUS *insere_cpus(CPUS *lista, CPU *cpu){
    CPUS *n = (CPUS*)malloc(sizeof(CPUS));
    n->cpu = cpu;
    n->prox = lista;
    return n;
}

void libera_cpus(CPUS *lista){
    CPUS *aux = lista, *tmp;
    while(aux){
        tmp = aux;
        aux = aux->prox;
        free(tmp);
    }
}

// Listas encadeadas DMAS
DMAS *insere_dma(DMAS *lista, DMA *disco){
    DMAS *n = (DMAS*)malloc(sizeof(DMAS));
    n->disco = disco;
    n->prox = lista;
    return n;
}

void libera_dma(DMAS *lista){
    DMAS *aux = lista, *tmp;
    while(aux){
        tmp = aux;
        aux = aux->prox;
        free(tmp);
    }
}

// Função que recebe um processo e a quantidade de discos que ele precisa para executar
// Checa quantos discos estão disponíveis no momento, e caso tenha o suficiente ou mais, aloca disco para o processo
// Se não houver suficientes não faz nada
int insere_processo_DMA(DMAS *dmas, P *processo) {
    int discos_necessarios = processo->numero_discos;
    int discos_disponiveis = 0;

    // Conta quantos discos estão disponíveis
    DMAS *aux = dmas;
    while (aux) {
        if (aux->disco->processo.id_processo == -1) {
            discos_disponiveis++;
        }
        aux = aux->prox;
    }

    // Se houver discos suficientes, aloca-os para o processo
    if (discos_disponiveis >= discos_necessarios) {
        aux = dmas;
        while (aux && discos_necessarios > 0) {
            if (aux->disco->processo.id_processo == -1) {
                aux->disco->processo = *processo;
                discos_necessarios--;
            }
            aux = aux->prox;
        }
        printf("Processo %d alocado em discos.\n", processo->id_processo);
        return 1;
    } else {
        printf("Discos insuficientes para o processo %d.\n", processo->id_processo);
        return 0;
    }
}

// Percorre o DMA e salva os processos que possuem 1 ou mais discos em uma lista
// Para cada processo na lista, diminui em 1 o tempo necessário na fase de entrada e saída
void executa_DMA(DMAS *dmas, MP *ram, ARM *disco_rigido) {
    // Cria uma lista para armazenar os processos que possuem discos alocados
    F *lista_processos = NULL;

    // Percorre os DMAs e adiciona os processos à lista
    DMAS *aux = dmas;
    while (aux) {
        if (aux->disco->processo.id_processo != -1) {
            lista_processos = insere_na_fila(lista_processos, &aux->disco->processo);
        }
        aux = aux->prox;
    }

    // Para cada processo na lista, diminui em 1 o tempo necessário na fase de entrada e saída
    F *aux_lista = lista_processos;
    while (aux_lista) {
        if (aux_lista->processo->controle_es > 0) {
            aux_lista->processo->controle_es--;
            printf("Processo %d: tempo de E/S decrementado para %d.\n", aux_lista->processo->id_processo, aux_lista->processo->controle_es);

            // Se o tempo de controle de E/S chegar a zero
            if (aux_lista->processo->controle_es == 0) {
                printf("A requisição de E/S do Processo %d ficou pronta.\n", aux_lista->processo->id_processo);

                // Verifica o estado do processo
                if (aux_lista->processo->estado == BLOQUEADO) {
                    aux_lista->processo->estado = PRONTO;
                    
                    // Remove o processo dos discos
                    DMAS *aux_dmas = dmas;
                    while (aux_dmas) {
                        if (aux_dmas->disco->processo.id_processo == aux_lista->processo->id_processo) {
                            aux_dmas->disco->processo.id_processo = -1;
                        }
                        aux_dmas = aux_dmas->prox;
                    }

                    // Remove o processo da fila de bloqueados
                    ram->bloqueados = retira_da_fila(ram->bloqueados, aux_lista->processo);

                    // Insere o processo na fila de prontos em RQ0
                    aux_lista->processo->indice_fila = 0;
                     ram->prontosRQ0 = insere_na_fila(ram->prontosRQ0, aux_lista->processo);
                 } else if (aux_lista->processo->estado == BLOQUEADO_SUSPENSO) {
                     aux_lista->processo->estado = PRONTO_SUSPENSO;

//                     // Remove o processo dos discos
                     DMAS *aux_dmas = dmas;
                     while (aux_dmas) {
                         if (aux_dmas->disco->processo.id_processo == aux_lista->processo->id_processo) {
                             aux_dmas->disco->processo.id_processo = -1;
                         }
                         aux_dmas = aux_dmas->prox;
                     }

                     // Remove o processo da fila de bloqueados suspensos
                     disco_rigido->bloqueado_suspenso = retira_da_fila(disco_rigido->bloqueado_suspenso, aux_lista->processo);

                     // Insere o processo no final da fila de prontos suspensos
                     aux_lista->processo->indice_fila = -1;
                     disco_rigido->pronto_suspenso = insere_na_fila(disco_rigido->pronto_suspenso, aux_lista->processo);
                 }
             }
         }
         aux_lista = aux_lista->prox;
     }
    
     // Libera a lista de processos
     libera_fila(lista_processos);
}