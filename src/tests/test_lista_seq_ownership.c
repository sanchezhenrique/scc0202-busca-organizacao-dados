#include <assert.h>

#include "lista_seq.h"

int main(void) {
    ITEM *item = item_criar();
    assert(item != NULL);
    item_set_chave(item, 17);

    LISTA_SEQ *lista = lista_seq_criar(false);
    assert(lista != NULL && lista_seq_inserir(lista, item));
    item_apagar(&item); /* A lista mantém sua própria referência. */
    lista_seq_apagar(&lista);
    assert(lista == NULL);
    return 0;
}
