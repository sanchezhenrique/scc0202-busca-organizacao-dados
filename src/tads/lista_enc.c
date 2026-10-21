#include "../includes/lista_enc.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct no_ NO;

/* Lista duplamente encadeada: cada no conhece os dois vizinhos. */
struct no_ {
    ITEM *item;
    NO *anterior;
    NO *proximo;
};

struct lista_enc_ {
    /* inicio/fim permitem insercao e percurso nos dois sentidos. */
    NO *inicio;
    NO *fim;
    int tamanho;
    bool ordenada;
};

static NO *no_criar(ITEM *item) {
    NO *novo = malloc(sizeof(NO));
    if (novo != NULL) {
        if (!item_reter(item)) {
            free(novo);
            return NULL;
        }
        novo->item = item;
        novo->anterior = NULL;
        novo->proximo = NULL;
    }
    return novo;
}

LISTA_ENC *lista_enc_criar(bool ordenada) {
    LISTA_ENC *lista = malloc(sizeof(LISTA_ENC));
    if (lista == NULL)
        return NULL;

    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    lista->ordenada = ordenada;
    return lista;
}

void lista_enc_apagar(LISTA_ENC **lista) {
    if (lista == NULL || *lista == NULL)
        return;

    NO *p = (*lista)->inicio;
    while (p != NULL) {
        NO *proximo = p->proximo;
        item_apagar(&p->item);
        free(p);
        p = proximo;
    }

    free(*lista);
    *lista = NULL;
}

bool lista_enc_inserir_inicio(LISTA_ENC *lista, ITEM *item) {
    if (lista == NULL || item == NULL || lista->ordenada)
        return false;

    NO *novo = no_criar(item);
    if (novo == NULL)
        return false;

    /* Insercao nas extremidades so e permitida no modo nao ordenado. */
    novo->proximo = lista->inicio;

    if (lista->inicio == NULL)
        lista->fim = novo;
    else
        lista->inicio->anterior = novo;

    lista->inicio = novo;
    lista->tamanho++;
    return true;
}

bool lista_enc_inserir_fim(LISTA_ENC *lista, ITEM *item) {
    if (lista == NULL || item == NULL || lista->ordenada)
        return false;

    NO *novo = no_criar(item);
    if (novo == NULL)
        return false;

    novo->anterior = lista->fim;

    if (lista->fim == NULL)
        lista->inicio = novo;
    else
        lista->fim->proximo = novo;

    lista->fim = novo;
    lista->tamanho++;
    return true;
}

bool lista_enc_inserir(LISTA_ENC *lista, ITEM *item) {
    if (lista == NULL || item == NULL)
        return false;
    if (!lista->ordenada)
        return lista_enc_inserir_fim(lista, item);

    NO *novo = no_criar(item);
    if (novo == NULL)
        return false;

    /* No modo ordenado, chaves iguais permanecem na ordem em que chegaram. */
    int chave = item_chave(item);
    NO *atual = lista->inicio;
    while (atual != NULL && item_chave(atual->item) <= chave)
        atual = atual->proximo;

    if (atual == NULL) {
        novo->anterior = lista->fim;
        if (lista->fim == NULL)
            lista->inicio = novo;
        else
            lista->fim->proximo = novo;
        lista->fim = novo;
    } else {
        novo->proximo = atual;
        novo->anterior = atual->anterior;
        if (atual->anterior == NULL)
            lista->inicio = novo;
        else
            atual->anterior->proximo = novo;
        atual->anterior = novo;
    }
    lista->tamanho++;
    return true;
}

bool lista_enc_percorrer(const LISTA_ENC *lista, LISTA_VISITANTE visitante,
                         void *contexto) {
    if (lista == NULL || visitante == NULL)
        return false;

    for (const NO *p = lista->inicio; p != NULL; p = p->proximo)
        if (!visitante(p->item, contexto))
            return false;
    return true;
}

ITEM *lista_enc_buscar(const LISTA_ENC *lista, int chave) {
    if (lista == NULL)
        return NULL;

    /* Busca sequencial, valida para ambos os modos da lista. */
    for (const NO *p = lista->inicio; p != NULL; p = p->proximo)
        if (item_chave(p->item) == chave)
            return p->item;

    return NULL;
}

ITEM *lista_enc_remover(LISTA_ENC *lista, int chave) {
    if (lista == NULL)
        return NULL;

    NO *p = lista->inicio;
    while (p != NULL && item_chave(p->item) != chave)
        p = p->proximo;

    if (p == NULL)
        return NULL;

    /* Reconecta vizinhos e trata separadamente remocao no inicio/fim. */
    if (p->anterior == NULL)
        lista->inicio = p->proximo;
    else
        p->anterior->proximo = p->proximo;

    if (p->proximo == NULL)
        lista->fim = p->anterior;
    else
        p->proximo->anterior = p->anterior;

    /* O no e liberado, mas sua referencia ao ITEM e transferida ao chamador. */
    ITEM *item = p->item;
    p->anterior = NULL;
    p->proximo = NULL;
    free(p);
    lista->tamanho--;
    return item;
}

int lista_enc_tamanho(const LISTA_ENC *lista) {
    return lista != NULL ? lista->tamanho : 0;
}

bool lista_enc_vazia(const LISTA_ENC *lista) {
    return lista == NULL || lista->inicio == NULL;
}

bool lista_enc_cheia(const LISTA_ENC *lista) {
    (void)lista;
    return false;
}

void lista_enc_imprimir(const LISTA_ENC *lista) {
    if (lista == NULL) {
        printf("<lista inexistente>\n");
        return;
    }

    printf("inicio -> ");
    for (const NO *p = lista->inicio; p != NULL; p = p->proximo) {
        item_imprimir(p->item);
        printf(" <-> ");
    }
    printf("NULL\n");
}

void lista_enc_imprimir_reversa(const LISTA_ENC *lista) {
    if (lista == NULL) {
        printf("<lista inexistente>\n");
        return;
    }

    printf("fim -> ");
    for (const NO *p = lista->fim; p != NULL; p = p->anterior) {
        item_imprimir(p->item);
        printf(" <-> ");
    }
    printf("NULL\n");
}