#include "consulta.h"
#include "../leitores/csv.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

/* Cada no representa FILTER ou SIM, na ordem em que aparece no arquivo. */
typedef struct criterio {
  bool similaridade;
  size_t campo;
  int operador;
  char *valor;
  double alvo;
  double tolerancia;
  double peso;
  struct criterio *proximo;
} CRITERIO;

struct consulta {
  CRITERIO *inicio;
  CRITERIO *fim;
  double limiar;
  double maior_peso;
  bool tem_accept;
  bool tem_ordem;
  bool por_similaridade;
  bool descendente;
  size_t ordem;
  bool tem_grupo;
  bool tem_cruzamento;
  size_t grupos[2];
  size_t cruzamentos[2];
};

static bool adicionar_criterio(CONSULTA *consulta, char **partes, const TABELA *tabela) {
  const char *operadores[] = {"==", "!=", "<", "<=", ">", ">="};
  CRITERIO *criterio = calloc(1, sizeof(CRITERIO));
  bool valido;
  int i;

  if (criterio == NULL) {
    return false;
  }
  criterio->similaridade = strcmp(partes[0], "SIM") == 0;
  valido = tabela_indice(tabela, partes[1], &criterio->campo);
  if (criterio->similaridade) {
    valido = valido && texto_numero(partes[2], &criterio->alvo) &&
             texto_numero(partes[3], &criterio->tolerancia) && criterio->tolerancia > 0 &&
             texto_numero(partes[4], &criterio->peso) && criterio->peso >= 0;
  } else {
    criterio->operador = -1;
    for (i = 0; i < 6; i++) {
      if (strcmp(partes[2], operadores[i]) == 0) {
        criterio->operador = i;
      }
    }
    criterio->valor = texto_copiar(partes[3]);
    valido = valido && criterio->operador >= 0 && criterio->valor != NULL;
  }
  if (!valido) {
    free(criterio->valor);
    free(criterio);
    return false;
  }
  if (consulta->fim == NULL) {
    consulta->inicio = criterio;
  } else {
    consulta->fim->proximo = criterio;
  }
  consulta->fim = criterio;
  consulta->maior_peso = fmax(consulta->maior_peso, criterio->peso);
  return true;
}

static bool interpretar(CONSULTA *consulta, char *linha, const TABELA *tabela) {
  char *partes[5] = {linha};
  size_t quantidade = 1;
  char *p;
  size_t *campos;
  bool *definido;

  /* A separacao manual conserva o alvo vazio em FILTER|campo|==|. */
  for (p = linha; *p != '\0'; p++) {
    if (*p == '|') {
      if (quantidade == 5) {
        return false;
      }
      *p = '\0';
      partes[quantidade++] = p + 1;
    }
  }
  if ((strcmp(partes[0], "FILTER") == 0 && quantidade == 4) ||
      (strcmp(partes[0], "SIM") == 0 && quantidade == 5)) {
    return adicionar_criterio(consulta, partes, tabela);
  }
  if (strcmp(partes[0], "ACCEPT") == 0 && quantidade == 2) {
    if (consulta->tem_accept || !texto_numero(partes[1], &consulta->limiar) ||
        consulta->limiar < 0 || consulta->limiar > 1) {
      return false;
    }
    consulta->tem_accept = true;
    return true;
  }
  if (quantidade != 3) {
    return false;
  }
  if (strcmp(partes[0], "ORDER") == 0) {
    consulta->por_similaridade = strcmp(partes[1], "similarity") == 0;
    if (consulta->tem_ordem ||
        (!consulta->por_similaridade && !tabela_indice(tabela, partes[1], &consulta->ordem)) ||
        (strcmp(partes[2], "ASC") != 0 && strcmp(partes[2], "DESC") != 0)) {
      return false;
    }
    consulta->tem_ordem = true;
    consulta->descendente = strcmp(partes[2], "DESC") == 0;
    return true;
  }
  if (strcmp(partes[0], "GROUP") == 0) {
    campos = consulta->grupos;
    definido = &consulta->tem_grupo;
  } else if (strcmp(partes[0], "CROSS") == 0) {
    campos = consulta->cruzamentos;
    definido = &consulta->tem_cruzamento;
  } else {
    return false;
  }
  if (*definido || !tabela_indice(tabela, partes[1], &campos[0]) ||
      !tabela_indice(tabela, partes[2], &campos[1])) {
    return false;
  }
  *definido = true;
  return true;
}

CONSULTA *consulta_ler(const char *caminho, const TABELA *tabela) {
  FILE *arquivo = caminho != NULL ? fopen(caminho, "rb") : NULL;
  CONSULTA *consulta = calloc(1, sizeof(CONSULTA));
  char *linha = NULL;
  size_t numero = 0;
  bool valido = arquivo != NULL && consulta != NULL && tabela != NULL;
  CRITERIO *criterio;

  while (valido) {
    valido = arquivo_ler_linha(arquivo, &linha, false);
    if (!valido || linha == NULL) {
      break;
    }
    numero++;
    if (linha[0] != '\0' && linha[0] != '#') {
      valido = interpretar(consulta, linha, tabela);
    }
    free(linha);
    linha = NULL;
  }
  if (arquivo != NULL) {
    fclose(arquivo);
  }
  /* SIM com todos os pesos nulos nao possui uma media definida. */
  for (criterio = consulta != NULL ? consulta->inicio : NULL;
       criterio != NULL; criterio = criterio->proximo) {
    if (criterio->similaridade && consulta->maior_peso == 0) {
      valido = false;
    }
  }
  if (!valido) {
    printf("ERRO=Consulta, linha %zu: arquivo, comando ou parametro invalido.\n", numero);
    consulta_apagar(&consulta);
  }
  return consulta;
}

void consulta_apagar(CONSULTA **consulta) {
  CRITERIO *removido;

  if (consulta != NULL && *consulta != NULL) {
    while ((*consulta)->inicio != NULL) {
      removido = (*consulta)->inicio;
      (*consulta)->inicio = removido->proximo;
      free(removido->valor);
      free(removido);
    }
    free(*consulta);
    *consulta = NULL;
  }
}

double consulta_similaridade(const CONSULTA *consulta, const ITEM *item) {
  CRITERIO *criterio;
  double soma = 0;
  double pesos = 0;
  double valor;
  double nota;
  double peso;

  if (consulta == NULL || item == NULL) {
    return 0;
  }
  for (criterio = consulta->inicio; criterio != NULL; criterio = criterio->proximo) {
    if (criterio->similaridade) {
      nota = 0;
      if (item_numero(item, criterio->campo, &valor)) {
        nota = fmax(0, 1 - fabs(valor - criterio->alvo) / criterio->tolerancia);
      }
      /* Escalar os pesos evita transbordamento e conserva a mesma media. */
      peso = criterio->peso / consulta->maior_peso;
      soma += nota * peso;
      pesos += peso;
    }
  }
  return pesos > 0 ? soma / pesos : 1;
}

bool consulta_aceita(const CONSULTA *consulta, const ITEM *item) {
  CRITERIO *criterio;
  const char *campo;
  double numero;
  int comparacao;
  bool aceito;

  if (consulta == NULL || item == NULL) {
    return false;
  }
  for (criterio = consulta->inicio; criterio != NULL; criterio = criterio->proximo) {
    if (!criterio->similaridade) {
      campo = item_campo(item, criterio->campo);
      if (campo == NULL || (texto_numero(criterio->valor, &numero) && !texto_numero(campo, &numero)) ||
          (texto_booleano(criterio->valor, &numero) && !texto_booleano(campo, &numero))) {
        return false;
      }
      comparacao = texto_comparar(campo, criterio->valor);
      switch (criterio->operador) {
        case 0:
          aceito = comparacao == 0;
          break;
        case 1:
          aceito = comparacao != 0;
          break;
        case 2:
          aceito = comparacao < 0;
          break;
        case 3:
          aceito = comparacao <= 0;
          break;
        case 4:
          aceito = comparacao > 0;
          break;
        default:
          aceito = comparacao >= 0;
      }
      if (!aceito) {
        return false;
      }
    }
  }
  return consulta->maior_peso == 0 || consulta_similaridade(consulta, item) >= consulta->limiar;
}

bool consulta_estrutura(const CONSULTA *consulta, TIPO_COLECAO tipo) {
  return tipo >= COLECAO_PILHA && tipo <= COLECAO_LISTA_CRUZ &&
         (tipo != COLECAO_LISTA_GEN || (consulta != NULL && consulta->tem_grupo)) &&
         (tipo != COLECAO_LISTA_CRUZ || (consulta != NULL && consulta->tem_cruzamento));
}

int consulta_comparar(const CONSULTA *consulta, const ITEM *a, const ITEM *b) {
  double x;
  double y;
  int comparacao;

  if (consulta == NULL || !consulta->tem_ordem) {
    return (item_posicao(a) > item_posicao(b)) -
           (item_posicao(a) < item_posicao(b));
  }
  if (consulta->por_similaridade) {
    x = consulta_similaridade(consulta, a);
    y = consulta_similaridade(consulta, b);
    comparacao = (x > y) - (x < y);
  } else {
    comparacao = texto_comparar(item_campo(a, consulta->ordem), item_campo(b, consulta->ordem));
  }
  return consulta->descendente ? -comparacao : comparacao;
}

const char *consulta_grupo(const CONSULTA *consulta, const ITEM *item, unsigned int nivel) {
  if (consulta == NULL || nivel > 1) {
    return "";
  }
  return item_campo(item, consulta->grupos[nivel]);
}

int consulta_cruzar(const CONSULTA *consulta, const ITEM *a,
                    const ITEM *b, unsigned int eixo) {
  size_t campo;

  if (consulta == NULL || eixo > 1) {
    return 0;
  }
  campo = consulta->cruzamentos[eixo];
  return texto_comparar(item_campo(a, campo), item_campo(b, campo));
}
