CC = cc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2
CPPFLAGS = -D_POSIX_C_SOURCE=200809L
LDLIBS = -lm

FONTES = main.c programa/opcoes.c programa/metricas.c dados/registro.c \
         leitores/csv.c leitores/leitor.c leitores/datatran.c leitores/carga.c \
         consultas/consulta.c busca/busca.c exportacao/exportacao.c \
         tads/colecao.c tads/pilha.c tads/fila.c tads/lista_seq.c \
         tads/lista_enc.c tads/lista_cabeca.c tads/lista_ord.c \
         tads/lista_gen.c tads/lista_cruz.c
HEADERS = $(wildcard programa/*.h dados/*.h leitores/*.h consultas/*.h busca/*.h exportacao/*.h tads/*.h)

# Uma compilacao direta, sem gerar objetos ou arquivos de dependencia separados.
trabalho: $(FONTES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(FONTES) $(LDLIBS) -o trabalho

test: trabalho
	$(MAKE) -C testes
	python3 -B testes/teste_programa.py
	python3 -B testes/teste_datatran.py

clean:
	rm -f trabalho $(FONTES:.c=.o) $(FONTES:.c=.d) testes/teste_colecoes testes/teste_csv

.PHONY: test clean
