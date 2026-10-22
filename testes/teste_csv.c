#include "../leitores/csv.h"
#include "../dados/registro.h"

#include <stdio.h>
#include <string.h>

static int falhas = 0;

static void conferir(bool condicao, const char *mensagem) {
  if (!condicao) {
    fprintf(stderr, "FALHA CSV: %s\n", mensagem);
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
           "leitura de registro com quebra interna");
  if (linha != NULL) {
    conferir(strcmp(linha, primeira) == 0, "preservacao dos bytes originais");
  }
  free(linha);
  linha = NULL;
  conferir(csv_ler_linha(arquivo, &linha) && linha != NULL,
           "ultimo registro sem quebra final");
  if (linha != NULL) {
    conferir(strcmp(linha, "4,5,6") == 0, "ultimo registro completo");
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
  if (falhas != 0) {
    return EXIT_FAILURE;
  }
  puts("CSV: aspas, delimitadores, campos vazios, EOF e numeros verificados.");
  return EXIT_SUCCESS;
}
