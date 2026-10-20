#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stddef.h>

typedef struct item ITEM;

/*
 * Cria um ITEM com uma referencia pertencente ao chamador. Cada insercao bem-
 * sucedida em um TAD retém outra referencia; cada remocao transfere a referencia
 * do no ao chamador e cada destruicao de estrutura libera as referencias nos.
 * item_apagar libera uma referencia e apaga o ITEM quando a ultima for liberada.
 */
ITEM *item_criar(void);

/* Libera o ITEM, seus nomes/valores de campo e a mensagem. Aceita ponteiro nulo. */
void item_apagar(ITEM **item);
bool item_reter(ITEM *item);

/* Chave inteira legada usada nas buscas atuais das listas. */
void item_set_chave(ITEM *item, int chave);
int item_get_chave(const ITEM *item);
int item_chave(const ITEM *item);

/* Mensagem legada usada para imprimir itens em pilha, fila e listas. */
bool item_set_mensagem(ITEM *item, const char *mensagem);
const char *item_get_mensagem(const ITEM *item);

/*
 * Campos flexiveis para leitores de datasets. A ordem de insercao e mantida;
 * atualizar um campo existente troca seu valor sem mudar sua posicao. Strings
 * sao copiadas pelo ITEM, e valor NULL equivale a string vazia.
 */
bool item_set_campo(ITEM *item, const char *nome, const char *valor);
const char *item_get_campo(const ITEM *item, const char *nome);
size_t item_num_campos(const ITEM *item);
const char *item_nome_campo(const ITEM *item, size_t indice);
const char *item_valor_campo(const ITEM *item, size_t indice);

/* Conversoes estritas; retornam false se o campo faltar ou nao for conversivel. */
bool item_get_numero(const ITEM *item, const char *nome, double *valor);
bool item_get_booleano(const ITEM *item, const char *nome, bool *valor);

/* Imprime a mensagem definida ou, se vazia, uma representacao resumida. */
void item_imprimir(const ITEM *item);

#endif
