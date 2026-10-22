#ifndef OPCOES_H
#define OPCOES_H

#include "../tads/colecao.h"

/* Os textos apontam para argv: nao sao copiados nem devem ser liberados aqui. */
typedef struct {
  const char *leitor;
  const char *entrada;
  const char *consulta;
  const char *saida;
  TIPO_COLECAO base;
  TIPO_COLECAO resultado;
  size_t top_k;
  size_t limite;
} OPCOES;

/* Top-k e limite assumem zero; as seis opcoes de selecao/caminhos sao obrigatorias.
 * Uma opcao invalida produz mensagem ERRO com printf e retorno false. */
bool opcoes_ler(int argc, char **argv, OPCOES *opcoes);
void opcoes_imprimir_uso(void);

#endif
