#include "carga.h"

#include <stdio.h>

bool carga_executar(LEITOR *leitor, COLECAO *base, size_t limite,
                    METRICAS *metricas, char *erro, size_t capacidade) {
  double inicio;
  double fim;
  bool sucesso = true;

  if (leitor == NULL || base == NULL || metricas == NULL ||
      !metricas_agora(&inicio)) {
    if (erro != NULL && capacidade > 0) {
      snprintf(erro, capacidade, "Carga invalida ou relogio indisponivel.");
    }
    return false;
  }
  /* Verifica o limite antes de ler para nao criar um item alem do solicitado. */
  while (limite == 0 || metricas->registros_entrada < limite) {
    ITEM *item = NULL;

    if (!leitor_proximo(leitor, &item, erro, capacidade)) {
      sucesso = false;
      break;
    }
    if (item == NULL) {
      break;
    }
    if (!colecao_inserir(base, item)) {
      /* A base assume o item somente depois da insercao bem-sucedida. */
      item_apagar(&item);
      if (erro != NULL && capacidade > 0) {
        snprintf(erro, capacidade, "Sem memoria para inserir um item na base.");
      }
      sucesso = false;
      break;
    }
    metricas->registros_entrada++;
  }
  if (!metricas_agora(&fim)) {
    if (erro != NULL && capacidade > 0) {
      snprintf(erro, capacidade, "Relogio indisponivel ao concluir a carga.");
    }
    return false;
  }
  /* A carga inclui leitura, criacao dos itens e insercao na estrutura da base. */
  metricas->carga_ms = fim - inicio;
  return sucesso;
}
