#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stdlib.h>

/* Uma linha de dados do CSV. A representacao fica somente em item.c. */
typedef struct item ITEM;

/* No sucesso, assume original, campos e suas strings sem copia-los.
 * Na falha, os argumentos continuam pertencendo ao chamador. */
ITEM *item_criar(char *original, char **campos, size_t quantidade,
                  size_t posicao);
/* Libera os dados da linha e zera o ponteiro do chamador. Aceita NULL. */
void item_apagar(ITEM **item);
/* Textos emprestados: nao liberar. Campos finais omitidos sao lidos como "". */
const char *item_campo(const ITEM *item, size_t indice);
const char *item_original(const ITEM *item);
size_t item_posicao(const ITEM *item);
bool item_numero(const ITEM *item, size_t indice, double *numero);

/* Auxiliares comuns aos valores dos itens, ao cabecalho e as consultas. */
/* Retorna false para ausentes, conversoes parciais, NaN ou infinito. */
bool texto_numero(const char *texto, double *numero);

/* Libera todas as strings e o vetor, inclusive durante a limpeza de uma falha. */
void campos_apagar(char ***campos, size_t quantidade);

char *texto_copiar(const char *texto);
bool texto_booleano(const char *texto, double *valor);
int texto_comparar(const char *a, const char *b);

#endif
