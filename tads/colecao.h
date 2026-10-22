#ifndef COLECAO_H
#define COLECAO_H

#include "../dados/item.h"

struct consulta;
typedef struct colecao COLECAO;
typedef enum {
  COLECAO_PILHA, COLECAO_FILA, COLECAO_LISTA_SEQ, COLECAO_LISTA_ENC,
  COLECAO_LISTA_CABECA, COLECAO_LISTA_ORD, COLECAO_LISTA_GEN, COLECAO_LISTA_CRUZ
} TIPO_COLECAO;

bool colecao_tipo(const char *nome, TIPO_COLECAO *tipo);
COLECAO *colecao_criar(TIPO_COLECAO tipo, const struct consulta *consulta);
bool colecao_inserir(COLECAO *colecao, ITEM *item);
/* Retira o ponteiro sem destruir o ITEM; o chamador decide seu destino. */
ITEM *colecao_retirar(COLECAO *colecao);
size_t colecao_tamanho(const COLECAO *colecao);
/* Reinicie antes de percorrer. Somente lista_cruz aceita eixo 1.
 * Insercoes e retiradas reiniciam o percurso; nao as misture com uma visita em curso. */
bool colecao_reiniciar(COLECAO *colecao, unsigned int eixo);
/* Consulta sem retirar: o ITEM continua pertencendo a base. */
ITEM *colecao_proximo(COLECAO *colecao);
/* A base libera itens; o resultado libera apenas sua propria estrutura. */
void colecao_apagar(COLECAO **colecao, bool liberar_itens);

#endif
