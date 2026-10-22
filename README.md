# Projeto 1 — Busca e organização de dados

O mesmo executável atende às duas partes, com TADs ocultos, funções diretas, laços explícitos e comentários em português.

| Parte | `--reader` | Arquivo fornecido |
| --- | --- | --- |
| Asteroides | `nasa` | `asteroid_dataset_20251019.csv` |
| Acidentes | `datatran` | `datatran2026.csv` |

## Compilar e executar

O ambiente utilizado é Linux, com compilador C99 e `make`. A medição usa `clock_gettime` de `<time.h>`, conforme a autorização dada pelo usuário. Não há bibliotecas externas de execução.

```sh
make
./trabalho --reader nasa --input asteroid_dataset_20251019.csv \
  --base lista_seq --result lista_ord --query consultas/nasa.txt \
  --output resultado_nasa.csv --top-k 100 --limit 0
```

Para selecionar acidentes com mortes e organizar por estado e tipo:

```sh
./trabalho --reader datatran --input datatran2026.csv \
  --base lista_seq --result lista_gen --query consultas/datatran_graves.txt \
  --output resultado_graves.csv --top-k 100 --limit 0
```

Para buscar acidentes com feridos em MG por similaridade de quilômetro e quantidade de pessoas:

```sh
./trabalho --reader datatran --input datatran2026.csv \
  --base fila --result lista_ord --query consultas/datatran_proximidade.txt \
  --output resultado_proximidade.csv --top-k 100 --limit 0
```

`--reader`, `--input`, `--base`, `--result`, `--query` e `--output` são obrigatórios. `--top-k` e `--limit` são opcionais e assumem `0`, que significa todos os registros.

## Consultas e estruturas

As oito opções são `pilha`, `fila`, `lista_seq`, `lista_enc`, `lista_cabeca`, `lista_ord`, `lista_gen` e `lista_cruz`. Todas podem ser usadas como base ou resultado. Inclua `GROUP` para a generalizada e `CROSS` para a cruzada, inclusive quando usadas na base.

`ORDER` controla a lista ordenada. As demais conservam sua organização própria. A pilha começa pelo topo; a fila, pelo início; a generalizada, pelo primeiro grupo e subgrupo criados; e a cruzada, pelo primeiro campo de `CROSS` em ordem crescente.

Escreva as consultas em UTF-8 e os números com ponto decimal. O leitor DATATRAN adapta os campos de consulta de Latin-1 para UTF-8 e normaliza `km`, `latitude` e `longitude`. A saída conserva o formato original: ponto e vírgula, aspas, vírgulas decimais e Latin-1. Os datasets fornecidos não são modificados.

## TAD ITEM e representação dos dados

Cada linha de dados do CSV é representada por um `ITEM`, definido em `dados/item.h` e implementado em `dados/item.c`. Sua estrutura é opaca: leitores, consultas, busca, exportação e as oito estruturas utilizam `ITEM *` e suas operações públicas.

O item guarda a linha original para exportar, os campos separados para consultar, a quantidade de campos e a posição de leitura. O cabeçalho fica em um TAD separado, `TABELA`, em `dados/tabela.h` e `dados/tabela.c`, compartilhado entre os itens do arquivo pelo leitor.

- `item_criar` assume a linha e os campos alocados pelo leitor somente no sucesso.
- `item_campo`, `item_original` e `item_posicao` consultam o conteúdo sem expor a representação.
- `item_numero` tenta converter um campo em `double`; valores ausentes ou inválidos são recusados.
- `item_apagar` libera o conteúdo e zera o ponteiro do chamador.

A carga cria e insere cada item na base. O resultado guarda referências aos mesmos itens, sem copiar as linhas. No encerramento, o resultado libera sua estrutura e a base destrói os itens. Se a inserção falhar, a carga destrói o item que não foi inserido.

## Verificação e documentação

```sh
make test
```

Os testes verificam as 64 combinações de estruturas em cada dataset, filtros, similaridade, agrupamentos, limites, erros e indicadores. As exportações integrais devem ser idênticas aos arquivos originais. Os testes de integração usam Python 3 com sua biblioteca padrão; o programa C não depende de Python.

O [relatório do código](RELATORIO_CODIGO.md) explica cada módulo, as funções e os trechos principais, os percursos das oito estruturas, os dois leitores, as conversões do DATATRAN e a responsabilidade pela memória.
