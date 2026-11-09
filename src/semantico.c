#include "semantico.h"
#include "tabela.h"

static void visita(No *no);

static void visita_lista(No *lista) {
    for (No *no = lista; no; no = no->proximo) {
        visita(no);
    }
}

static void visita_filhos(No *no) {
    for (int i = 0; i < NO_MAX_FILHOS; i++) {
        visita_lista(no->filhos[i]);
    }
}

static void visita_funcao(No *no) {
    escopo_entra();

    /* Parametros e corpo dividem o mesmo escopo: o bloco do corpo nao abre outro */
    No *corpo = no->filhos[1];
    if (corpo) {
        visita_lista(corpo->filhos[0]);
    }
    escopo_sai();
}

static void visita(No *no) {
    switch (no->categoria) {
        case NO_BLOCO:
        case NO_FOR:
            /* No for, o let da inicializacao fica neste escopo, por fora do bloco do corpo */
            escopo_entra();
            visita_filhos(no);
            escopo_sai();
            break;
        case NO_FUNCAO:
            visita_funcao(no);
            break;
        default:
            visita_filhos(no);
            break;
    }
}

void analisa(No *raiz) {
    visita_lista(raiz);
}
