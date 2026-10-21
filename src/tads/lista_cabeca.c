#include "../includes/lista_cabeca.h"

#include <stdlib.h>

typedef struct no_cabeca_ NO_CABECA;

struct no_cabeca_ {
    ITEM *item;
    NO_CABECA *proximo;
};

struct lista_cabeca_ {
    NO_CABECA *cabeca; /* Sentinela real; seu campo item permanece NULL. */
    NO_CABECA *fim;
    int tamanho;
};

LISTA_CABECA *lista_cabeca_criar(void) {
    LISTA_CABECA *lista = malloc(sizeof(*lista));
    if (lista == NULL)
        return NULL;

    lista->cabeca = malloc(sizeof(*lista->cabeca));
    if (lista->cabeca == NULL) {
        free(lista);
        return NULL;
    }

    lista->cabeca->item = NULL;
    lista->cabeca->proximo = NULL;
    lista->fim = lista->cabeca;
    lista->tamanho = 0;
    return lista;
}

void lista_cabeca_apagar(LISTA_CABECA **lista) {
    if (lista == NULL || *lista == NULL)
        return;

    NO_CABECA *atual = (*lista)->cabeca;
    while (atual != NULL) {
        NO_CABECA *proximo = atual->proximo;
        if (atual->item != NULL)
            item_apagar(&atual->item);
        free(atual);
        atual = proximo;
    }

    free(*lista);
    *lista = NULL;
}

bool lista_cabeca_inserir(LISTA_CABECA *lista, ITEM *item) {
    if (lista == NULL || item == NULL)
        return false;

    NO_CABECA *novo = malloc(sizeof(*novo));
    if (novo == NULL)
        return false;
    if (!item_reter(item)) {
        free(novo);
        return false;
    }

    novo->item = item;
    novo->proximo = NULL;
    lista->fim->proximo = novo;
    lista->fim = novo;
    lista->tamanho++;
    return true;
}

ITEM *lista_cabeca_buscar(const LISTA_CABECA *lista, int chave) {
    if (lista == NULL)
        return NULL;

    for (const NO_CABECA *atual = lista->cabeca->proximo;
         atual != NULL; atual = atual->proximo) {
        if (item_get_chave(atual->item) == chave)
            return atual->item;
    }
    return NULL;
}

ITEM *lista_cabeca_remover(LISTA_CABECA *lista, int chave) {
    if (lista == NULL)
        return NULL;

    NO_CABECA *anterior = lista->cabeca;
    NO_CABECA *atual = anterior->proximo;
    while (atual != NULL && item_get_chave(atual->item) != chave) {
        anterior = atual;
        atual = atual->proximo;
    }
    if (atual == NULL)
        return NULL;

    anterior->proximo = atual->proximo;
    if (lista->fim == atual)
        lista->fim = anterior;
    ITEM *item = atual->item; /* Transfere ao chamador a referência do nó. */
    free(atual);
    lista->tamanho--;
    return item;
}

bool lista_cabeca_percorrer(const LISTA_CABECA *lista,
                            LISTA_VISITANTE visitante, void *contexto) {
    if (lista == NULL || visitante == NULL)
        return false;

    for (const NO_CABECA *atual = lista->cabeca->proximo;
         atual != NULL; atual = atual->proximo) {
        if (!visitante(atual->item, contexto))
            return false;
    }
    return true;
}

int lista_cabeca_tamanho(const LISTA_CABECA *lista) {
    return lista != NULL ? lista->tamanho : 0;
}

bool lista_cabeca_vazia(const LISTA_CABECA *lista) {
    return lista == NULL || lista->cabeca->proximo == NULL;
}
