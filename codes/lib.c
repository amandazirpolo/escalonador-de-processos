#include "lib.h"
/* funções vão nesse arquivo */

int tem_memoria(int total_ram, int tamanho_processo){
    return total_ram - tamanho_processo;
}

int calcula_paginas_processo(int tamanho_processo, int tamanho_pagina) {
    return (tamanho_processo + tamanho_pagina - 1) / tamanho_pagina;
}

T_TABELA_PAGINAS* new_tabela_paginas(int qtd_paginas) {
    T_TABELA_PAGINAS *tabela = (T_TABELA_PAGINAS *) malloc(sizeof(T_TABELA_PAGINAS));
    if (!tabela) {
        exit(-1);
    }
    tabela->array_de_paginas = (T_PAGINA *) malloc(sizeof(T_PAGINA) * qtd_paginas);
    if (!tabela->array_de_paginas) {
        free(tabela);
        exit(-1);
    }
    
    tabela->qtd_paginas = qtd_paginas;
    
    for (int i = 0; i < qtd_paginas; i++) {
        tabela->array_de_paginas[i].i_quadro = -1;
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
    disco_rigido->suspensos = NULL;

    // inicializa as cpus
    cpu1->indice = 1;
    cpu1->processo.id_processo = -1;

    cpu2->indice = 2;
    cpu2->processo.id_processo = -1;

    cpu3->indice = 3;
    cpu3->processo.id_processo = -1;

    cpu4->indice = 4;
    cpu4->processo.id_processo = -1;
}

void inicializa_processos(FILE *arquivo, ARM *disco_rigido, MP ram){
    P aux, tmp;
    int id = 1;
    printf("entrando while\n");
    while(fscanf(arquivo, "%d, %d, %d, %d, %d, %d\n",
        &aux.chegada,
        &aux.duracao_fase1,
        &aux.duracao_es,
        &aux.duracao_fase2,
        &aux.tam,
        &aux.numero_discos) == 6){
            printf("buscando processo\n");
            tmp = busca_processo_ARM(*disco_rigido, aux);
            if(tmp.id_processo == -1){
                printf("criando processo\n");
                disco_rigido->processos = cria_processo(id, aux.chegada, aux.duracao_fase1,
                    aux.duracao_es, aux.duracao_fase2, aux.tam, aux.numero_discos, *disco_rigido, ram);
                    printf("pre calcula_paginas_processo\n");
                if (ram.paginas_disponiveis < calcula_paginas_processo(aux.tam, ram.tamanho_pagina)) {
                    printf("swapper\n");
                    swapperMS(disco_rigido, &ram);  // Chama o swapperMS para liberar memória
                }
            }
            id++;
        }
}



//void insere_bloqueados(ARM disco_rigido, MP *ram, P processo){}
void insere_MP(ARM disco_rigido, MP *ram, P *processo){
    // se essa condição for maior que zero, entao tem espaço disponivel na memoria
    if(tem_pagina_disponivel(processo->tam, ram->tam_total, ram->controle_memoria, ram->tamanho_pagina, ram->paginas_disponiveis)){ // se tem paginas tem memoria
        // atualiza o contexto do processo
        processo->estado = PRONTO;
        // atualiza o contexto da mp
        ram->controle_memoria = ram->controle_memoria - processo->tam;
        ram->paginas_disponiveis = ram->paginas_disponiveis - processo->tabela_paginas->qtd_paginas;
        int tmp = verifica_fila(*processo);
        processo->indice_fila = tmp;
        switch (tmp){
            case 0:
                ram->prontosRQ0 = insere_na_fila(ram->prontosRQ0, *processo);
                break;
            case 1:
                ram->prontosRQ1 = insere_na_fila(ram->prontosRQ1, *processo);
                break;
            case 2:
                ram->prontosRQ2 = insere_na_fila(ram->prontosRQ2, *processo);
                break;
            default:
                break;
        }
        ram->processos = insere_na_fila(ram->processos, *processo);
        // atualiza o contexto do processo no armazenamento
        F *aux = disco_rigido.processos;
        while(aux){
            if(aux->processo.id_processo == processo->id_processo){
                aux->processo.estado = PRONTO;
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
    P aux_processo;

    if (aux_ram) {
        ram->processos = retira_da_fila(ram->processos, aux_processo);

        if (aux_processo.estado == PRONTO){
            aux_processo = aux_ram->processo;
            ram->processos = retira_da_fila(ram->prontosRQ0, aux_processo);
            
            // Depois, atualiza o contexto do processo suspenso
            aux_processo.estado = PRONTO_SUSPENSO;
            ram->controle_memoria += aux_processo.tam;
            ram->paginas_disponiveis += aux_processo.tabela_paginas->qtd_paginas;

            // Insere o processo suspenso no disco rígido
            disco_rigido->suspensos = insere_na_fila(disco_rigido->suspensos, aux_processo);

            // Atualiza o contexto do processo no armazenamento
            F *aux_processo_disco = busca_processo_fila(disco_rigido->processos, aux_processo);
            if (aux_processo_disco) aux_processo_disco->processo.estado = PRONTO_SUSPENSO;
        }
        else {
            aux_processo = aux_ram->processo;
            ram->processos = retira_da_fila(ram->bloqueados, aux_processo);
            
            // Depois, atualiza o contexto do processo suspenso
            aux_processo.estado = BLOQUEADO_SUSPENSO;
            ram->controle_memoria += aux_processo.tam;
            ram->paginas_disponiveis += aux_processo.tabela_paginas->qtd_paginas;

            // Insere o processo suspenso no disco rígido
            disco_rigido->suspensos = insere_na_fila(disco_rigido->suspensos, aux_processo);

            // Atualiza o contexto do processo no armazenamento
            F *aux_processo_disco = busca_processo_fila(disco_rigido->processos, aux_processo);
            if (aux_processo_disco) aux_processo_disco->processo.estado = BLOQUEADO_SUSPENSO;
        }
    }
}

void swapperMS(ARM *disco_rigido, MP *ram) {
    // Primeiro, verifica se há processos suspensos no disco rígido
    F *aux_disco = disco_rigido->suspensos;
    P aux_processo;

    if (aux_disco) {
        aux_processo = aux_disco->processo;

        // Verifica se há memória disponível na RAM para o processo
        if (ram->controle_memoria >= aux_processo.tam && ram->paginas_disponiveis >= aux_processo.tabela_paginas->qtd_paginas) {
            // Retira o processo suspenso do disco rígido
            disco_rigido->suspensos = retira_da_fila(disco_rigido->suspensos, aux_processo);

            // Atualiza o contexto do processo
            if (aux_processo.estado == PRONTO_SUSPENSO) {
                aux_processo.estado = PRONTO;
                // Insere de volta na fila correspondente a sua fila do feedback de antes de ter sido suspenso
                if (aux_processo.indice_fila == 0)
                    ram->prontosRQ0 = insere_na_fila(ram->prontosRQ0, aux_processo);
                if (aux_processo.indice_fila == 1)
                    ram->prontosRQ1 = insere_na_fila(ram->prontosRQ1, aux_processo);
                if (aux_processo.indice_fila == 2)
                    ram->prontosRQ2 = insere_na_fila(ram->prontosRQ2, aux_processo);
            } else if (aux_processo.estado == BLOQUEADO_SUSPENSO) {
                aux_processo.estado = BLOQUEADO;
                ram->bloqueados = insere_na_fila(ram->bloqueados, aux_processo);
            }

            // Atualiza o controle da RAM
            ram->controle_memoria -= aux_processo.tam;
            ram->paginas_disponiveis -= aux_processo.tabela_paginas->qtd_paginas;

            // Insere o processo na RAM
            ram->processos = insere_na_fila(ram->processos, aux_processo);

            // Atualiza o contexto do processo no armazenamento
            F *aux_processo_disco = busca_processo_fila(disco_rigido->processos, aux_processo);
            if (aux_processo_disco) {
                if (aux_processo.estado == PRONTO) 
                    aux_processo_disco->processo.estado = PRONTO;
                else if (aux_processo.estado == BLOQUEADO) 
                    aux_processo_disco->processo.estado = BLOQUEADO;
            }
        }
    }
}

void gerencia_filas_feedback(ARM *disco_rigido, MP *ram) {
    // Processa a fila prontosRQ0
    F *aux = ram->prontosRQ0;
    F *prev = NULL;
    while (aux) {
        P *processo = &aux->processo;

        // Se o processo tiver excedido seu quantum na fila RQ0, movê-lo para RQ1
        if (processo->tempoEmFila >= QUANTUM) {
            processo->tempoEmFila = 0;
            processo->indice_fila = verifica_fila(*processo);
            ram->prontosRQ1 = insere_na_fila(ram->prontosRQ1, *processo);
            if (prev)
                prev->prox = aux->prox;
            else 
                ram->prontosRQ0 = aux->prox;
            F *tmp = aux;
            aux = aux->prox;
            free(tmp);
        } else {
            prev = aux;
            aux = aux->prox;
        }
    }

    // Processa a fila prontosRQ1
    aux = ram->prontosRQ1;
    prev = NULL;
    while (aux) {
        P *processo = &aux->processo;

        // Se o processo tiver excedido seu quantum na fila RQ1, movê-lo para RQ2
        if (processo->tempoEmFila >= QUANTUM) {
            processo->tempoEmFila = 0;
            processo->indice_fila = verifica_fila(*processo);
            ram->prontosRQ2 = insere_na_fila(ram->prontosRQ2, *processo);
            if (prev) 
                prev->prox = aux->prox;
            else 
                ram->prontosRQ1 = aux->prox;
            F *tmp = aux;
            aux = aux->prox;
            free(tmp);
        } else {
            prev = aux;
            aux = aux->prox;
        }
    }

    // Processa a fila prontosRQ2
    aux = ram->prontosRQ2;
    prev = NULL;
    while (aux) {
        P *processo = &aux->processo;

        // Se o processo tiver excedido seu quantum na fila RQ2, mantê-lo na mesma fila porem joga ele pro final da fila
        if (processo->tempoEmFila >= QUANTUM) {
            processo->tempoEmFila = 0;
            // Retirar o processo da posição atual
            if (prev)
                prev->prox = aux->prox;
            else 
                ram->prontosRQ2 = aux->prox;
            
            // Inserir o processo no final da fila
            aux->prox = NULL;
            ram->prontosRQ2 = insere_na_fila(ram->prontosRQ2, *processo);

            // Atualizar ponteiro auxiliar
            F *tmp = aux;
            aux = aux->prox;
            free(tmp);        
        } else {
            prev = aux;
            aux = aux->prox;
        }
    }
    // Após mover processos entre as filas, chama o swapperMS para gerenciar a memória
    swapperMS(disco_rigido, ram);
}

int verifica_fila(P processo){
    if (processo.indice_fila == -1) return 0;
    if (processo.indice_fila == 0) return 1;
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
        CPU indice_cpu = *aux->cpu;
        
        if (indice_cpu.processo.id_processo == -1) {
            printf(" %d   |     -1       |       -1          |       -1        |       -1        \n", indice_cpu.indice);
            printf("------------------------------------------------------------------------------\n");

        } else {
            printf(" %d   |      %d       |        %d          |        %d        |        %d        \n",
                    indice_cpu.indice,
                    indice_cpu.processo.id_processo,
                    indice_cpu.processo.controle_fase1, 
                    indice_cpu.processo.controle_es, 
                    indice_cpu.processo.controle_fase2);
            printf("------------------------------------------------------------------------------\n");
        }
        aux = aux->prox;
    }
}

F *retira_da_fila(F *fila, P processo){
    F *atual = fila, *anterior = NULL;
    while((atual) && (atual->processo.id_processo != processo.id_processo)){
        anterior = atual;
        atual = atual->prox;
    }
    if(!atual) return fila;
    if(!anterior) fila = fila->prox;
    else anterior->prox = atual->prox;
    free(atual);
    return fila;
}

CPU cpu_disponivel(CPU cpu1, CPU cpu2, CPU cpu3, CPU cpu4){
    if(cpu1.processo.id_processo == -1) return cpu1;
    if(cpu2.processo.id_processo == -1) return cpu2;
    if(cpu3.processo.id_processo == -1) return cpu3;
    if(cpu4.processo.id_processo == -1) return cpu4;
    CPU tmp;
    tmp.indice = -1;
    tmp.processo.id_processo = -1;
    return tmp;
}

int fase_do_processo(P processo){
    // retorna 1 se for a fase 1, 2 se for a fase de e/s e 3 se for a fase 2
    if((processo.controle_fase1 > 0) && (processo.controle_es > 0) 
        && (processo.controle_fase2 > 0)) return 1;
    if((processo.controle_fase1 == 0) && (processo.controle_es > 0)
        && (processo.controle_fase2 > 0)) return 2;
    if((processo.controle_fase1 == 0) && (processo.controle_es == 0) 
        && (processo.controle_fase2 > 0)) return 3;
}

void execucao(ARM disco_rigido, MP *ram, P processo, CPU *indice_cpu){
    if(processo.estado == PRONTO){
        insere_CPU(disco_rigido, ram, processo, indice_cpu);
    }
    // Chama o swapperMS periodicamente durante a execução do escalonador
    swapperMS(&disco_rigido, ram);
}

F *busca_processo_fila(F *fila, P processo){
    F *aux = fila;
    while((aux) && (aux->processo.id_processo != processo.id_processo)){
        aux = aux->prox;
    }
    return aux;
}

void insere_CPU(ARM disco_rigido, MP *ram, P processo, CPU *indice_cpu){
    // atualiza o contexto do processo
    processo.estado = EXECUTANDO;
    int fase = fase_do_processo(processo);
    switch (fase){
    case 1:
        processo.controle_fase1--;
        break;
    case 3:
        processo.duracao_fase2--;
    default:
        ("ERRO \n");
        break;
    }
    // atualiza o contexto da mp
    int tmp = verifica_fila(processo);
    switch (tmp){
        case 0:
            ram->prontosRQ0 = retira_da_fila(ram->prontosRQ0, processo);
            break;
        case 1:
            ram->prontosRQ1 = retira_da_fila(ram->prontosRQ1, processo);
            break;
        case 2:
            ram->prontosRQ2 = retira_da_fila(ram->prontosRQ2, processo);
            break;
        default:
            break;
    } 

    F *aux = ram->processos;
    while(aux){
        if(aux->processo.id_processo == processo.id_processo){
            aux->processo = processo;
            aux->processo.indice_fila = -1;
            break;
        }
        aux = aux->prox;
    }
    
    // atualiza o contexto do processo no armazenamento
    F *tmp_fila = disco_rigido.processos;

    while(tmp_fila){
        if(tmp_fila->processo.id_processo == processo.id_processo){
            tmp_fila->processo = processo;
            tmp_fila->processo.indice_fila = -1;
            break;
        }
        tmp_fila = tmp_fila->prox;
    }

    // aloca na cpu
    indice_cpu->processo = processo;
}

F* insere_na_fila(F *fila, P processo){
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
    int duracao_es, int duracao_fase2, int tam, int numero_discos, ARM disco_rigido, MP ram){
        printf("malloc novo\n");
    F *novo = (F*)malloc(sizeof(F));
    if(!novo) exit(1);
    printf("construindo atributos\n");
    // Guarda as infos nas estruturas necessárias
    novo->processo.id_processo = id_processo;
    printf("id_processo atribuído: %d\n", novo->processo.id_processo);
    novo->processo.chegada = chegada;
    printf("chegada atribuída: %d\n", novo->processo.chegada);
    novo->processo.duracao_fase1 = duracao_fase1;
    printf("duracao_fase1 atribuída: %d\n", novo->processo.duracao_fase1);
    novo->processo.duracao_es = duracao_es;
    printf("duracao_es atribuída: %d\n", novo->processo.duracao_es);
    novo->processo.duracao_fase2 = duracao_fase2;
    printf("duracao_fase2 atribuída: %d\n", novo->processo.duracao_fase2);
    novo->processo.tam = tam;
    printf("tam atribuído: %d\n", novo->processo.tam);
    novo->processo.tabela_paginas = cria_paginas_processo(&(novo->processo), ram.tamanho_pagina);
    novo->processo.tabela_paginas->qtd_paginas = (novo->processo.tam + ram.tamanho_pagina - 1) / ram.tamanho_pagina;
    printf("qtd_paginas atribuída: %d\n", novo->processo.tabela_paginas->qtd_paginas);
    novo->processo.controle_fase1 = novo->processo.duracao_fase1;
    printf("controle_fase1 atribuído: %d\n", novo->processo.controle_fase1);

    novo->processo.controle_es = novo->processo.duracao_es;
    printf("controle_es atribuído: %d\n", novo->processo.controle_es);

    novo->processo.controle_fase2 = novo->processo.duracao_fase2;
    printf("controle_fase2 atribuído: %d\n", novo->processo.controle_fase2);

    novo->processo.indice_fila = -1;
    printf("indice_fila atribuído: %d\n", novo->processo.indice_fila);
    // Foi carregado em memória, vai para o estado novo
    novo->processo.estado = NOVO;
    novo->processo.numero_discos = numero_discos;
    // Se a fila estiver nula, o novo processo se torna o primeiro da fila e retornamos ele
    printf("atributos construidos");
    if(!disco_rigido.processos) {
        novo->prox = disco_rigido.processos;
        return novo;
    }
    F *aux = disco_rigido.processos;
    while(aux->prox != NULL){
        aux = aux->prox;
    }
    aux->prox = novo;
    novo->prox = NULL;
    return disco_rigido.processos;
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

void visualiza_ARM(ARM disco_rigido) {
    F* aux = disco_rigido.processos;

    printf("-------------------------------------------------------\n");
    printf("             LISTA DE PROCESSOS EXISTENTES             \n");
    printf("------------+--------------+--------+------------------\n");
    printf("ID Processo | Tamanho (MB) | Estado | Índice da Fila\n");
    printf("------------+--------------+--------+------------------\n");

    while(aux) {
        printf("%11d | %12d | %6d | %13d\n", 
               aux->processo.id_processo, 
               aux->processo.tam, 
               aux->processo.estado, 
               aux->processo.indice_fila);
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
               tmp->processo.id_processo, 
               tmp->processo.tabela_paginas->qtd_paginas, 
               tmp->processo.tam, 
               fase_do_processo(tmp->processo));
        tmp = tmp->prox;
    }
    printf("-------------------------------------------------------\n\n");
}


void visualiza_MP(MP ram) {
    F* aux = ram.processos;

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
                   aux->processo.id_processo, 
                   aux->processo.tabela_paginas->qtd_paginas, 
                   aux->processo.tam);
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
    aux = ram.prontosRQ0;
    if(!aux) {
        printf("        -1         |    RQ0   \n");
    } else {
        while(aux) {
            printf("         %d         |    RQ0   \n",aux->processo.id_processo);
            aux = aux->prox;
        }
    }

    aux = ram.prontosRQ1;
    if(!aux) {
        printf("        -1         |    RQ1   \n");
    } else {
        while(aux) {
            printf("         %d         |    RQ1   \n",aux->processo.id_processo);
            aux = aux->prox;
        }
    }

    aux = ram.prontosRQ2;
    if(!aux) {
        printf("        -1         |    RQ2   \n");
    } else {
        while(aux) {
            printf("         %d         |    RQ2   \n",aux->processo.id_processo);
            aux = aux->prox;
        }
    }
    printf("-------------------------------------------------------\n");

    // Lista de bloqueados
    printf("\n\n                LISTA DE BLOQUEADOS:                \n");

    printf("-------------------------------------------------------\n");
    printf("  ID Processo |\n");
    printf("--------------+----------------------------------------\n");
    aux = ram.bloqueados;
    if(!aux) {
        printf("     -1       |\n");
    } else {
        while(aux) {
            printf("       %d     |\n", aux->processo.id_processo);
            aux = aux->prox;
        }
    }
    printf("-------------------------------------------------------\n\n");

    // Informações sobre páginas
    printf("Numero total de paginas:        %d\n", ram.numero_paginas);
    printf("Numero de paginas disponiveis:  %d\n\n", ram.paginas_disponiveis);
    printf("-------------------------------------------------------\n");
}

P busca_processo_ARM(ARM disco_rigido, P processos){
    F *aux = disco_rigido.processos;

    // procura no disco
    while(aux){
        if(aux->processo.id_processo == processos.id_processo) return aux->processo;
        aux= aux->prox;
    }

    // se não retornou, então o elemento não existe
    P proc;
    // se o id_processo for -1, elemento não existe
    proc.id_processo = -1;

    return proc;
}

void* endereco_real(void* endereco_virtual, void* endereco_pagina, unsigned int tamanho_pagina_bytes) {
    uintptr_t virtual = (uintptr_t) endereco_virtual;
    uintptr_t pagina = (uintptr_t) endereco_pagina;
    uintptr_t offset = virtual % tamanho_pagina_bytes;
    return (void*)(pagina + offset);
}

// Listas encaedadas
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

//Função que recebe um processo e a quantidade de discos que ele precisa para executar
//Checa quantos discos estão disponíveis no momento, e caso tenha o suficiente ou mais, aloca disco para o processo
//Se não houver suficientes não faz nada
void insereProcessoNoDMA(){

}

//percorre o DMA e salva os processos que possuem 1 ou mais discos em uma lista
//para cada processo na lista diminui em 1 o tempo necessario na fase de entrada e saida
void executaDMA(){

}