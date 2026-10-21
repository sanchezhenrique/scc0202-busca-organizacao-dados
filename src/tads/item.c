#include "../includes/item.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    /* Nome e valor pertencem ao ITEM; nenhum deles aponta para o buffer do CSV. */
    char *nome;
    char *valor;
} CAMPO;

struct item {
    /* Uma referencia inicial do chamador mais uma por cada no que o armazena. */
    size_t referencias;
    /* Chave usada pelas operacoes legadas; nao substitui os campos do dataset. */
    int chave;
    char *mensagem;
    /* Vetor dinamico preservado na ordem em que o leitor adiciona as colunas. */
    CAMPO *campos;
    size_t quantidade;
    size_t capacidade;
};

/* Faz uma copia propria da string; NULL representa um campo textual vazio. */
static char *string_copiar(const char *origem) {
    if (origem == NULL)
        origem = "";

    size_t tamanho = strlen(origem) + 1;
    char *copia = malloc(tamanho);
    if (copia != NULL)
        memcpy(copia, origem, tamanho);
    return copia;
}

/* Compara booleanos sem diferenciar maiusculas e minusculas ou espacos externos. */
static bool strings_iguais(const char *a, const char *b) {
    if (a == NULL || b == NULL)
        return false;

    while (isspace((unsigned char)*a))
        a++;
    while (isspace((unsigned char)*b))
        b++;

    size_t tamanho_a = strlen(a);
    size_t tamanho_b = strlen(b);
    while (tamanho_a > 0 && isspace((unsigned char)a[tamanho_a - 1]))
        tamanho_a--;
    while (tamanho_b > 0 && isspace((unsigned char)b[tamanho_b - 1]))
        tamanho_b--;
    if (tamanho_a != tamanho_b)
        return false;

    for (size_t i = 0; i < tamanho_a; i++) {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i]))
            return false;
    }
    return true;
}

ITEM *item_criar(void) {
    /* calloc inicializa chave, vetor e contadores como zero/NULL. */
    ITEM *item = calloc(1, sizeof(*item));
    if (item == NULL)
        return NULL;
    item->referencias = 1;

    item->mensagem = string_copiar("");
    if (item->mensagem == NULL) {
        free(item);
        return NULL;
    }
    return item;
}

void item_apagar(ITEM **item) {
    if (item == NULL || *item == NULL)
        return;

    ITEM *liberado = *item;
    *item = NULL;
    if (--liberado->referencias != 0)
        return;

    /* Cada campo possui duas strings alocadas independentemente. */
    for (size_t i = 0; i < liberado->quantidade; i++) {
        free(liberado->campos[i].nome);
        free(liberado->campos[i].valor);
    }
    free(liberado->campos);
    free(liberado->mensagem);
    free(liberado);
}

bool item_reter(ITEM *item) {
    if (item == NULL || item->referencias == (size_t)-1)
        return false;
    item->referencias++;
    return true;
}

void item_set_chave(ITEM *item, int chave) {
    if (item != NULL)
        item->chave = chave;
}

int item_get_chave(const ITEM *item) {
    return item != NULL ? item->chave : 0;
}

int item_chave(const ITEM *item) {
    return item_get_chave(item);
}

bool item_set_mensagem(ITEM *item, const char *mensagem) {
    if (item == NULL)
        return false;

    char *copia = string_copiar(mensagem);
    if (copia == NULL)
        return false;

    free(item->mensagem);
    item->mensagem = copia;
    return true;
}

const char *item_get_mensagem(const ITEM *item) {
    return item != NULL ? item->mensagem : NULL;
}

bool item_set_campo(ITEM *item, const char *nome, const char *valor) {
    if (item == NULL || nome == NULL || nome[0] == '\0')
        return false;

    /* Busca linear: para um dataset, cada ITEM costuma ter poucas dezenas de campos. */
    size_t indice = 0;
    while (indice < item->quantidade &&
           strcmp(item->campos[indice].nome, nome) != 0)
        indice++;

    char *valor_copiado = string_copiar(valor);
    if (valor_copiado == NULL)
        return false;

    if (indice < item->quantidade) {
        /* Atualizacao nao altera a ordem das colunas. */
        free(item->campos[indice].valor);
        item->campos[indice].valor = valor_copiado;
        return true;
    }

    char *nome_copiado = string_copiar(nome);
    if (nome_copiado == NULL) {
        free(valor_copiado);
        return false;
    }

    if (item->quantidade == item->capacidade) {
        /* Crescimento geometrico evita realloc a cada campo inserido. */
        size_t nova_capacidade = item->capacidade == 0 ? 8 : item->capacidade * 2;
        if (nova_capacidade < item->capacidade ||
            nova_capacidade > (size_t)-1 / sizeof(*item->campos)) {
            free(nome_copiado);
            free(valor_copiado);
            return false;
        }

        CAMPO *novos_campos = realloc(item->campos,
                                     nova_capacidade * sizeof(*item->campos));
        if (novos_campos == NULL) {
            free(nome_copiado);
            free(valor_copiado);
            return false;
        }
        item->campos = novos_campos;
        item->capacidade = nova_capacidade;
    }

    item->campos[item->quantidade].nome = nome_copiado;
    item->campos[item->quantidade].valor = valor_copiado;
    item->quantidade++;
    return true;
}

const char *item_get_campo(const ITEM *item, const char *nome) {
    if (item == NULL || nome == NULL)
        return NULL;

    /* Retorna NULL se o campo nao existe; campo vazio retorna a string "". */
    for (size_t i = 0; i < item->quantidade; i++)
        if (strcmp(item->campos[i].nome, nome) == 0)
            return item->campos[i].valor;
    return NULL;
}

size_t item_num_campos(const ITEM *item) {
    return item != NULL ? item->quantidade : 0;
}

const char *item_nome_campo(const ITEM *item, size_t indice) {
    if (item == NULL || indice >= item->quantidade)
        return NULL;
    return item->campos[indice].nome;
}

const char *item_valor_campo(const ITEM *item, size_t indice) {
    if (item == NULL || indice >= item->quantidade)
        return NULL;
    return item->campos[indice].valor;
}

bool item_get_numero(const ITEM *item, const char *nome, double *valor) {
    if (valor == NULL)
        return false;

    const char *texto = item_get_campo(item, nome);
    if (texto == NULL || texto[0] == '\0')
        return false;

    char *fim = NULL;
    double convertido = strtod(texto, &fim);

    if (fim == texto || !isfinite(convertido))
        return false;

    while (*fim != '\0' && isspace((unsigned char)*fim))
        fim++;

    if (*fim != '\0')
        return false;

    *valor = convertido;
    return true;
}

bool item_get_booleano(const ITEM *item, const char *nome, bool *valor) {
    if (valor == NULL)
        return false;

    const char *texto = item_get_campo(item, nome);
    if (texto == NULL)
        return false;

    /* Aceita as grafias comuns sem transformar valores desconhecidos em false. */
    if (strings_iguais(texto, "true") || strings_iguais(texto, "1") ||
        strings_iguais(texto, "yes") || strings_iguais(texto, "sim")) {
        *valor = true;
        return true;
    }
    if (strings_iguais(texto, "false") || strings_iguais(texto, "0") ||
        strings_iguais(texto, "no") || strings_iguais(texto, "nao") ||
        strings_iguais(texto, "não")) {
        *valor = false;
        return true;
    }
    return false;
}

void item_imprimir(const ITEM *item) {
    if (item == NULL) {
        printf("<item inexistente>");
        return;
    }

    const char *mensagem = item_get_mensagem(item);
    if (mensagem != NULL && mensagem[0] != '\0')
        printf("%s", mensagem);
    else
        printf("ITEM(chave=%d, campos=%zu)", item->chave, item->quantidade);
}
