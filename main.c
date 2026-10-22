#include "programa/opcoes.h"
#include "programa/metricas.h"
#include "leitores/carga.h"
#include "consultas/consulta.h"
#include "busca/busca.h"
#include "exportacao/exportacao.h"

#include <stdio.h>

/* O cliente conhece as operacoes dos TADs, mas nao seus campos internos. */
static bool executar(const OPCOES *opcoes, METRICAS *metricas) {
  LEITOR *leitor = leitor_abrir(opcoes->leitor, opcoes->entrada);
  CONSULTA *consulta = NULL;
  COLECAO *base = NULL;
  COLECAO *resultado = NULL;
  bool sucesso = false;

  if (leitor != NULL) {
    consulta = consulta_ler(opcoes->consulta, leitor_tabela(leitor));
    if (consulta != NULL) {
      base = colecao_criar(opcoes->base, consulta);
      resultado = colecao_criar(opcoes->resultado, consulta);
      if (base != NULL && resultado != NULL) {
        sucesso = carga_executar(leitor, base, opcoes->limite, metricas) &&
                  busca_executar(base, resultado, consulta, metricas) &&
                  exportacao_gravar(opcoes->saida, leitor_tabela(leitor), resultado,
                                    opcoes->top_k, metricas);
      } else {
        printf("ERRO=Confira GROUP, CROSS e a memoria disponivel para as estruturas.\n");
      }
    }
  }
  /* O resultado empresta os itens: somente a base pode libera-los. */
  colecao_apagar(&resultado, false);
  colecao_apagar(&base, true);
  consulta_apagar(&consulta);
  leitor_apagar(&leitor);
  return sucesso;
}

int main(int argc, char **argv) {
  OPCOES opcoes;
  METRICAS metricas = {0};
  bool sucesso = opcoes_ler(argc, argv, &opcoes);

  /* Cada etapa imprime sua propria falha; as metricas aparecem em ambos os casos. */
  if (sucesso) {
    sucesso = executar(&opcoes, &metricas);
  } else {
    opcoes_imprimir_uso();
  }
  metricas_imprimir(&metricas, sucesso);
  return sucesso ? EXIT_SUCCESS : EXIT_FAILURE;
}
