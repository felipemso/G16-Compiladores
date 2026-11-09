#include "semantico.h"

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

static void visita(No *no) {
    visita_filhos(no);
}

void analisa(No *raiz) {
    visita_lista(raiz);
}
