#include "../tads/colecao.h"
#include "../consultas/consulta.h"
#include "../leitores/csv.h"

#include <stdio.h>
#include <string.h>

static int falhas = 0;

static void conferir(bool condicao, const char *mensagem) {
  if (!condicao) {
    fprintf(stderr, "FALHA: %s\n", mensagem);
    falhas++;
  }
}

/* Registros pequenos de teste unitario, sem substituir o dataset de aplicacao. */
static REGISTRO *criar_linha(const char *linha, size_t posicao) {
  char *original = texto_copiar(linha);
  char **campos = NULL;
  size_t quantidade = 0;
  REGISTRO *registro = NULL;

  if (original != NULL && csv_separar(original, ',', &campos, &quantidade)) {
    registro = registro_criar(original, campos, quantidade, posicao);
  }
  if (registro == NULL) {
    campos_apagar(&campos, quantidade);
    free(original);
  }
  return registro;
}

static REGISTRO *criar_registro(size_t posicao) {
  const char *linhas[] = {"0,30,B,z", "1,10,A,y", "2,20,B,x",
                          "3,10,B,z", "4,40,A,y", "5,5,A,x"};

  return criar_linha(linhas[posicao % 6], posicao);
}

static void verificar_percurso(COLECAO *colecao, const int *esperados,
                               size_t quantidade, unsigned int eixo) {
  REGISTRO *registro;
  size_t i = 0;

  conferir(colecao_reiniciar(colecao, eixo), "reinicio do percurso");
  while ((registro = colecao_proximo(colecao)) != NULL) {
    conferir(i < quantidade, "itens excedentes no percurso");
    if (i < quantidade) {
      conferir(registro_posicao(registro) == (size_t)esperados[i], "ordem do percurso");
    }
    i++;
  }
  conferir(i == quantidade, "quantidade visitada");
}

static void testar_estrutura(TIPO_COLECAO tipo, const CONSULTA *consulta) {
  const int sequencia[] = {0, 1, 2, 3, 4, 5};
  const int pilha[] = {5, 4, 3, 2, 1, 0};
  const int ordenada[] = {5, 1, 3, 2, 0, 4};
  const int grupos[] = {0, 3, 2, 1, 4, 5};
  const int *esperados = sequencia;
  COLECAO *colecao = colecao_criar(tipo, consulta);
  REGISTRO *registro;
  REGISTRO *anterior;
  size_t i;
  size_t j;

  conferir(colecao != NULL, "criacao do TAD");
  conferir(colecao_retirar(colecao) == NULL, "retirada de estrutura vazia");
  conferir(!colecao_inserir(colecao, NULL), "insercao nula");
  for (i = 0; i < 6; i++) {
    registro = criar_registro(i);
    if (!colecao_inserir(colecao, registro)) {
      conferir(false, "insercao de registro");
      registro_apagar(&registro);
    }
    if (tipo == COLECAO_LISTA_ORD || tipo == COLECAO_LISTA_CRUZ) {
      /* A ordem precisa existir apos cada insercao, nao somente no final. */
      colecao_reiniciar(colecao, 0);
      anterior = colecao_proximo(colecao);
      while ((registro = colecao_proximo(colecao)) != NULL) {
        conferir(consulta_comparar(consulta, anterior, registro) <= 0, "invariante de ordem");
        anterior = registro;
      }
    }
  }
  if (tipo == COLECAO_PILHA) {
    esperados = pilha;
  } else if (tipo == COLECAO_LISTA_ORD || tipo == COLECAO_LISTA_CRUZ) {
    esperados = ordenada;
  } else if (tipo == COLECAO_LISTA_GEN) {
    esperados = grupos;
  }
  verificar_percurso(colecao, esperados, 6, 0);
  for (i = 0; i < 6; i++) {
    if (tipo == COLECAO_LISTA_CRUZ) {
      int restantes[6];
      size_t quantidade = 0;

      /* O segundo eixo precisa continuar valido depois de cada retirada. */
      for (j = 0; j < 6; j++) {
        size_t k;
        for (k = i; k < 6; k++) {
          if ((int)j == esperados[k]) {
            restantes[quantidade++] = (int)j;
          }
        }
      }
      verificar_percurso(colecao, restantes, quantidade, 1);
    }
    registro = colecao_retirar(colecao);
    conferir(registro != NULL && registro_posicao(registro) == (size_t)esperados[i], "retirada");
    registro_apagar(&registro);
    conferir(colecao_tamanho(colecao) == 5 - i, "tamanho apos retirada");
  }
  registro = criar_registro(0);
  conferir(colecao_inserir(colecao, registro), "reutilizacao apos esvaziar");
  verificar_percurso(colecao, sequencia, 1, 0);
  colecao_apagar(&colecao, true);
  conferir(colecao == NULL, "destrutor deve zerar a referencia");
  colecao_apagar(&colecao, true);
}

static void testar_combinacoes(const CONSULTA *consulta) {
  TIPO_COLECAO a;
  TIPO_COLECAO b;
  COLECAO *base;
  COLECAO *resultado;
  REGISTRO *registro;
  size_t i;

  for (a = COLECAO_PILHA; a <= COLECAO_LISTA_CRUZ; a++) {
    for (b = COLECAO_PILHA; b <= COLECAO_LISTA_CRUZ; b++) {
      base = colecao_criar(a, consulta);
      resultado = colecao_criar(b, consulta);
      for (i = 0; i < 6; i++) {
        registro = criar_registro(i);
        conferir(colecao_inserir(base, registro), "insercao na base");
      }
      colecao_reiniciar(base, 0);
      while ((registro = colecao_proximo(base)) != NULL) {
        conferir(colecao_inserir(resultado, registro), "insercao no resultado");
      }
      conferir(colecao_tamanho(resultado) == 6, "seis referencias no resultado");
      colecao_apagar(&resultado, false);
      colecao_reiniciar(base, 0);
      for (i = 0; i < 6; i++) {
        registro = colecao_proximo(base);
        conferir(registro != NULL && registro_campo(registro, 0) != NULL,
                 "destruir o resultado deve preservar registros da base");
      }
      colecao_apagar(&base, true);
    }
  }
}

static void testar_crescimento_e_ausentes(const CONSULTA *consulta) {
  const char *linhas[] = {"0,0,,", "1,0,A,x", "2,0,,", "3,0,,x"};
  const int esperados[] = {0, 2, 3, 1};
  COLECAO *colecao = colecao_criar(COLECAO_LISTA_SEQ, NULL);
  REGISTRO *registro;
  size_t i;

  for (i = 0; i < 2048; i++) {
    registro = criar_registro(i);
    conferir(colecao_inserir(colecao, registro), "crescimento do vetor");
  }
  colecao_reiniciar(colecao, 0);
  for (i = 0; i < 2048; i++) {
    registro = colecao_proximo(colecao);
    conferir(registro != NULL && registro_posicao(registro) == i, "vetor ampliado");
  }
  colecao_apagar(&colecao, true);
  colecao = colecao_criar(COLECAO_LISTA_GEN, consulta);
  for (i = 0; i < 4; i++) {
    registro = criar_linha(linhas[i], i);
    conferir(colecao_inserir(colecao, registro), "agrupamento de valores ausentes");
  }
  verificar_percurso(colecao, esperados, 4, 0);
  colecao_apagar(&colecao, true);
}

int main(void) {
  char *cabecalho = texto_copiar("id,valor,grupo,subgrupo\n");
  char **campos = NULL;
  size_t quantidade = 0;
  TABELA *tabela = NULL;
  CONSULTA *consulta = NULL;
  FILE *arquivo = fopen("consulta_tads.tmp", "wb");
  char erro[256];
  TIPO_COLECAO tipo;

  conferir(csv_separar(cabecalho, ',', &campos, &quantidade), "cabecalho de teste");
  tabela = tabela_criar(cabecalho, campos, quantidade, ',');
  conferir(arquivo != NULL && tabela != NULL, "preparacao de teste");
  if (arquivo != NULL && tabela != NULL) {
    fputs("ORDER|valor|ASC\nGROUP|grupo|subgrupo\nCROSS|valor|id\n", arquivo);
    fclose(arquivo);
    consulta = consulta_ler("consulta_tads.tmp", tabela, erro, sizeof(erro));
    conferir(consulta != NULL, "consulta dos TADs");
    if (consulta != NULL) {
      for (tipo = COLECAO_PILHA; tipo <= COLECAO_LISTA_CRUZ; tipo++) {
        testar_estrutura(tipo, consulta);
      }
      testar_combinacoes(consulta);
      testar_crescimento_e_ausentes(consulta);
    }
  }
  consulta_apagar(&consulta);
  tabela_apagar(&tabela);
  remove("consulta_tads.tmp");
  puts("TADs: oito estruturas, dois eixos e 64 combinacoes verificados.");
  return falhas == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
