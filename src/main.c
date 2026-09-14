#include <stdio.h>

/* Declaração da função gerada pelo Bison */
extern int yyparse(void);

int main(void) {
    printf("Iniciando compilador Mini-JS...\n");
    printf("Digite o codigo JS e aperte Ctrl+D para finalizar a leitura:\n\n");
    
    if (yyparse() == 0) {
        printf("\nCompilacao bem-sucedida! Nao ha erros de sintaxe.\n");
    } else {
        printf("\nFalha na compilacao.\n");
    }
    
    return 0;
}