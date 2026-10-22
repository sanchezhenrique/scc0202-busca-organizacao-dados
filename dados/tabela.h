#ifndef TABELA_H
#define TABELA_H

#include <stdbool.h>
#include <stdlib.h>

/* Metadados do CSV; a representacao fica somente em tabela.c. */
typedef struct tabela TABELA;

/* No sucesso, assume o cabecalho, o vetor e seus nomes; na falha, nao os libera. */
TABELA *tabela_criar(char *cabecalho, char **campos, size_t quantidade,
                    char delimitador);
void tabela_apagar(TABELA **tabela);
size_t tabela_colunas(const TABELA *tabela);
/* Nome inexistente: retorna false sem fornecer um indice valido. */
bool tabela_indice(const TABELA *tabela, const char *nome, size_t *indice);
/* Textos emprestados: pertencem a tabela e nao devem ser liberados. */
const char *tabela_campo(const TABELA *tabela, size_t indice);
const char *tabela_cabecalho(const TABELA *tabela);
char tabela_delimitador(const TABELA *tabela);

#endif
