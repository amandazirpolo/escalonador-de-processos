#include <stdio.h>;
#include <stdlib.h>;
#include "lib.h"
#include "memory_manager.h"

int tem_memoria(int total_ram, int tamanho_processo){
    return total_ram - tamanho_processo;
}

int calcula_paginas_processo(int tamanho_processo, int tamanho_pagina) {
    int result = tamanho_processo / tamanho_pagina;
    if (tamanho_processo % tamanho_pagina != 0)
        result ++;

    return result;
}
int tem_pagina(int tamanho_processo, int total_ram, int disponivel_ram, int tamanho_pagina, int paginas_disponiveis){
    return paginas_disponiveis > calcula_paginas_processo(tamanho_processo, tamanho_pagina);
}

