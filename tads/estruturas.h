#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include "../dados/registro.h"

struct consulta;
/* As representacoes e os nos de cada TAD ficam apenas no respectivo .c. */

typedef struct pilha PILHA;
PILHA *pilha_criar(void);
bool pilha_inserir(PILHA *estrutura, REGISTRO *registro);
REGISTRO *pilha_retirar(PILHA *estrutura);
void pilha_reiniciar(PILHA *estrutura);
REGISTRO *pilha_proximo(PILHA *estrutura);
void pilha_apagar(PILHA **estrutura, bool liberar_registros);

typedef struct fila FILA;
FILA *fila_criar(void);
bool fila_inserir(FILA *estrutura, REGISTRO *registro);
REGISTRO *fila_retirar(FILA *estrutura);
void fila_reiniciar(FILA *estrutura);
REGISTRO *fila_proximo(FILA *estrutura);
void fila_apagar(FILA **estrutura, bool liberar_registros);

typedef struct lista_seq LISTA_SEQ;
LISTA_SEQ *lista_seq_criar(void);
bool lista_seq_inserir(LISTA_SEQ *estrutura, REGISTRO *registro);
REGISTRO *lista_seq_retirar(LISTA_SEQ *estrutura);
void lista_seq_reiniciar(LISTA_SEQ *estrutura);
REGISTRO *lista_seq_proximo(LISTA_SEQ *estrutura);
void lista_seq_apagar(LISTA_SEQ **estrutura, bool liberar_registros);

typedef struct lista_enc LISTA_ENC;
LISTA_ENC *lista_enc_criar(void);
bool lista_enc_inserir(LISTA_ENC *estrutura, REGISTRO *registro);
REGISTRO *lista_enc_retirar(LISTA_ENC *estrutura);
void lista_enc_reiniciar(LISTA_ENC *estrutura);
REGISTRO *lista_enc_proximo(LISTA_ENC *estrutura);
void lista_enc_apagar(LISTA_ENC **estrutura, bool liberar_registros);

typedef struct lista_cabeca LISTA_CABECA;
LISTA_CABECA *lista_cabeca_criar(void);
bool lista_cabeca_inserir(LISTA_CABECA *estrutura, REGISTRO *registro);
REGISTRO *lista_cabeca_retirar(LISTA_CABECA *estrutura);
void lista_cabeca_reiniciar(LISTA_CABECA *estrutura);
REGISTRO *lista_cabeca_proximo(LISTA_CABECA *estrutura);
void lista_cabeca_apagar(LISTA_CABECA **estrutura, bool liberar_registros);

typedef struct lista_ord LISTA_ORD;
LISTA_ORD *lista_ord_criar(const struct consulta *consulta);
bool lista_ord_inserir(LISTA_ORD *estrutura, REGISTRO *registro);
REGISTRO *lista_ord_retirar(LISTA_ORD *estrutura);
void lista_ord_reiniciar(LISTA_ORD *estrutura);
REGISTRO *lista_ord_proximo(LISTA_ORD *estrutura);
void lista_ord_apagar(LISTA_ORD **estrutura, bool liberar_registros);

typedef struct lista_gen LISTA_GEN;
LISTA_GEN *lista_gen_criar(const struct consulta *consulta);
bool lista_gen_inserir(LISTA_GEN *estrutura, REGISTRO *registro);
REGISTRO *lista_gen_retirar(LISTA_GEN *estrutura);
void lista_gen_reiniciar(LISTA_GEN *estrutura);
REGISTRO *lista_gen_proximo(LISTA_GEN *estrutura);
void lista_gen_apagar(LISTA_GEN **estrutura, bool liberar_registros);

typedef struct lista_cruz LISTA_CRUZ;
LISTA_CRUZ *lista_cruz_criar(const struct consulta *consulta);
bool lista_cruz_inserir(LISTA_CRUZ *estrutura, REGISTRO *registro);
REGISTRO *lista_cruz_retirar(LISTA_CRUZ *estrutura);
void lista_cruz_reiniciar(LISTA_CRUZ *estrutura, unsigned int eixo);
REGISTRO *lista_cruz_proximo(LISTA_CRUZ *estrutura);
void lista_cruz_apagar(LISTA_CRUZ **estrutura, bool liberar_registros);

#endif
