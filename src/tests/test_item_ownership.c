#include <assert.h>

#include "Pilha.h"
#include "fila.h"
#include "lista_enc.h"

int main(void) {
    ITEM *compartilhado = item_criar();
    assert(compartilhado != NULL);
    item_set_chave(compartilhado, 42);
    assert(item_set_mensagem(compartilhado, "registro compartilhado"));

    PILHA *pilha = pilha_criar();
    FILA *fila = fila_criar();
    LISTA_ENC *lista = lista_enc_criar(false);
    assert(pilha != NULL && fila != NULL && lista != NULL);
    assert(pilha_empilhar(pilha, compartilhado));
    assert(fila_inserir(fila, compartilhado));
    assert(lista_enc_inserir(lista, compartilhado));

    /* Cada insercao retém uma referencia; main libera a referencia inicial. */
    ITEM *emprestado = lista_enc_buscar(lista, 42);
    assert(emprestado == compartilhado);
    item_apagar(&compartilhado);
    assert(compartilhado == NULL);

    /* As outras estruturas ainda mantêm o mesmo ITEM válido. */
    assert(item_get_chave(emprestado) == 42);
    pilha_apagar(&pilha);
    fila_apagar(&fila);
    assert(item_get_chave(emprestado) == 42);
    lista_enc_apagar(&lista);
    assert(pilha == NULL && fila == NULL && lista == NULL);

    /* Remover tambem devolve a referencia sem liberar o registro. */
    ITEM *compartilhado2 = item_criar();
    assert(compartilhado2 != NULL);
    item_set_chave(compartilhado2, 43);
    lista = lista_enc_criar(false);
    assert(lista != NULL && lista_enc_inserir(lista, compartilhado2));
    item_apagar(&compartilhado2);
    ITEM *removido = lista_enc_remover(lista, 43);
    assert(removido != NULL && item_get_chave(removido) == 43);
    item_apagar(&removido); /* Libera a referencia transferida pela remocao. */
    assert(removido == NULL);
    lista_enc_apagar(&lista);

    ITEM *item_pilha = item_criar();
    assert(item_pilha != NULL);
    item_set_chave(item_pilha, 51);
    pilha = pilha_criar();
    assert(pilha != NULL && pilha_empilhar(pilha, item_pilha));
    item_apagar(&item_pilha);
    ITEM *desempilhado = pilha_desempilhar(pilha);
    assert(desempilhado != NULL && item_get_chave(desempilhado) == 51);
    pilha_apagar(&pilha);
    item_apagar(&desempilhado);

    ITEM *item_fila = item_criar();
    assert(item_fila != NULL);
    item_set_chave(item_fila, 52);
    fila = fila_criar();
    assert(fila != NULL && fila_inserir(fila, item_fila));
    item_apagar(&item_fila);
    ITEM *removido_fila = fila_remover(fila);
    assert(removido_fila != NULL && item_get_chave(removido_fila) == 52);
    fila_apagar(&fila);
    item_apagar(&removido_fila);

    return 0;
}
