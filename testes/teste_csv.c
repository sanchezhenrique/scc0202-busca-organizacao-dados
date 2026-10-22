#include "../leitores/csv.h"
#include "../dados/item.h"

#include <stdio.h>
#include <string.h>

static int falhas = 0;

static void conferir(bool condicao, const char *mensagem) {
  if (!condicao) {
    printf("FALHA CSV: %s\n", mensagem);
    falhas++;
  }
}

static void verificar_campos(const char *linha, char delimitador,
                             const char *const *esperados, size_t total) {
  char **campos = NULL;
  size_t quantidade = 0;
  size_t i;

  conferir(csv_separar(linha, delimitador, &campos, &quantidade), "separacao valida");
  conferir(quantidade == total, "numero de campos");
  for (i = 0; i < quantidade && i < total; i++) {
    conferir(strcmp(campos[i], esperados[i]) == 0, "conteudo do campo");
  }
  campos_apagar(&campos, quantidade);
}

/* Confere o contrato do ITEM: texto original, campos adaptados e propriedade. */
static void testar_item(void) {
  const char *linha = "42,MG,\"146,1\",true,\r\n";
  char *original = texto_copiar(linha);
  char **campos = NULL;
  size_t quantidade = 0;
  ITEM *item = NULL;
  double numero = 99;

  conferir(item_criar(NULL, NULL, 0, 0) == NULL, "criacao invalida de item");
  if (original == NULL || !csv_separar(original, ',', &campos, &quantidade)) {
    conferir(false, "preparacao do item");
    free(original);
    return;
  }
  conferir(item_criar(original, campos, 0, 7) == NULL, "item sem campos recusado");
  /* A criacao recusada nao deve assumir nem liberar os argumentos recebidos. */
  conferir(strcmp(original, linha) == 0 && quantidade == 5,
           "argumentos preservados na falha");
  if (quantidade == 5 && strcmp(campos[2], "146,1") == 0) {
    campos[2][3] = '.';
    item = item_criar(original, campos, quantidade, 7);
  }
  conferir(item != NULL, "criacao do item CSV");
  if (item == NULL) {
    campos_apagar(&campos, quantidade);
    free(original);
    return;
  }
  conferir(item_original(item) == original && item_campo(item, 2) == campos[2],
           "item assume os ponteiros sem duplicar os dados");
  conferir(strcmp(item_original(item), linha) == 0,
           "original preservado apesar da adaptacao do campo");
  conferir(item_numero(item, 2, &numero) && numero == 146.1,
           "consulta numerica usa o campo adaptado");
  conferir(item_posicao(item) == 7, "posicao de leitura do item");
  conferir(strcmp(item_campo(item, 4), "") == 0 &&
           strcmp(item_campo(item, 99), "") == 0, "campos vazios e omitidos");
  numero = 99;
  conferir(!item_numero(item, 1, &numero) && numero == 99,
           "falha numerica nao altera a saida");
  conferir(!item_numero(item, 99, &numero), "campo omitido nao vira zero");
  item_apagar(&item);
  conferir(item == NULL, "destrutor do item zera o ponteiro");
  item_apagar(&item);
  item_apagar(NULL);
  conferir(item_campo(NULL, 0) == NULL && item_original(NULL) == NULL &&
           !item_numero(NULL, 0, &numero), "acessos a item nulo");
}

/* tmpfile testa EOF e quebras internas em arquivos que sao descartados no fechamento. */
static void testar_leitura(void) {
  const char *primeira = "1,\"duas\nlinhas\",3\r\n";
  FILE *arquivo = tmpfile();
  char *linha = NULL;

  conferir(arquivo != NULL, "arquivo temporario");
  if (arquivo == NULL) {
    return;
  }
  fputs(primeira, arquivo);
  fputs("4,5,6", arquivo);
  rewind(arquivo);
  conferir(csv_ler_linha(arquivo, &linha) && linha != NULL,
           "leitura de item com quebra interna");
  if (linha != NULL) {
    conferir(strcmp(linha, primeira) == 0, "preservacao dos bytes originais");
  }
  free(linha);
  linha = NULL;
  conferir(csv_ler_linha(arquivo, &linha) && linha != NULL,
           "ultimo item sem quebra final");
  if (linha != NULL) {
    conferir(strcmp(linha, "4,5,6") == 0, "ultimo item completo");
  }
  free(linha);
  linha = NULL;
  conferir(csv_ler_linha(arquivo, &linha) && linha == NULL, "fim sem item fantasma");
  fclose(arquivo);
  arquivo = tmpfile();
  conferir(arquivo != NULL, "arquivo de aspas incompletas");
  if (arquivo != NULL) {
    fputs("\"incompleto", arquivo);
    rewind(arquivo);
    conferir(!csv_ler_linha(arquivo, &linha) && linha == NULL,
             "aspas abertas no fim sao erro");
    fclose(arquivo);
  }
}

int main(void) {
  const char *const virgulas[] = {"a,b", "disse \"oi\"", "", "fim", ""};
  const char *const tabulacoes[] = {"a", "b\tc", ""};
  const char *const linhas[] = {"1", "duas\nlinhas", "3"};
  char **campos = NULL;
  size_t quantidade = 0;
  double numero;

  /* Strings unitarias de sintaxe; nao substituem o dataset fornecido. */
  verificar_campos("\"a,b\",\"disse \"\"oi\"\"\",,fim,\r\n", ',', virgulas, 5);
  verificar_campos("a\t\"b\tc\"\t", '\t', tabulacoes, 3);
  verificar_campos("1,\"duas\nlinhas\",3\r\n", ',', linhas, 3);
  conferir(!csv_separar("\"ab\"erro,c", ',', &campos, &quantidade),
           "texto apos fechamento de aspas deve ser recusado");
  conferir(!csv_separar("ab\"cd,c", ',', &campos, &quantidade),
           "aspas em campo sem citacao devem ser recusadas");
  conferir(texto_numero(" 1.25e2 \t", &numero) && numero == 125, "notacao cientifica");
  conferir(!texto_numero("", &numero), "numero ausente");
  conferir(!texto_numero("NaN", &numero), "NaN nao e numero valido");
  conferir(!texto_numero("inf", &numero), "infinito nao e numero valido");
  conferir(!texto_numero("1.5x", &numero), "sufixo invalido");
  testar_leitura();
  testar_item();
  if (falhas != 0) {
    return EXIT_FAILURE;
  }
  printf("CSV e ITEM: sintaxe, conversoes, linha original e propriedade verificados.\n");
  return EXIT_SUCCESS;
}
