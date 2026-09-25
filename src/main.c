#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Declarações externas do Flex e Bison */
extern int yyparse(void);
extern FILE *yyin;
extern int yynerrs;

/* Declaração das rotinas para manipulação de buffer em memória (Flex) */
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

int main(int argc, char *argv[]) {
    int res = 0;

    if (argc > 1) {
        /* Suporte a execução de código em memória via CLI (-e / --eval) */
        if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--eval") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Erro: nenhum codigo fornecido para a flag '%s'.\n", argv[1]);
                return 1;
            }
            YY_BUFFER_STATE buffer = yy_scan_string(argv[2]);
            res = yyparse();
            yy_delete_buffer(buffer);
            if (res == 0 && yynerrs > 0) {
                res = 1;
            }
            return res;
        }

        /* Leitura de arquivo fonte passado como argumento */
        FILE *f = fopen(argv[1], "r");
        if (!f) {
            fprintf(stderr, "Erro ao abrir arquivo '%s'\n", argv[1]);
            return 1;
        }
        yyin = f;
        res = yyparse();
        fclose(f);
        if (res == 0 && yynerrs > 0) {
            res = 1;
        }
        return res;
    }

    /* Leitura interativa via stdin */
    res = yyparse();
    if (res == 0 && yynerrs > 0) {
        res = 1;
    }
    return res;
}