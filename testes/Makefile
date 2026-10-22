CC = cc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
FONTES = ../tads/colecao.c ../tads/pilha.c ../tads/fila.c \
         ../tads/lista_seq.c ../tads/lista_enc.c ../tads/lista_cabeca.c \
         ../tads/lista_ord.c ../tads/lista_gen.c ../tads/lista_cruz.c \
         ../dados/registro.c ../consultas/consulta.c ../leitores/csv.c
HEADERS = $(wildcard ../tads/*.h ../dados/*.h ../consultas/*.h ../leitores/*.h)

verificar: teste_colecoes teste_csv
	./teste_colecoes
	./teste_csv

teste_colecoes: teste_colecoes.c $(FONTES) $(HEADERS)
	$(CC) $(CFLAGS) teste_colecoes.c $(FONTES) -lm -o $@

teste_csv: teste_csv.c ../leitores/csv.c ../dados/registro.c $(HEADERS)
	$(CC) $(CFLAGS) teste_csv.c ../leitores/csv.c ../dados/registro.c -lm -o $@

.PHONY: verificar
