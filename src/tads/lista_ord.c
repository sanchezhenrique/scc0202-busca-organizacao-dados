#include "../includes/lista_ord.h"

#include <stdlib.h>
#include <string.h>

typedef struct no_ord_ NO_ORD;

struct no_ord_ {
    ITEM *item;
    double valor_ordenacao;
    NO_ORD *proximo;
};

struct lista_ord_ {
    NO_ORD *cabeca; /* Sentinela sem ITEM simplifica inserção e remoção inicial. */
    char *campo;
    bool decrescente;
    int tamanho;
};

static char *copiar_string(const char *texto) {
    size_t tamanho = strlen(texto) + 1;
    char *copia = malloc(tamanho);
    if (copia != NULL)
        memcpy(copia, texto, tamanho);
    return copia;
}

LISTA_ORD *lista_ord_criar(const char *campo, bool decrescente) {
    if (campo == NULL || campo[0] == '\0')
        return NULL;

    LISTA_ORD *lista = malloc(sizeof(*lista));
    if (lista == NULL)
        return NULL;

    lista->campo = copiar_string(campo);
    if (lista->campo == NULL) {
        free(lista);
        return NULL;
    }

    lista->cabeca = malloc(sizeof(*lista->cabeca));
    if (lista->cabeca == NULL) {
        free(lista->campo);
        free(lista);
        return NULL;
    }

    lista->cabeca->item = NULL;
    lista->cabeca->proximo = NULL;
    lista->decrescente = decrescente;
    lista->tamanho = 0;
    return lista;
}

void lista_ord_apagar(LISTA_ORD **lista) {
    if (lista == NULL || *lista == NULL)
        return;

    NO_ORD *atual = (*lista)->cabeca;
    while (atual != NULL) {
        NO_ORD *proximo = atual->proximo;
        if (atual->item != NULL)
            item_apagar(&atual->item);
        free(atual);
        atual = proximo;
    }
    free((*lista)->campo);
    free(*lista);
    *lista = NULL;
}

bool lista_ord_inserir(LISTA_ORD *lista, ITEM *item) {
    if (lista == NULL || item == NULL)
        return false;

    double valor_novo;
    if (!item_get_numero(item, lista->campo, &valor_novo))
        return false;

    NO_ORD *novo = malloc(sizeof(*novo));
    if (novo == NULL)
        return false;
    if (!item_reter(item)) {
        free(novo);
        return false;
    }
    novo->item = item;
    novo->valor_ordenacao = valor_novo;

    /* Avança sobre valores iguais para manter a ordem de chegada estável. */
    NO_ORD *anterior = lista->cabeca;
    while (anterior->proximo != NULL &&
           (lista->decrescente
                ? anterior->proximo->valor_ordenacao >= valor_novo
                : anterior->proximo->valor_ordenacao <= valor_novo))
        anterior = anterior->proximo;
    novo->proximo = anterior->proximo;
    anterior->proximo = novo;
    lista->tamanho++;
    return true;
}

ITEM *lista_ord_buscar(const LISTA_ORD *lista, int chave) {
    if (lista == NULL)
        return NULL;

    const NO_ORD *atual = lista->cabeca->proximo;
    while (atual != NULL && item_get_chave(atual->item) != chave)
        atual = atual->proximo;
    return atual != NULL ? atual->item : NULL;
}

ITEM *lista_ord_remover(LISTA_ORD *lista, int chave) {
    if (lista == NULL)
        return NULL;

    NO_ORD *anterior = lista->cabeca;
    while (anterior->proximo != NULL &&
           item_get_chave(anterior->proximo->item) != chave)
        anterior = anterior->proximo;
    NO_ORD *atual = anterior->proximo;
    if (atual == NULL)
        return NULL;

    anterior->proximo = atual->proximo;
    ITEM *item = atual->item; /* Transfere ao chamador a referência do nó. */
    free(atual);
    lista->tamanho--;
    return item;
}

bool lista_ord_percorrer(const LISTA_ORD *lista, LISTA_VISITANTE visitante,
                         void *contexto) {
    if (lista == NULL || visitante == NULL)
        return false;

    for (const NO_ORD *atual = lista->cabeca->proximo;
         atual != NULL; atual = atual->proximo) {
        if (!visitante(atual->item, contexto))
            return false;
    }
    return true;
}

int lista_ord_tamanho(const LISTA_ORD *lista) {
    return lista != NULL ? lista->tamanho : 0;
}

bool lista_ord_vazia(const LISTA_ORD *lista) {
    return lista == NULL || lista->cabeca->proximo == NULL;
}
