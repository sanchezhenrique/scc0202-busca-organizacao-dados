# Consulta independente NASA: resultado esperado

Este caso usa o arquivo `asteroids.csv` existente no projeto. A referência foi calculada diretamente do dataset por um procedimento independente em Python, com `csv.reader` e aritmética `Decimal` de 50 dígitos. O procedimento não executa nem importa código do programa C para escolher os itens ou obter suas contagens.

A seleção usa os primeiros 10.000 registros lógicos, sem contar o cabeçalho. Os valores esperados dependem dessa versão do dataset, dessa consulta e dos argumentos abaixo. Os arquivos de entrada e a consulta anterior `consultas/nasa.txt` são preservados.

## 1. O que a consulta pesquisa

Aproximações ocorridas em 2021, a uma distância de até 0,02 UA, com velocidade próxima de 15 km/s e `risk_score` próximo de 0,50. O critério de risco tem peso dois; o de velocidade, peso um. A média deve atingir pelo menos 0,70.

Conteúdo de `consultas/nasa_verificacao.txt`, desconsiderando os comentários:

```text
FILTER|year|==|2021
FILTER|distance_au|<=|0.02
SIM|velocity_km_s|15.0|10.0|1.0
SIM|risk_score|0.50|0.20|2.0
ACCEPT|0.70
ORDER|similarity|DESC
```

Não são necessários GROUP ou CROSS para a combinação utilizada neste teste: base encadeada e resultado ordenado.

## 2. Comando para testar

Na pasta raiz do projeto, execute:

```sh
make
./trabalho --reader nasa --input asteroids.csv \
  --base lista_enc --result lista_ord \
  --query consultas/nasa_verificacao.txt \
  --output resultado_verificacao.csv --top-k 100 --limit 10000
```

## 3. Contagens esperadas

```text
STATUS=OK
INPUT_RECORDS=10000
MATCHED_RECORDS=287
OUTPUT_RECORDS=100
```

Os quatro indicadores de tempo (`LOAD_MS`, `SEARCH_MS`, `RESULT_INSERT_MS` e `WRITE_MS`) devem ser calculados na execução. Não há valores fixos esperados para eles.

A redução dos candidatos deve ser:

| Etapa | Quantidade |
| --- | ---: |
| Registros carregados | 10000 |
| Ano igual a 2021 | 3845 |
| Ano igual a 2021 e distância até 0,02 UA | 878 |
| Filtros satisfeitos e similaridade de pelo menos 0,70 | 287 |
| Exportados após ordenação e top-k | 100 |

`top-k` limita apenas a exportação. Os 287 aceitos continuam contados em `MATCHED_RECORDS`, mesmo que somente 100 sejam gravados. O CSV de saída deve ter 20 colunas e 100 registros de dados, além do cabeçalho.

## 4. Primeiros dez itens, na ordem esperada

A similaridade nesta tabela é auxiliar para a conferência. Ela não deve aparecer como uma coluna extra no CSV.

| Posição na saída | Designação | Data de aproximação | Similaridade aproximada |
| ---: | --- | --- | ---: |
| 1 | 2021 KO2 | 2021-05-30 04:01:00 | 0.886733908107 |
| 2 | 2021 LV | 2021-05-27 13:20:00 | 0.885233794808 |
| 3 | 2021 UW5 | 2021-11-01 20:24:00 | 0.882543809879 |
| 4 | 2021 LG5 | 2021-06-12 22:19:00 | 0.881665002528 |
| 5 | 2021 UF12 | 2021-10-29 17:46:00 | 0.879373710061 |
| 6 | 2021 VU4 | 2021-11-10 19:48:00 | 0.878849002024 |
| 7 | 2023 JV | 2021-10-29 20:57:00 | 0.877528508041 |
| 8 | 2021 SP | 2021-09-17 11:50:00 | 0.871528535263 |
| 9 | 2021 VK3 | 2021-11-07 16:55:00 | 0.870477106879 |
| 10 | 2021 VR5 | 2021-11-12 09:25:00 | 0.869022898951 |

A seleção usa a coluna `year`, referente ao registro da aproximação. Ela não extrai o ano do texto da designação do asteroide.

## 5. Cálculo do primeiro item

Para `2021 KO2`, os valores do dataset são:

- `distance_au = 0.0024774154821441`;
- `velocity_km_s = 15.000294653608`;
- `risk_score = 0.4660231189680826`.

A fórmula utilizada é:

```text
s_velocidade = max(0, 1 - |velocidade - 15| / 10)
s_risco      = max(0, 1 - |risco - 0.50| / 0.20)
s_total      = (s_velocidade + 2 * s_risco) / 3
```

Nesse item, `s_velocidade ≈ 0.999970534639`, `s_risco ≈ 0.830115594840` e `s_total ≈ 0.886733908107`. Ele passa pelo limiar de 0,70 e ocupa a primeira posição por ter a maior nota entre os aceitos.

## 6. Comparar o arquivo inteiro

O arquivo `testes/esperados/nasa_verificacao.csv` contém o cabeçalho original e as 100 linhas esperadas, na ordem calculada independentemente. Foram conservados os bytes originais das linhas, sem acrescentar a similaridade.

Depois de executar o comando da seção 2, compare:

```sh
cmp resultado_verificacao.csv testes/esperados/nasa_verificacao.csv
```

Se o comando não imprimir nada e terminar com código zero, os arquivos são idênticos. Qualquer mensagem de diferença indica que conteúdo, ordem ou formato precisa ser conferido.

Para testar a exportação de todos os aceitos, mantenha `--limit 10000` e altere somente `--top-k` para `0`: o esperado passa a ser `MATCHED_RECORDS=287` e `OUTPUT_RECORDS=287`. Esse arquivo maior não será idêntico à referência de 100 itens.

## 7. Identificação da referência

Os hashes SHA-256 permitem verificar se os arquivos são exatamente os usados na geração da referência:

| Arquivo | SHA-256 |
| --- | --- |
| `asteroids.csv` | `be86bc6c2f1fb8670faa691d050c7900f67b2f851bd612f7ed5277cd5c3e54d6` |
| `consultas/nasa_verificacao.txt` | `08ec9097bb29b93ec84643768c1ac9d440a6670f358e2097f97b1f3e04d6887f` |
| `testes/esperados/nasa_verificacao.csv` | `7b3137f34fd6b15a4f95742a8c17ec53378cdfa04250f12eb71244af65f57fff` |

A ordenação independente é estável em ordem decrescente da nota. A 100ª nota é maior que a 101ª, portanto não existe empate na fronteira do top-k deste teste. As diferenças de arredondamento entre `Decimal` e `double` não mudam a seleção nem a ordem esperada, conforme a comparação feita com a execução do programa.
