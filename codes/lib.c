#include "lib.h"

/* funções vão nesse arquivo */
void inicializa_hardware(MP *ram, DMA *disco1, DMA *disco2, DMA *disco3, DMA *disco4, ARM *disco_rigido, CPU *cpu1, CPU *cpu2, CPU *cpu3, CPU *cpu4){
    // inicializa a ram
    ram->tam_total = 32768;
    ram->controle_memoria = 0;
    ram->tamanho_pagina = 1024; // cada pagina tem 1024 MB
    ram->numero_paginas = ram->tam_total / ram->tamanho_pagina; // ao todo são 32 paginas
    ram->paginas_disponiveis = ram->numero_paginas;
    ram->processos = NULL;
    ram->prontos = NULL;
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
    while(fscanf(arquivo, "%d, %d, %d, %d, %d, %d\n",
        &aux.chegada,
        &aux.duracao_fase1,
        &aux.duracao_es,
        &aux.duracao_fase2,
        &aux.tam,
        &aux.numero_discos) == 6){
            tmp = busca_processo_ARM(*disco_rigido, aux);
            if(tmp.id_processo == -1){
                disco_rigido->processos = cria_processo(id, aux.chegada, aux.duracao_fase1,
                    aux.duracao_es, aux.duracao_fase2, aux.tam, aux.numero_discos, *disco_rigido, ram);
            }
        id++;
        }
}

//void insere_bloqueados(ARM disco_rigido, MP *ram, P processo){}

void insere_MP(ARM disco_rigido, MP *ram, P processo){
    // se essa condição for maior que zero, entao tem espaço disponivel na memoria
    if(((ram->tam_total - ram->controle_memoria) > 0) && (ram->paginas_disponiveis) > 0){
        // atualiza o contexto do processo
        processo.estado = PRONTO;
        // atualiza o contexto da mp
        ram->controle_memoria = ram->controle_memoria - processo.tam;
        ram->paginas_disponiveis = ram->paginas_disponiveis - processo.qtd_paginas;
        ram->prontos = insere_na_fila(ram->prontos, processo);
        ram->processos = insere_na_fila(ram->processos, processo);
        // atualiza o contexto do processo no armazenamento
        F *aux = disco_rigido.processos;
        while(aux){
            if(aux->processo.id_processo == processo.id_processo){
                aux->processo.estado = PRONTO;
                break;
            }
            aux = aux->prox;
        }
    }
    /*else {
        swapper(disco_rigido, ram, processo);
        swapper vai tirar alguem da memoria principal, suspender, e atualizar
        a info do processo no disco rigido
    }*/
}

void visualiza_CPU (CPU indice_cpu){
    printf("ESTADO DA CPU %d: \n", indice_cpu.indice);
    if(indice_cpu.processo.id_processo == -1){
        printf("CPU %d DISPONIVEL \n", indice_cpu.indice);
        return;
    }
    else {
        printf("PROCESSO %d em EXECUCAO \n", indice_cpu.processo.id_processo);
        printf("tempo restante fase_1: %d\n", indice_cpu.processo.controle_fase1);
        printf("tempo restante e/s: %d\n", indice_cpu.processo.controle_es);
        printf("tempo restante fase_2: %d\n", indice_cpu.processo.controle_fase2);
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
    case 2:
        processo.duracao_es--;
        break;
    case 3:
        processo.duracao_fase2--;
    default:
        ("ERRO \n");
        break;
    }
    // atualiza o contexto da mp
    ram->prontos = retira_da_fila(ram->prontos, processo);
    F *aux = ram->processos;
    
    while(aux){
        if(aux->processo.id_processo == processo.id_processo){
            aux->processo = processo; //acho que o problema ta nessa linha
            break;
        }
        aux = aux->prox;
    }
    
    // atualiza o contexto do processo no armazenamento
    F *tmp = ram->processos;

    while(tmp){
        if(tmp->processo.id_processo == processo.id_processo){
            tmp->processo = processo;
            break;
        }
        tmp = tmp->prox;
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
    F *novo = (F*)malloc(sizeof(F));
    if(!novo) exit(1);
    // guarda as infos nas estruturas necessárias
    novo->processo.id_processo = id_processo;
    novo->processo.chegada = chegada;
    novo->processo.duracao_fase1 = duracao_fase1;
    novo->processo.duracao_es = duracao_es;
    novo->processo.duracao_fase2 = duracao_fase2;
    novo->processo.tam = tam;
    novo->processo.qtd_paginas = (novo->processo.tam + ram.tamanho_pagina - 1) / ram.tamanho_pagina;
    novo->processo.controle_fase1 = novo->processo.duracao_fase1;
    novo->processo.controle_es = novo->processo.duracao_es;
    novo->processo.controle_fase2 = novo->processo.duracao_fase2;  
    // foi carregado em memória, vai para o estado novo
    novo->processo.estado = NOVO;
    novo->processo.numero_discos = numero_discos;
    // Se a fila estiver nula, o novo processo se torna o primeiro da fila e retornamos ele
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

void visualiza_DMA(DMA id_disco){
    printf("DISCO %d: \n", id_disco.indice);
    if(id_disco.processo.id_processo == -1){
        printf("DISCO LIVRE \n");
    } else {
        printf("Processo %d em disco. \n", id_disco.processo.id_processo);
    }
}

void libera_fila(F *fila){
    F *aux = fila, *tmp;

    while(aux != NULL){
        tmp = aux;
        aux = aux->prox;
        free(tmp);
    }
}

void visualiza_ARM (ARM disco_rigido){
    F *aux = disco_rigido.processos;

    printf("\nLista de processos existentes: \n");
    while(aux){
        printf("id_processo: %d, tam_processo: %d MB, estado_processo: %d \n", 
        aux->processo.id_processo, aux->processo.tam, aux->processo.estado);
        aux = aux->prox;
    }
}

void visualiza_MP (MP ram){
    printf("\n\nESTADO DA MEMORIA PRINCIPAL: \n\n");
    
    printf("LISTA DE PROCESSOS EM MP: \n");
    F *aux = ram.processos;
    if(!aux) {
        printf("NENHUM PROCESSO EM MP. \n");
        return;
    }

    while(aux){
        printf("Processo %d - ", aux->processo.id_processo);
        aux = aux->prox;
    }
    printf("FIM. \n");

    printf("LISTA DE PRONTOS: \n");
    aux = ram.prontos;
    if(!aux) printf("NENHUM PROCESSO PRONTO. \n");
    
    while(aux){
        printf("Processo %d - ", aux->processo.id_processo);
        aux = aux->prox;
    }
    printf("FIM. \n");

    printf("LISTA DE BLOQUEADOS: \n");
    aux = ram.bloqueados;
    if(!aux) printf("NENHUM PROCESSO BLOQUEADO. \n");
    
    while(aux){
        printf("Processo %d - ", aux->processo.id_processo);
        aux = aux->prox;
    }
    printf("FIM. \n");

    printf("numero total de paginas: %d \n", ram.numero_paginas);
    printf("numero de paginas disponiveis: %d \n", ram.paginas_disponiveis);
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