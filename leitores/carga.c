#include "carga.h"

#include <stdio.h>

bool carga_executar(LEITOR *leitor, COLECAO *base, size_t limite,
                    METRICAS *metricas) {
  double inicio;
  double fim;
  bool sucesso = true;

  if (leitor == NULL || base == NULL || metricas == NULL ||
      !metricas_agora(&inicio)) {
    printf("ERRO=Carga invalida ou relogio indisponivel.\n");
    return false;
  }
  /* Verifica o limite antes de ler para nao criar um item alem do solicitado. */
  while (limite == 0 || metricas->registros_entrada < limite) {
    ITEM *item = NULL;

    if (!leitor_proximo(leitor, &item)) {
      sucesso = false;
      break;
    }
    if (item == NULL) {
      break;
    }
    if (!colecao_inserir(base, item)) {
      /* A base assume o item somente depois da insercao bem-sucedida. */
      item_apagar(&item);
      printf("ERRO=Sem memoria para inserir um item na base.\n");
      sucesso = false;
      break;
    }
    metricas->registros_entrada++;
  }
  if (!metricas_agora(&fim)) {
    printf("ERRO=Relogio indisponivel ao concluir a carga.\n");
    return false;
  }
  /* A carga inclui leitura, criacao dos itens e insercao na estrutura da base. */
  metricas->carga_ms = fim - inicio;
  return sucesso;
}
