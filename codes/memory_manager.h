#include <stdio.h>
#include <stdlib.h>

typedef struct pagina {
    void* endereco_inicial;
    unsigned int tamanho_alocado;
} T_PAGINA;

typedef struct tabela_paginas {
    T_PAGINA* bloco_de_paginas;
} T_TABELA_PAGINAS;

int tem_memoria(int total_ram, int tamanho_processo);

int calcula_paginas_processo(int tamanho_processo, int tamanho_pagina);

int tem_pagina(int tamanho_processo, int total_ram, int disponivel_ram, int tamanho_pagina, int paginas_disponiveis);

