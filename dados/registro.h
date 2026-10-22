#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdbool.h>
#include <stdlib.h>

/* Tipos opacos: os demais modulos acessam dados somente pelas funcoes abaixo. */
typedef struct registro REGISTRO;
typedef struct tabela TABELA;

/* Em caso de sucesso, as criacoes assumem as strings e o vetor de campos. */
TABELA *tabela_criar(char *cabecalho, char **campos, size_t quantidade,
                    char delimitador);
void tabela_apagar(TABELA **tabela);
size_t tabela_colunas(const TABELA *tabela);
/* Em caso de nome inexistente, retorna false sem fornecer um indice valido. */
bool tabela_indice(const TABELA *tabela, const char *nome, size_t *indice);
/* Textos retornados sao emprestados e continuam pertencendo a tabela. */
const char *tabela_campo(const TABELA *tabela, size_t indice);
const char *tabela_cabecalho(const TABELA *tabela);
char tabela_delimitador(const TABELA *tabela);

/* original e campos passam ao registro somente se a criacao retornar nao nulo. */
REGISTRO *registro_criar(char *original, char **campos, size_t quantidade,
                        size_t posicao);
void registro_apagar(REGISTRO **registro);
/* Textos emprestados: nao liberar. Campos finais omitidos sao lidos como "". */
const char *registro_campo(const REGISTRO *registro, size_t indice);
const char *registro_original(const REGISTRO *registro);
size_t registro_posicao(const REGISTRO *registro);
bool registro_numero(const REGISTRO *registro, size_t indice, double *numero);
/* Retorna false para ausentes, conversoes parciais, NaN ou infinito. */
bool texto_numero(const char *texto, double *numero);

/* Libera todas as strings e o vetor, inclusive durante a limpeza de uma falha. */
void campos_apagar(char ***campos, size_t quantidade);

char *texto_copiar(const char *texto);
bool texto_booleano(const char *texto, double *valor);
int texto_comparar(const char *a, const char *b);

#endif
