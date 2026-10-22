#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include "../dados/item.h"

struct consulta;
/* As representacoes e os nos de cada TAD ficam apenas no respectivo .c. */

typedef struct pilha PILHA;
PILHA *pilha_criar(void);
bool pilha_inserir(PILHA *estrutura, ITEM *item);
ITEM *pilha_retirar(PILHA *estrutura);
void pilha_reiniciar(PILHA *estrutura);
ITEM *pilha_proximo(PILHA *estrutura);
void pilha_apagar(PILHA **estrutura, bool liberar_itens);

typedef struct fila FILA;
FILA *fila_criar(void);
bool fila_inserir(FILA *estrutura, ITEM *item);
ITEM *fila_retirar(FILA *estrutura);
void fila_reiniciar(FILA *estrutura);
ITEM *fila_proximo(FILA *estrutura);
void fila_apagar(FILA **estrutura, bool liberar_itens);

typedef struct lista_seq LISTA_SEQ;
LISTA_SEQ *lista_seq_criar(void);
bool lista_seq_inserir(LISTA_SEQ *estrutura, ITEM *item);
ITEM *lista_seq_retirar(LISTA_SEQ *estrutura);
void lista_seq_reiniciar(LISTA_SEQ *estrutura);
ITEM *lista_seq_proximo(LISTA_SEQ *estrutura);
void lista_seq_apagar(LISTA_SEQ **estrutura, bool liberar_itens);

typedef struct lista_enc LISTA_ENC;
LISTA_ENC *lista_enc_criar(void);
bool lista_enc_inserir(LISTA_ENC *estrutura, ITEM *item);
ITEM *lista_enc_retirar(LISTA_ENC *estrutura);
void lista_enc_reiniciar(LISTA_ENC *estrutura);
ITEM *lista_enc_proximo(LISTA_ENC *estrutura);
void lista_enc_apagar(LISTA_ENC **estrutura, bool liberar_itens);

typedef struct lista_cabeca LISTA_CABECA;
LISTA_CABECA *lista_cabeca_criar(void);
bool lista_cabeca_inserir(LISTA_CABECA *estrutura, ITEM *item);
ITEM *lista_cabeca_retirar(LISTA_CABECA *estrutura);
void lista_cabeca_reiniciar(LISTA_CABECA *estrutura);
ITEM *lista_cabeca_proximo(LISTA_CABECA *estrutura);
void lista_cabeca_apagar(LISTA_CABECA **estrutura, bool liberar_itens);

typedef struct lista_ord LISTA_ORD;
LISTA_ORD *lista_ord_criar(const struct consulta *consulta);
bool lista_ord_inserir(LISTA_ORD *estrutura, ITEM *item);
ITEM *lista_ord_retirar(LISTA_ORD *estrutura);
void lista_ord_reiniciar(LISTA_ORD *estrutura);
ITEM *lista_ord_proximo(LISTA_ORD *estrutura);
void lista_ord_apagar(LISTA_ORD **estrutura, bool liberar_itens);

typedef struct lista_gen LISTA_GEN;
LISTA_GEN *lista_gen_criar(const struct consulta *consulta);
bool lista_gen_inserir(LISTA_GEN *estrutura, ITEM *item);
ITEM *lista_gen_retirar(LISTA_GEN *estrutura);
void lista_gen_reiniciar(LISTA_GEN *estrutura);
ITEM *lista_gen_proximo(LISTA_GEN *estrutura);
void lista_gen_apagar(LISTA_GEN **estrutura, bool liberar_itens);

typedef struct lista_cruz LISTA_CRUZ;
LISTA_CRUZ *lista_cruz_criar(const struct consulta *consulta);
bool lista_cruz_inserir(LISTA_CRUZ *estrutura, ITEM *item);
ITEM *lista_cruz_retirar(LISTA_CRUZ *estrutura);
void lista_cruz_reiniciar(LISTA_CRUZ *estrutura, unsigned int eixo);
ITEM *lista_cruz_proximo(LISTA_CRUZ *estrutura);
void lista_cruz_apagar(LISTA_CRUZ **estrutura, bool liberar_itens);

#endif
