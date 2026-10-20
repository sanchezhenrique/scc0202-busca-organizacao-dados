#include <assert.h>

#include "lista_gen.h"

int main(void) {
    ITEM *item = item_criar();
    assert(item != NULL);
    item_set_chave(item, 23);

    LISTA_GEN *raiz = lista_gen_criar();
    LISTA_GEN *sublista = lista_gen_criar();
    assert(raiz != NULL && sublista != NULL);
    assert(lista_gen_inserir_atomo(sublista, item));
    assert(lista_gen_inserir_sublista(raiz, sublista));
    sublista = NULL; /* A insercao transfere os nos da sublista para a raiz. */

    assert(lista_gen_buscar(raiz, 23) == item);
    item_apagar(&item); /* A lista generalizada mantém a própria referência. */
    lista_gen_apagar(&raiz);
    assert(raiz == NULL);
    return 0;
}
