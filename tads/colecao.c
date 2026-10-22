#include "colecao.h"
#include "estruturas.h"
#include "../consultas/consulta.h"

#include <string.h>

/* Guarda apenas a referencia para o TAD escolhido, sem tabela de funcoes. */
struct colecao {
  TIPO_COLECAO tipo;
  size_t tamanho;
  PILHA *pilha;
  FILA *fila;
  LISTA_SEQ *lista_seq;
  LISTA_ENC *lista_enc;
  LISTA_CABECA *lista_cabeca;
  LISTA_ORD *lista_ord;
  LISTA_GEN *lista_gen;
  LISTA_CRUZ *lista_cruz;
};

bool colecao_tipo(const char *nome, TIPO_COLECAO *tipo) {
  const char *nomes[] = {
    "pilha", "fila", "lista_seq", "lista_enc", "lista_cabeca",
    "lista_ord", "lista_gen", "lista_cruz"
  };
  size_t i;

  if (nome == NULL || tipo == NULL) {
    return false;
  }
  for (i = 0; i < 8; i++) {
    if (strcmp(nome, nomes[i]) == 0) {
      *tipo = (TIPO_COLECAO)i;
      return true;
    }
  }
  return false;
}

COLECAO *colecao_criar(TIPO_COLECAO tipo, const struct consulta *consulta) {
  COLECAO *colecao;
  bool criada = false;

  if (!consulta_estrutura(consulta, tipo)) {
    return NULL;
  }
  colecao = calloc(1, sizeof(COLECAO));
  if (colecao == NULL) {
    return NULL;
  }
  colecao->tipo = tipo;
  switch (tipo) {
    case COLECAO_PILHA:
      colecao->pilha = pilha_criar();
      criada = colecao->pilha != NULL;
      break;
    case COLECAO_FILA:
      colecao->fila = fila_criar();
      criada = colecao->fila != NULL;
      break;
    case COLECAO_LISTA_SEQ:
      colecao->lista_seq = lista_seq_criar();
      criada = colecao->lista_seq != NULL;
      break;
    case COLECAO_LISTA_ENC:
      colecao->lista_enc = lista_enc_criar();
      criada = colecao->lista_enc != NULL;
      break;
    case COLECAO_LISTA_CABECA:
      colecao->lista_cabeca = lista_cabeca_criar();
      criada = colecao->lista_cabeca != NULL;
      break;
    case COLECAO_LISTA_ORD:
      colecao->lista_ord = lista_ord_criar(consulta);
      criada = colecao->lista_ord != NULL;
      break;
    case COLECAO_LISTA_GEN:
      colecao->lista_gen = lista_gen_criar(consulta);
      criada = colecao->lista_gen != NULL;
      break;
    case COLECAO_LISTA_CRUZ:
      colecao->lista_cruz = lista_cruz_criar(consulta);
      criada = colecao->lista_cruz != NULL;
      break;
  }
  if (!criada) {
    free(colecao);
    colecao = NULL;
  }
  return colecao;
}

bool colecao_inserir(COLECAO *colecao, REGISTRO *registro) {
  bool inserido = false;

  if (colecao == NULL || registro == NULL || colecao->tamanho == (size_t)-1) {
    return false;
  }
  switch (colecao->tipo) {
    case COLECAO_PILHA:
      inserido = pilha_inserir(colecao->pilha, registro);
      break;
    case COLECAO_FILA:
      inserido = fila_inserir(colecao->fila, registro);
      break;
    case COLECAO_LISTA_SEQ:
      inserido = lista_seq_inserir(colecao->lista_seq, registro);
      break;
    case COLECAO_LISTA_ENC:
      inserido = lista_enc_inserir(colecao->lista_enc, registro);
      break;
    case COLECAO_LISTA_CABECA:
      inserido = lista_cabeca_inserir(colecao->lista_cabeca, registro);
      break;
    case COLECAO_LISTA_ORD:
      inserido = lista_ord_inserir(colecao->lista_ord, registro);
      break;
    case COLECAO_LISTA_GEN:
      inserido = lista_gen_inserir(colecao->lista_gen, registro);
      break;
    case COLECAO_LISTA_CRUZ:
      inserido = lista_cruz_inserir(colecao->lista_cruz, registro);
      break;
  }
  if (inserido) {
    colecao->tamanho++;
  }
  return inserido;
}

REGISTRO *colecao_retirar(COLECAO *colecao) {
  REGISTRO *registro = NULL;

  if (colecao == NULL || colecao->tamanho == 0) {
    return NULL;
  }
  switch (colecao->tipo) {
    case COLECAO_PILHA:
      registro = pilha_retirar(colecao->pilha);
      break;
    case COLECAO_FILA:
      registro = fila_retirar(colecao->fila);
      break;
    case COLECAO_LISTA_SEQ:
      registro = lista_seq_retirar(colecao->lista_seq);
      break;
    case COLECAO_LISTA_ENC:
      registro = lista_enc_retirar(colecao->lista_enc);
      break;
    case COLECAO_LISTA_CABECA:
      registro = lista_cabeca_retirar(colecao->lista_cabeca);
      break;
    case COLECAO_LISTA_ORD:
      registro = lista_ord_retirar(colecao->lista_ord);
      break;
    case COLECAO_LISTA_GEN:
      registro = lista_gen_retirar(colecao->lista_gen);
      break;
    case COLECAO_LISTA_CRUZ:
      registro = lista_cruz_retirar(colecao->lista_cruz);
      break;
  }
  if (registro != NULL) {
    colecao->tamanho--;
  }
  return registro;
}

size_t colecao_tamanho(const COLECAO *colecao) {
  return colecao != NULL ? colecao->tamanho : 0;
}

bool colecao_reiniciar(COLECAO *colecao, unsigned int eixo) {
  if (colecao == NULL || eixo > 1 ||
      (eixo == 1 && colecao->tipo != COLECAO_LISTA_CRUZ)) {
    return false;
  }
  switch (colecao->tipo) {
    case COLECAO_PILHA:
      pilha_reiniciar(colecao->pilha);
      break;
    case COLECAO_FILA:
      fila_reiniciar(colecao->fila);
      break;
    case COLECAO_LISTA_SEQ:
      lista_seq_reiniciar(colecao->lista_seq);
      break;
    case COLECAO_LISTA_ENC:
      lista_enc_reiniciar(colecao->lista_enc);
      break;
    case COLECAO_LISTA_CABECA:
      lista_cabeca_reiniciar(colecao->lista_cabeca);
      break;
    case COLECAO_LISTA_ORD:
      lista_ord_reiniciar(colecao->lista_ord);
      break;
    case COLECAO_LISTA_GEN:
      lista_gen_reiniciar(colecao->lista_gen);
      break;
    case COLECAO_LISTA_CRUZ:
      lista_cruz_reiniciar(colecao->lista_cruz, eixo);
      break;
  }
  return true;
}

REGISTRO *colecao_proximo(COLECAO *colecao) {
  if (colecao == NULL) {
    return NULL;
  }
  switch (colecao->tipo) {
    case COLECAO_PILHA:
      return pilha_proximo(colecao->pilha);
    case COLECAO_FILA:
      return fila_proximo(colecao->fila);
    case COLECAO_LISTA_SEQ:
      return lista_seq_proximo(colecao->lista_seq);
    case COLECAO_LISTA_ENC:
      return lista_enc_proximo(colecao->lista_enc);
    case COLECAO_LISTA_CABECA:
      return lista_cabeca_proximo(colecao->lista_cabeca);
    case COLECAO_LISTA_ORD:
      return lista_ord_proximo(colecao->lista_ord);
    case COLECAO_LISTA_GEN:
      return lista_gen_proximo(colecao->lista_gen);
    case COLECAO_LISTA_CRUZ:
      return lista_cruz_proximo(colecao->lista_cruz);
  }
  return NULL;
}

void colecao_apagar(COLECAO **colecao, bool liberar_registros) {
  if (colecao == NULL || *colecao == NULL) {
    return;
  }
  switch ((*colecao)->tipo) {
    case COLECAO_PILHA:
      pilha_apagar(&(*colecao)->pilha, liberar_registros);
      break;
    case COLECAO_FILA:
      fila_apagar(&(*colecao)->fila, liberar_registros);
      break;
    case COLECAO_LISTA_SEQ:
      lista_seq_apagar(&(*colecao)->lista_seq, liberar_registros);
      break;
    case COLECAO_LISTA_ENC:
      lista_enc_apagar(&(*colecao)->lista_enc, liberar_registros);
      break;
    case COLECAO_LISTA_CABECA:
      lista_cabeca_apagar(&(*colecao)->lista_cabeca, liberar_registros);
      break;
    case COLECAO_LISTA_ORD:
      lista_ord_apagar(&(*colecao)->lista_ord, liberar_registros);
      break;
    case COLECAO_LISTA_GEN:
      lista_gen_apagar(&(*colecao)->lista_gen, liberar_registros);
      break;
    case COLECAO_LISTA_CRUZ:
      lista_cruz_apagar(&(*colecao)->lista_cruz, liberar_registros);
      break;
  }
  free(*colecao);
  *colecao = NULL;
}
