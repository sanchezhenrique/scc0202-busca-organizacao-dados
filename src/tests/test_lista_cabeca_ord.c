#include <assert.h>

#include "lista_cabeca.h"
#include "lista_ord.h"

typedef struct {
    ITEM *itens[8];
    size_t quantidade;
} COLETA;

static bool coletar(ITEM *item, void *contexto){
    COLETA *coleta = contexto;
    assert(coleta->quantidade < sizeof(coleta->itens) / sizeof(coleta->itens[0]));
    coleta->itens[coleta->quantidade++] = item;
    return true;
}

static ITEM *novo_item(int chave){
    ITEM *item = item_criar();
    assert(item != NULL);
    item_set_chave(item, chave);
    return item;
}

static ITEM *novo_item_com_score(int chave, const char *score){
    ITEM *item = novo_item(chave);
    assert(item_set_campo(item, "similarity", score));
    return item;
}

static void testar_lista_cabeca(void){
    LISTA_CABECA *lista = lista_cabeca_criar();
    assert(lista != NULL && lista_cabeca_vazia(lista));
    assert(lista_cabeca_tamanho(lista) == 0);
    assert(lista_cabeca_buscar(lista, 10) == NULL);

    ITEM *a = novo_item(10);
    ITEM *b = novo_item(20);
    ITEM *c = novo_item(30);
    assert(lista_cabeca_inserir(lista, a));
    assert(lista_cabeca_inserir(lista, b));
    assert(lista_cabeca_inserir(lista, c));
    item_apagar(&a);
    item_apagar(&b);
    item_apagar(&c);

    COLETA coleta = {{0}, 0};
    assert(lista_cabeca_percorrer(lista, coletar, &coleta));
    assert(coleta.quantidade == 3);
    assert(item_get_chave(coleta.itens[0]) == 10);
    assert(item_get_chave(coleta.itens[1]) == 20);
    assert(item_get_chave(coleta.itens[2]) == 30);
    assert(lista_cabeca_buscar(lista, 20) == coleta.itens[1]);

    ITEM *removido = lista_cabeca_remover(lista, 10);
    assert(removido != NULL && item_get_chave(removido) == 10);
    item_apagar(&removido);
    removido = lista_cabeca_remover(lista, 30);
    assert(removido != NULL && item_get_chave(removido) == 30);
    item_apagar(&removido);
    assert(lista_cabeca_tamanho(lista) == 1);
    assert(lista_cabeca_remover(lista, 999) == NULL);

    removido = lista_cabeca_remover(lista, 20);
    assert(removido != NULL && lista_cabeca_vazia(lista));
    item_apagar(&removido);

    ITEM *apos_esvaziar = novo_item(40);
    assert(lista_cabeca_inserir(lista, apos_esvaziar));
    item_apagar(&apos_esvaziar);
    assert(lista_cabeca_buscar(lista, 40) != NULL);
    lista_cabeca_apagar(&lista);
    assert(lista == NULL);
}

static void testar_lista_ordenada(void){
    LISTA_ORD *lista = lista_ord_criar("similarity", true);
    assert(lista != NULL && lista_ord_vazia(lista));

    ITEM *chave_5a = novo_item_com_score(5, "0.61");
    ITEM *chave_2 = novo_item_com_score(2, "0.70");
    ITEM *chave_5b = novo_item_com_score(5, "0.61");
    ITEM *chave_1 = novo_item_com_score(1, "0.93");
    assert(lista_ord_inserir(lista, chave_5a));
    assert(lista_ord_inserir(lista, chave_2));
    assert(lista_ord_inserir(lista, chave_5b));
    assert(lista_ord_inserir(lista, chave_1));

    COLETA esperados = {{chave_1, chave_2, chave_5a, chave_5b}, 4};
    item_apagar(&chave_5a);
    item_apagar(&chave_2);
    item_apagar(&chave_5b);
    item_apagar(&chave_1);

    COLETA coleta = {{0}, 0};
    assert(lista_ord_percorrer(lista, coletar, &coleta));
    assert(coleta.quantidade == esperados.quantidade);
    for(size_t i = 0; i < coleta.quantidade; i++)
        assert(coleta.itens[i] == esperados.itens[i]);
    assert(lista_ord_buscar(lista, 5) == esperados.itens[2]);
    assert(lista_ord_buscar(lista, 99) == NULL);

    ITEM *removido = lista_ord_remover(lista, 5);
    assert(removido == esperados.itens[2]); /* Primeiro dos iguais inseridos. */
    item_apagar(&removido);
    assert(lista_ord_buscar(lista, 5) == esperados.itens[3]);
    assert(lista_ord_remover(lista, 99) == NULL);
    assert(lista_ord_tamanho(lista) == 3);
    lista_ord_apagar(&lista);
    assert(lista == NULL);

    lista = lista_ord_criar("similarity", true);
    assert(lista != NULL);
    ITEM *sem_score = novo_item(7);
    assert(!lista_ord_inserir(lista, sem_score));
    assert(lista_ord_tamanho(lista) == 0);
    item_apagar(&sem_score);
    lista_ord_apagar(&lista);
}

int main(void){
    testar_lista_cabeca();
    testar_lista_ordenada();
    return 0;
}
