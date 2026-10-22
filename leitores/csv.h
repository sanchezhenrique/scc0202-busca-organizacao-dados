#ifndef CSV_H
#define CSV_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Le um registro logico, que pode conter quebras de linha entre aspas.
 * O chamador libera a string; no EOF normal, retorna true com *linha == NULL.
 */
bool csv_ler_linha(FILE *arquivo, char **linha);
/* Aloca campos sem as aspas externas; a limpeza cabe a campos_apagar. */
bool csv_separar(const char *linha, char delimitador, char ***campos,
                 size_t *quantidade);
bool csv_linha_vazia(const char *linha);

/* Modo texto simples para consultas; modo CSV permite quebras entre aspas. */
bool arquivo_ler_linha(FILE *arquivo, char **linha, bool csv);

#endif
