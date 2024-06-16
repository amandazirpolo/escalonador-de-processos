#include "lib.c"
void print_tabela_paginas(T_TABELA_PAGINAS *tabela) {
    for (int i = 0; i < tabela->qtd_paginas; i++) {
        printf("Pagina %d: i_quadro=%d, presente_mp=%d, modificado=%d\n", 
               i, 
               tabela->array_de_paginas[i].i_quadro, 
               tabela->array_de_paginas[i].presente_mp, 
               tabela->array_de_paginas[i].modificado);
    }
}

int main() {
    int qtd_paginas = 5;
    T_TABELA_PAGINAS *tabela = new_tabela_paginas(qtd_paginas);

    printf("Tabela de Paginas Inicializada:\n");
    print_tabela_paginas(tabela);

    

    return 0;
}