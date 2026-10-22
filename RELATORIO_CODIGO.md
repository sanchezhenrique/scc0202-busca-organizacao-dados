# Relatório do código — Projeto 1

## 1. O que o programa faz

O executável `trabalho` lê registros, guarda a base em uma estrutura escolhida, executa uma consulta e insere as referências aceitas em outra estrutura. Por fim, grava os primeiros resultados no formato original da entrada e libera a memória.

Cada linha de dados é representada por um TAD `ITEM`. As duas partes usam o mesmo programa, o mesmo tipo de item e os mesmos TADs de armazenamento. A seleção do leitor altera somente o tratamento da entrada.

| Leitor | Arquivo fornecido | Registros de dados | Colunas | Separador |
| --- | --- | ---: | ---: | --- |
| `nasa` | `asteroid_dataset_20251019.csv` | 89.227 | 20 | vírgula |
| `datatran` | `datatran2026.csv` | 48.712 | 30 | ponto e vírgula |

O arquivo DATATRAN possui texto em Latin-1 e vírgula decimal em `km`, `latitude` e `longitude`. Essas diferenças são tratadas no leitor, preservando os bytes originais para a saída.

Este relatório explica o funcionamento da implementação. Não apresenta o relatório acadêmico de complexidade, a avaliação experimental ou os seis exemplos completos de execução excluídos do pedido.

## 2. Organização e convenções

| Arquivo | Responsabilidade |
| --- | --- |
| `main.c` | Coordenação das etapas e encerramento. |
| `programa/opcoes.c` | Interpretação dos argumentos. |
| `programa/metricas.c` | Leitura do relógio e impressão dos indicadores. |
| `dados/item.c` | TAD ITEM: uma linha de dados, seus campos e operações sobre valores textuais. |
| `dados/tabela.c` | TAD TABELA: cabeçalho, nomes das colunas e delimitador. |
| `leitores/csv.c` | Leitura de linhas e separação dos campos CSV. |
| `leitores/leitor.c` | Abertura da base, escolha do leitor e criação dos registros. |
| `leitores/datatran.c` | Conversão dos campos específicos da Parte 2. |
| `leitores/carga.c` | Leitura e inserção direta na base. |
| `consultas/consulta.c` | Interpretação e avaliação das consultas. |
| `tads/colecao.c` | Encaminhamento das operações ao TAD selecionado. |
| `tads/pilha.c`, `fila.c` e `lista_*.c` | Implementações das oito estruturas. |
| `busca/busca.c` | Percurso da base e armazenamento dos aceitos. |
| `exportacao/exportacao.c` | Gravação do cabeçalho e dos resultados. |
| `testes/` | Verificação dos TADs, do CSV e das duas aplicações. |

As interfaces ficam nos `.h`, protegidos por include guards. As representações dos TADs ficam nos respectivos `.c`. Por exemplo, o cliente conhece `ITEM *`, mas não pode acessar seus campos internos. Ele precisa usar `item_campo`, `item_original` e as demais operações públicas.

`tads/estruturas.h` reúne as declarações dos tipos opacos `PILHA`, `FILA`, `LISTA_SEQ`, `LISTA_ENC`, `LISTA_CABECA`, `LISTA_ORD`, `LISTA_GEN` e `LISTA_CRUZ`. A declaração não revela os nós, vetores ou posições de percurso.

`OPCOES` e `METRICAS` são agrupamentos de dados usados pelo cliente. Não armazenam a representação das listas. Os TADs recebem ponteiros tipados `ITEM *`, sem ponteiros genéricos, callbacks ou tabelas de funções. O fluxo é estruturado e não utiliza saltos para rótulos.

## 3. Fluxo completo em `main.c`

### 3.1. Função `main`

`main` cria as opções e as métricas inicialmente zeradas. As mensagens de falha são impressas diretamente pela etapa responsável, sem vetor intermediário de erro. A chamada a `opcoes_ler` decide se os argumentos permitem continuar.

Se os argumentos estiverem corretos, chama `executar`. Caso contrário, imprime o modo de uso. Cada falha é informada com `printf` na função que a identifica. Ao final, `main` apresenta as métricas e retorna `EXIT_SUCCESS` ou `EXIT_FAILURE`.

O retorno do processo permite que um script diferencie sucesso de erro sem interpretar mensagens em português. A saída padrão permanece no formato `CHAVE=VALOR`: as falhas acrescentam `ERRO=...`, e argumentos inválidos também produzem `USO=...`. Em uma execução bem-sucedida, são impressos somente os oito indicadores.

### 3.2. Função `executar`

O leitor é aberto primeiro. Em seguida, a consulta é interpretada usando os nomes do cabeçalho. Isso precisa ocorrer antes da carga porque a lista ordenada, a generalizada e a cruzada utilizam informações da consulta durante suas inserções.

A função cria duas coleções distintas: uma para a base e outra para os resultados. Mesmo que os dois argumentos indiquem `fila`, por exemplo, são duas filas independentes.

Com a preparação concluída, chama, nesta ordem:

1. `carga_executar`: lê os registros e insere na base;
2. `busca_executar`: percorre a base e preenche o resultado;
3. `exportacao_gravar`: escreve a saída.

Essas chamadas estão ligadas por `&&`. Em C, a expressão deixa de avaliar as próximas chamadas assim que uma retorna `false`. Assim, uma carga com erro não é seguida pela busca, e uma busca com erro não produz uma exportação anunciada como válida.

A limpeza fica depois dos condicionais e ocorre tanto no sucesso quanto na falha. Primeiro o resultado é destruído sem apagar os registros; depois a base é destruída apagando-os. Consulta e leitor são destruídos por último. Os ponteiros começam nulos, e os destrutores aceitam esse estado, permitindo encerrar mesmo quando a preparação ficou incompleta.

## 4. Argumentos em `programa/opcoes.c`

### 4.1. Conversão de limites

`ler_natural` converte `--limit` e `--top-k` dígito a dígito. Uma string vazia, sinal negativo ou caractere que não seja dígito é rejeitado.

Antes de calcular `numero * 10 + algarismo`, verifica se o resultado cabe em `size_t`. A expressão `(size_t)-1` representa o maior valor desse tipo sem sinal. A verificação ocorre antes da multiplicação para impedir que ela já transborde.

### 4.2. Identificação e atribuição

`identificar_opcao` compara o nome recebido com os oito argumentos aceitos e devolve seu índice. `atribuir_opcao` usa esse índice em um `switch` para preencher o campo correspondente de `OPCOES`.

Os caminhos e o nome do leitor apontam para as strings de `argv`. Não são cópias alocadas pelo programa e, portanto, não são liberados na limpeza. Os nomes das estruturas são convertidos para `TIPO_COLECAO` por `colecao_tipo`.

### 4.3. Validação do conjunto

`opcoes_ler` percorre os argumentos em pares: opção e valor. O vetor `presentes` impede que uma opção seja repetida. Ao terminar, exige `--reader`, `--input`, `--base`, `--result`, `--query` e `--output`.

`--limit` e `--top-k` são opcionais e assumem zero. Zero significa toda a base ou todos os resultados, respectivamente. A função também rejeita um caminho de saída textualmente igual ao caminho de entrada ou da consulta, para evitar sua sobrescrita direta.

`opcoes_imprimir_uso` apresenta os dois leitores disponíveis e o padrão dos argumentos.

## 5. Dados: TAD ITEM e TAD TABELA

### 5.1. TAD `TABELA` em `dados/tabela.c`

A tabela mantém o cabeçalho original, um vetor com os nomes das colunas, a quantidade de colunas e o delimitador. Esses dados são compartilhados pela interpretação da consulta e pela exportação, sem serem copiados para cada registro.

| Função | Funcionamento |
| --- | --- |
| `tabela_criar` | Confere os parâmetros e recusa nomes de colunas vazios ou repetidos. Após alocar a tabela, assume a responsabilidade pelas strings recebidas. |
| `tabela_indice` | Procura um nome no vetor e escreve sua posição no parâmetro de saída. Retorna `false` se não encontrar. |
| `tabela_colunas` | Informa quantas colunas o cabeçalho define. |
| `tabela_campo` | Entrega o nome de uma coluna pelo índice. |
| `tabela_cabecalho` | Entrega o texto original usado na primeira linha da saída. |
| `tabela_delimitador` | Entrega o separador que o leitor escolheu para aquele arquivo. |
| `tabela_apagar` | Libera nomes, vetor, cabeçalho e tabela, zerando a referência do chamador. |

Nomes duplicados são recusados porque um comando como `FILTER|km|...` ficaria ambíguo se existissem duas colunas chamadas `km`.

### 5.2. TAD `ITEM` em `dados/item.c`

`dados/item.h` declara `typedef struct item ITEM` e suas operações. Somente `dados/item.c` define `struct item`. As listas, a pilha e a fila guardam `ITEM *`, sem acessar diretamente os campos da estrutura. A consulta também utiliza os acessores públicos.

Cada item representa uma linha lógica de dados do CSV, que pode incluir quebras internas entre aspas. O cabeçalho é representado separadamente pela `TABELA`.

| Campo interno do ITEM | Finalidade |
| --- | --- |
| `original` | String com os bytes da linha lida, preservados para a saída. |
| `campos` | Vetor de strings separado pelo parser e preparado pelo leitor. |
| `quantidade` | Número de campos efetivamente presentes nessa linha. |
| `posicao` | Ordem de leitura iniciada em zero; usada como ordem padrão da lista ordenada. |

A linha original e os campos separados têm funções diferentes. Os campos permitem comparar valores; a linha original permite exportar os mesmos bytes, incluindo aspas, espaços, vírgulas decimais e acentos na codificação de entrada.

| Função | Funcionamento |
| --- | --- |
| `item_criar` | Confere se a linha, o vetor e seus campos são válidos e aloca o item. Em caso de sucesso, assume a linha e o vetor recebidos; em caso de falha, eles continuam com o chamador. |
| `item_campo` | Acessa uma coluna sem expor a estrutura. Se o item existe, mas o campo final foi omitido, devolve `""`. |
| `item_original` | Devolve a linha destinada à exportação. |
| `item_posicao` | Informa a posição original na base. |
| `item_numero` | Obtém um campo e solicita sua conversão a `texto_numero`. |
| `item_apagar` | Libera campos, vetor, linha original e item, zerando o ponteiro do chamador. Aceita referências nulas. |

Os textos retornados pelos acessores são emprestados. Quem os recebe não deve liberá-los nem modificá-los. `campos_apagar` centraliza a liberação das strings e do vetor, sendo usado também nos caminhos de erro do leitor.

`item_criar` recebe os dados já alocados pelo leitor e guarda seus ponteiros. Ele não aloca outra cópia da linha nem depende do domínio NASA ou DATATRAN. Se a criação falhar, o leitor continua responsável por liberar a linha e os campos. No sucesso, o ITEM assume esse conteúdo.

Por exemplo, uma linha simplificada do DATATRAN pode conter `742942;MG;146,1`. O campo consultável de quilômetro é preparado como `146.1`, enquanto `item_original` continua devolvendo a linha com `146,1`. `item_numero` converte o valor consultável em `double` somente quando necessário. Em caso de falha, retorna `false` sem substituir o número informado pelo chamador.

A base guarda os itens que recebeu com sucesso. A busca coloca os mesmos ponteiros no resultado. A destruição do resultado usa `false` para liberar somente nós ou vetor; a destruição da base usa `true` para chamar `item_apagar` em cada item. Retirar um item de uma coleção devolve o ponteiro vivo, cabendo ao chamador inseri-lo em outro lugar ou destruí-lo.

### 5.3. Operações sobre valores em `dados/item.c`

`texto_numero` usa `strtod`, confere se houve conversão, rejeita valores não finitos e permite somente espaços depois do número. Assim, `1.5x`, vazio, `NaN` e infinito não são aceitos como números válidos. A consulta deve escrever números com ponto decimal.

`texto_booleano` reconhece `true` e `false` sem distinguir maiúsculas. Sua normalização é limitada às letras ASCII desses literais; não altera nomes de municípios ou outros textos da base.

`texto_classe`, auxiliar privada, separa valores vazios, números, booleanos e textos comuns. `texto_comparar` compara primeiro essas classes. Dentro da mesma classe, compara números e booleanos pelos valores e textos por `strcmp`. Essa separação mantém uma ordem coerente em colunas que misturam tipos. O retorno é negativo, zero ou positivo.

`texto_copiar` aloca uma cópia independente de uma string. É usada quando uma consulta ou um grupo precisa conservar um texto depois que a variável de origem deixa de existir.

## 6. Leitura de linhas e CSV em `leitores/csv.c`

### 6.1. `arquivo_ler_linha`

A função lê blocos com `fgets`. O bloco local tem 1.024 posições, mas o registro completo não fica limitado a esse tamanho: um buffer alocado cresce com `realloc`, e `memcpy` acrescenta os blocos lidos.

No modo CSV, a variável `aspas` acompanha se a leitura está dentro de um campo citado. Uma quebra de linha encerra o registro somente quando não está dentro desse campo. Duas aspas seguidas alternam o estado duas vezes, preservando a situação de leitura para uma aspa escapada.

A função verifica `ferror` e recusa um campo que chega ao fim do arquivo sem fechar as aspas. No modo de consulta, as aspas não possuem esse papel, e o terminador da linha é retirado.

Seu contrato diferencia dois estados: `false` representa falha; `true` com ponteiro nulo representa fim normal do arquivo. O cliente não utiliza `while (!feof(...))`, pois o fim só é conhecido depois da tentativa de leitura.

`csv_ler_linha` chama essa rotina no modo CSV. `csv_linha_vazia` reconhece linhas completamente vazias, com LF ou CRLF; não remove espaços de campos válidos.

### 6.2. `csv_ler_campo`

Essa auxiliar percorre um campo e devolve uma nova string. Se o campo começa com aspas, copia seu conteúdo até o fechamento e transforma cada par `""` em uma única aspa. Depois de fechar o campo, só admite o delimitador ou o fim do registro.

Em um campo não citado, o delimitador encerra o valor. Aspas inesperadas e quebras de linha soltas são recusadas. O índice recebido por endereço é atualizado para que a próxima chamada continue na posição correta.

### 6.3. `csv_separar`

A função desconsidera apenas a quebra externa do registro, sem modificar a string original. Em seguida, chama `csv_ler_campo` repetidamente e amplia o vetor de ponteiros conforme encontra novas colunas.

O laço também produz o último campo vazio quando a linha termina em delimitador. Isso evita perder colunas ausentes no final. Se qualquer alocação ou interpretação falhar, os campos já prontos são liberados.

No DATATRAN, um valor como `"Aclive;Reta"` é um único campo, apesar do ponto e vírgula interno. A distinção depende das aspas, e não de uma separação indiscriminada por `;`.

### 6.4. Funções de leitura e escrita utilizadas

`fgets` é a função que efetivamente lê o conteúdo dos arquivos. Tanto o CSV quanto a consulta passam por `arquivo_ler_linha`. `fopen` prepara o acesso, `ferror` confere falhas e `fclose` encerra o acesso. O programa não usa `scanf`, `fscanf`, `sscanf`, `fread` ou `fgetc`. Os argumentos da execução são recebidos em `argc` e `argv`.

`csv_separar` e `csv_ler_campo` percorrem o texto já lido para identificar os campos. `strtod`, em `dados/item.c`, converte um desses textos em número; não lê o arquivo nem o teclado. A função `ler_natural`, das opções, converte os caracteres dos limites com um laço próprio.

A saída na tela utiliza somente `printf`. A escrita de conteúdo nos arquivos utiliza `fputs`, inclusive para preservar a linha original do CSV. Nos testes, `tmpfile` cria arquivos temporários e `rewind` retorna ao início para permitir sua leitura.

## 7. Seleção do leitor em `leitores/leitor.c`

O TAD oculto `LEITOR` guarda o arquivo aberto, sua tabela, a contagem de registros entregues e a indicação de uso do DATATRAN. Ele não guarda a propriedade dos registros que já entregou ao cliente.

### 7.1. Abertura e cabeçalho

`leitor_abrir` verifica o nome, aceita `nasa` ou `datatran`, aloca o leitor e abre a entrada em `rb`. Esse modo conserva os bytes das quebras de linha e da codificação original.

`ler_cabecalho` escolhe vírgula para NASA ou ponto e vírgula para DATATRAN, separa os nomes e cria `TABELA`. Para NASA, confere `asteroid_designation`, `close_approach_date` e `distance_au`. Para DATATRAN, delega a validação a `datatran_validar`.

A validação usa nomes, sem fixar a posição dessas colunas no código. As demais colunas do cabeçalho continuam disponíveis para consultas. Um arquivo do outro leitor é rejeitado, em vez de ser interpretado silenciosamente com o delimitador errado.

O leitor imprime os diagnósticos diretamente com `printf`, no formato `ERRO=...`. Se a abertura ou o cabeçalho falhar, a memória e o arquivo já adquiridos são desfeitos antes do retorno nulo. As operações não recebem vetor de erro nem parâmetro de capacidade para mensagens.

`leitor_tabela` entrega uma referência constante à tabela criada na abertura. A consulta usa essa referência para localizar colunas, e a exportação usa seu cabeçalho. A tabela continua pertencendo ao leitor e não deve ser destruída pelo cliente separadamente.

### 7.2. Entrega de cada registro

`leitor_proximo` começa zerando a referência de saída, ignora linhas vazias e solicita um registro lógico ao parser. Depois, separa os campos e verifica se não existem mais colunas do que as anunciadas pelo cabeçalho.

Para NASA, os valores seguem diretamente para a criação do registro. Para DATATRAN, `datatran_tratar` adapta os campos consultáveis antes dessa criação.

Somente depois de `item_criar` devolver um `ITEM *` válido a função incrementa `lidos`. A posição anterior desse contador é guardada no registro. Se alguma etapa falhar, libera a linha e todos os campos que ainda não passaram a pertencer a um registro.

O cliente recebe `true` com um registro alocado, `true` com registro nulo no EOF ou `false` com mensagem de falha. `leitor_apagar` fecha o arquivo, destrói a tabela, libera o leitor e zera a referência.

## 8. Leitor DATATRAN em `leitores/datatran.c`

### 8.1. `datatran_validar`

A função procura `id`, `data_inversa`, `uf`, `km`, `latitude` e `longitude` no cabeçalho. Esses campos identificam o formato esperado e incluem os três valores decimais que precisam de adaptação. As 30 colunas originais permanecem no registro e na saída.

### 8.2. `converter_texto`

O arquivo fornecido está em Latin-1, enquanto os arquivos de consulta são escritos em UTF-8. Comparar diretamente os bytes de um texto acentuado nessas duas codificações não produziria igualdade.

A auxiliar privada recebe o endereço de uma string de campo. Primeiro procura um byte acima de 127. Se todos os caracteres forem ASCII, o texto já é compatível e nenhuma cópia é necessária.

Se houver acentos, aloca até `2 * tamanho + 1` bytes. Cada byte ASCII é copiado. Cada byte Latin-1 acima de 127 é convertido em dois bytes UTF-8, calculados por `192 + caractere / 64` e `128 + caractere % 64`. Por exemplo, `ã`, representado por 227 em Latin-1, passa a ser o par de bytes 195 e 163 em UTF-8.

A string antiga só é liberada depois da criação da nova. Uma falha de alocação devolve `false`, permitindo ao leitor desfazer os campos já preparados. Essa conversão usa somente as bibliotecas autorizadas.

### 8.3. `datatran_tratar`

A função percorre os campos, converte os textos necessários para UTF-8 e consulta o nome de cada coluna. Somente em `km`, `latitude` e `longitude` troca a vírgula decimal por ponto. Contagens inteiras como `mortos`, `feridos` e `pessoas` já possuem um formato numérico aceito.

Assim, o campo `146,1` passa a ser consultado como `146.1`, permitindo filtros, similaridade e ordenação numérica. Um texto como `Colisão frontal` passa a coincidir com o mesmo texto em uma consulta UTF-8.

Essa adaptação não altera a linha original. A saída conserva Latin-1, as vírgulas decimais, as aspas e o ponto e vírgula. O texto `NA` é conservado como informação textual; quando utilizado em um critério numérico, não é convertido em zero.

## 9. Carga em `leitores/carga.c`

`carga_executar` inicia a medição e repete a leitura enquanto o limite permitir. O limite é verificado antes de solicitar outro item, evitando carregar um registro a mais.

Cada leitura possui três desdobramentos. Uma falha encerra a carga com erro. EOF encerra normalmente. Um registro válido é imediatamente inserido na base escolhida.

A base assume o registro somente se `colecao_inserir` retornar `true`. Se a inserção falhar, `item_apagar` libera o item recém-criado. `INPUT_RECORDS` é incrementado somente depois de uma inserção bem-sucedida.

A medição final inclui leitura, criação dos registros e inserção na base. A abertura do arquivo, a leitura do cabeçalho e a interpretação da consulta já ocorreram antes dessa região.

## 10. Consultas em `consultas/consulta.c`

### 10.1. Representação e inclusão de critérios

`CONSULTA` é opaco. Internamente mantém uma lista encadeada de `CRITERIO`, início e fim dessa lista, limiar, maior peso e campos usados na organização dos dados.

Cada critério indica se é `FILTER` ou `SIM`. Os filtros guardam o índice da coluna, o operador e uma cópia do alvo textual. As similaridades guardam coluna, alvo numérico, tolerância e peso. A lista conserva a ordem do arquivo.

`adicionar_criterio` aloca um nó, resolve o nome do campo com `tabela_indice` e valida os parâmetros. Os operadores dos filtros correspondem a `==`, `!=`, `<`, `<=`, `>` e `>=`. Uma tolerância de SIM precisa ser positiva; o peso deve ser não negativo. Os números precisam ser finitos.

Somente um critério válido é ligado ao final da lista. O alvo do filtro é copiado porque a linha usada na interpretação será liberada. O maior peso é atualizado para permitir o cálculo seguro da média ponderada.

### 10.2. Interpretação das linhas

`interpretar` substitui cada `|` pelo terminador de string e guarda ponteiros para as partes dentro da própria linha. A separação manual conserva um alvo vazio, como em `FILTER|campo|==|`.

A quantidade de partes é conferida antes de utilizar seus valores. FILTER possui quatro partes; SIM possui cinco; ACCEPT possui duas; os comandos de organização possuem três.

| Comando | Tratamento |
| --- | --- |
| `FILTER` | Acrescenta uma condição obrigatória. |
| `SIM` | Acrescenta um critério de similaridade. |
| `ACCEPT` | Define o limiar entre zero e um. O padrão é zero. |
| `ORDER` | Define coluna ou chave virtual `similarity` e direção `ASC` ou `DESC`. |
| `GROUP` | Resolve os campos de grupo e subgrupo. |
| `CROSS` | Resolve os campos dos eixos zero e um. |

As flags `tem_accept`, `tem_ordem`, `tem_grupo` e `tem_cruzamento` impedem redefinições desses comandos. FILTER e SIM podem aparecer várias vezes. A chave `similarity` não precisa existir no cabeçalho, pois seu valor é calculado pela consulta.

### 10.3. Leitura e destruição

`consulta_ler` abre o arquivo, usa `arquivo_ler_linha` no modo texto e ignora linhas vazias ou iniciadas por `#`. Uma linha inválida encerra a interpretação e produz uma mensagem com seu número.

Depois da leitura, confere se existe algum SIM com todos os pesos iguais a zero. Esse caso é recusado porque a média não teria denominador válido. Uma consulta sem SIM continua válida.

`consulta_apagar` percorre os critérios, libera os alvos textuais dos filtros, libera os nós e destrói a consulta. Os critérios SIM não possuem alvo textual alocado; seus ponteiros iniciam nulos por `calloc`, tornando a mesma limpeza válida para ambos.

### 10.4. Filtros e limiar

`consulta_aceita` percorre os filtros. Se o alvo for numérico, exige que o campo também possa ser convertido. Se o alvo for booleano, exige um booleano válido. Isso impede que um ausente seja aceito como se fosse zero ou falso.

A comparação é interpretada conforme o operador guardado. Basta um filtro falhar para o registro ser rejeitado. Só depois de todos os filtros passarem é avaliado o limiar de similaridade.

Sem SIM, os filtros são suficientes, e ACCEPT não acrescenta outra restrição. Uma consulta vazia aceita todos os registros válidos carregados.

### 10.5. Similaridade

Para cada SIM, `consulta_similaridade` calcula:

```text
nota = max(0, 1 - abs(valor - alvo) / tolerancia)
```

Uma correspondência exata recebe um. Uma diferença igual ou superior à tolerância recebe zero. Um campo ausente ou não numérico conserva nota zero naquele critério, mas seu peso continua no denominador.

Os pesos são divididos pelo maior peso antes da soma. Essa escala conserva a média e evita transbordamento desnecessário com pesos muito grandes. A função devolve a soma das notas ponderadas dividida pela soma dos pesos. Sem SIM, devolve um, uma convenção que mantém todos empatados em `ORDER|similarity`.

### 10.6. Organização das estruturas

`consulta_estrutura` verifica se o tipo existe. Para lista generalizada, exige GROUP; para cruzada, exige CROSS. A exigência vale tanto para base quanto para resultado.

`consulta_comparar` é usada pela lista ordenada. Sem ORDER, compara as posições originais dos registros. Com ORDER, compara a coluna selecionada ou as notas de similaridade. DESC inverte o sinal do resultado. Um empate permanece zero e é preservado pela inserção estável da lista.

`consulta_grupo` entrega o valor do campo do nível solicitado. `consulta_cruzar` compara os registros pelo campo do eixo solicitado. Cada eixo de CROSS fica crescente, independentemente de ORDER. Esses acessos usam a consulta sem revelar a representação de `ITEM` aos TADs.

## 11. Interface comum em `tads/colecao.c`

`COLECAO` guarda o tipo escolhido, sua contagem e uma referência para o TAD correspondente. Somente a estrutura escolhida é criada. Os `switch` fazem chamadas diretas, como `pilha_inserir` ou `fila_inserir`.

| Função | Funcionamento |
| --- | --- |
| `colecao_tipo` | Converte o nome recebido na linha de comando para o valor enumerado. |
| `colecao_criar` | Confere a consulta, aloca a coleção e chama o construtor específico. Desfaz a alocação se o construtor falhar. |
| `colecao_inserir` | Valida as referências, encaminha a inserção e incrementa o tamanho somente no sucesso. |
| `colecao_retirar` | Encaminha a retirada e diminui o tamanho quando recebe um registro. O registro é devolvido vivo ao chamador. |
| `colecao_tamanho` | Consulta a contagem sem expor a estrutura. |
| `colecao_reiniciar` | Posiciona o percurso no começo. O eixo um é permitido somente na cruzada. |
| `colecao_proximo` | Devolve o próximo registro e avança a posição interna, sem retirá-lo. |
| `colecao_apagar` | Destrói o TAD selecionado e a coleção, conforme a responsabilidade pelos registros. |

A forma de percurso é a mesma para o cliente:

```c
colecao_reiniciar(base, 0);
while ((item = colecao_proximo(base)) != NULL) {
  /* Avaliar o item. */
}
```

As inserções e retiradas reiniciam a posição interna de percurso. Por isso, não se deve alterar uma coleção enquanto ela está sendo percorrida. A busca respeita essa regra: percorre a base e insere em um objeto de resultado distinto.

## 12. Funcionamento dos oito TADs

### 12.1. Pilha — `tads/pilha.c`

`PILHA` mantém `inicio`, que representa o topo, e `atual`, usado no percurso. Cada nó possui um registro e o elo para o próximo.

- `pilha_criar` inicializa uma pilha vazia com `calloc`.
- `pilha_inserir` aloca um nó, liga-o ao topo anterior e atualiza `inicio`. O último inserido fica primeiro.
- `pilha_reiniciar` coloca `atual` no topo; `pilha_proximo` devolve o registro atual e avança pelo elo.
- `pilha_retirar` desloca o topo para o próximo nó e libera somente o nó removido.
- `pilha_apagar` repete as retiradas, libera os registros quando solicitado e destrói a pilha.

O percurso da saída é LIFO. A busca também percorre a pilha nessa ordem, sem desmontá-la.

### 12.2. Fila — `tads/fila.c`

`FILA` mantém início, fim e posição de percurso.

- `fila_criar` deixa essas referências nulas.
- `fila_inserir` liga o novo nó depois de `fim`. Na primeira inserção, também define `inicio`.
- `fila_reiniciar` começa em `inicio`; `fila_proximo` segue os próximos nós.
- `fila_retirar` remove o primeiro nó. Ao retirar o último, também zera `fim`, evitando uma referência para memória liberada.
- `fila_apagar` retira todos os itens e libera o objeto, aplicando a opção de destruir registros.

A fila conserva a ordem de chegada, ou FIFO.

### 12.3. Lista sequencial — `tads/lista_seq.c`

`LISTA_SEQ` possui um vetor de ponteiros, o tamanho ocupado, a capacidade alocada e o índice atual.

- `lista_seq_criar` começa sem vetor alocado.
- `lista_seq_inserir` reserva inicialmente 16 posições. Quando necessário, dobra a capacidade usando uma variável temporária para `realloc`; uma falha não perde o vetor anterior. Depois acrescenta o registro no final.
- `lista_seq_reiniciar` zera o índice; `lista_seq_proximo` devolve a posição indicada e incrementa esse índice.
- `lista_seq_retirar` guarda o primeiro ponteiro e fecha a lacuna com `memmove`, adequado a regiões sobrepostas.
- `lista_seq_apagar` percorre os ponteiros para liberar registros quando necessário e libera o vetor diretamente. Não desloca o vetor repetidamente só para destruí-lo.

### 12.4. Lista encadeada — `tads/lista_enc.c`

`LISTA_ENC` mantém um início e uma posição atual, sem sentinela e sem referência permanente para o fim.

- `lista_enc_criar` inicializa a lista vazia.
- `lista_enc_inserir` trata o início vazio ou percorre os elos até encontrar o último nó. O novo nó é anexado ali.
- `lista_enc_reiniciar` e `lista_enc_proximo` percorrem a cadeia a partir do início.
- `lista_enc_retirar` desliga o primeiro nó, libera-o e devolve o registro.
- `lista_enc_apagar` repete as retiradas e libera os recursos conforme a opção recebida.

### 12.5. Lista com cabeça — `tads/lista_cabeca.c`

`lista_cabeca_criar` faz uma alocação adicional para a sentinela. `inicio` aponta permanentemente para esse nó, cujo registro é nulo. Na lista vazia, `fim` também aponta para ele.

`lista_cabeca_inserir` sempre liga o novo nó depois de `fim`, inclusive na lista vazia. Isso é possível porque a sentinela continua existindo.

`lista_cabeca_reiniciar` começa em `inicio->proximo`. `lista_cabeca_proximo` nunca entrega a sentinela como se fosse um registro. `lista_cabeca_retirar` modifica esse mesmo elo da sentinela e, ao esvaziar a lista, faz `fim` voltar para ela.

`lista_cabeca_apagar` destrói os nós de dados e depois libera a sentinela separadamente. Essa representação é efetivamente diferente da lista simples sem cabeça.

### 12.6. Lista ordenada — `tads/lista_ord.c`

`lista_ord_criar` guarda uma referência emprestada à consulta. Por isso, ela deve continuar viva enquanto a lista a utiliza.

`lista_ord_inserir` percorre os nós com as referências `anterior` e `atual`, comparando seus registros por `consulta_comparar`. O laço avança também sobre empates. Depois liga o novo nó antes de `atual`, atualizando `inicio` quando não existe anterior.

A lista já está ordenada ao terminar cada inserção. Não existe uma etapa posterior que ordene todos os itens. Inserir depois dos empates conserva a ordem de chegada desses registros.

`lista_ord_reiniciar` e `lista_ord_proximo` percorrem essa ordem. `lista_ord_retirar` remove seu primeiro item. `lista_ord_apagar` destrói os nós e, quando solicitado, os registros, sem liberar a consulta emprestada.

### 12.7. Lista generalizada — `tads/lista_gen.c`

A representação possui nós `GRUPO` e folhas `FOLHA`. Um grupo do primeiro nível aponta para seus subgrupos; cada subgrupo possui início e fim de uma lista de folhas. Somente as folhas referenciam registros.

`encontrar_grupo` procura uma chave entre irmãos. `criar_grupo` aloca o nó e uma cópia da chave. `anexar_grupo` coloca um novo irmão no fim, preservando a primeira ocorrência.

`lista_gen_criar` exige uma consulta com GROUP. Na inserção, `lista_gen_inserir` obtém a primeira chave, encontra ou cria o grupo, obtém a segunda chave e encontra ou cria o subgrupo. Em seguida, anexa uma folha ao subgrupo. O novo grupo do primeiro nível só é publicado depois de concluir as alocações necessárias; uma falha não deixa esse grupo incompleto na coleção.

`lista_gen_reiniciar` posiciona grupo, subgrupo e folha atuais nos primeiros existentes. `lista_gen_proximo` avança primeiro entre folhas; depois entre subgrupos; depois entre grupos. Assim, itens de um mesmo subgrupo ficam juntos no percurso.

`lista_gen_retirar` remove a primeira folha do primeiro subgrupo. Se o subgrupo ficar vazio, libera seu nó e chave; se o grupo também ficar sem subgrupos, libera-o. `lista_gen_apagar` repete esse processo até esvaziar a hierarquia.

### 12.8. Lista cruzada — `tads/lista_cruz.c`

Cada nó possui uma referência ao registro e dois pares de elos: `proximo[2]` e `anterior[2]`. `LISTA_CRUZ` mantém dois inícios, uma posição atual e o eixo selecionado.

`lista_cruz_criar` exige CROSS. `lista_cruz_inserir` aloca um único nó e, para cada eixo, encontra sua posição usando `consulta_cruzar`. Religa anterior e próximo daquele eixo. Ao terminar, o mesmo nó participa das duas organizações, sem duplicar o registro.

`lista_cruz_reiniciar` escolhe o eixo zero ou um e posiciona a visita em seu início. `lista_cruz_proximo` segue apenas os elos desse eixo. Trocar de eixo muda a ordem dos mesmos itens.

`lista_cruz_retirar` escolhe o primeiro nó do eixo zero e o desliga dos dois encadeamentos antes de liberá-lo uma única vez. `lista_cruz_apagar` repete as retiradas. A execução padrão busca e exporta pelo eixo zero; o segundo eixo é exposto pela interface e exercitado nos testes.

## 13. Busca em `busca/busca.c`

`busca_executar` verifica os parâmetros, inicia o relógio e reinicia a base. O laço obtém um registro por vez com `colecao_proximo`.

Quando `consulta_aceita` retorna `true`, incrementa `MATCHED_RECORDS` e insere a referência em `resultado`. O registro não é copiado nem retirado da base. A ordem das inserções decorre do percurso da base; a ordem final também depende do TAD escolhido para o resultado.

A inserção é medida entre dois instantes próprios. Ao terminar o percurso, o tempo total da região de busca é reduzido pela soma desses intervalos. Dessa forma, a inserção dos resultados não é contada simultaneamente em `SEARCH_MS` e `RESULT_INSERT_MS`.

Uma falha de inserção ou relógio interrompe o processamento com erro. `top-k` não participa da busca: ele limita somente a exportação e não reduz `MATCHED_RECORDS`.

## 14. Exportação em `exportacao/exportacao.c`

`exportacao_gravar` abre a saída em `wb`, identifica a quebra de linha do cabeçalho e escreve esse cabeçalho original. Depois reinicia o resultado e percorre seus registros até o fim ou até atingir `top-k`. Zero permite todos.

O laço verifica o limite antes de solicitar outro item. A ordem é a do próprio TAD: topo da pilha, início da fila/listas, hierarquia da generalizada ou primeiro eixo da cruzada. A exportação não cria um vetor auxiliar para reordenar os itens.

`gravar_linha` escreve `item_original`, não os campos adaptados. Se uma linha não possuía terminador e mudou de posição, acrescenta uma quebra compatível para impedir que ela se una à próxima. O cabeçalho, as colunas, a ordem das colunas e o delimitador permanecem os da entrada. Nenhuma coluna de similaridade é acrescentada.

As operações de escrita e `fclose` são conferidas. Essa última conferência é necessária porque uma falha pode aparecer somente quando o buffer de saída é descarregado no fechamento.

## 15. Indicadores em `programa/metricas.c`

`metricas_agora` lê `clock_gettime(CLOCK_MONOTONIC)` e converte segundos e nanossegundos para milissegundos. Ele fornece um instante; cada duração é calculada pela diferença entre dois instantes. Esse relógio não acompanha ajustes no horário do sistema.

O uso de `<time.h>` foi autorizado pelo usuário. O `Makefile` define `_POSIX_C_SOURCE=200809L` para habilitar essa interface no ambiente Linux.

| Indicador | Significado em uma execução bem-sucedida |
| --- | --- |
| `STATUS` | `OK`; falhas produzem `ERROR` e retorno diferente de zero. |
| `INPUT_RECORDS` | Registros inseridos na base, sem contar o cabeçalho. |
| `MATCHED_RECORDS` | Registros que passaram pela consulta. |
| `OUTPUT_RECORDS` | Registros exportados após aplicar `top-k`. |
| `LOAD_MS` | Leitura, preparação dos registros e inserção na base. |
| `SEARCH_MS` | Percurso e avaliação, descontando a inserção dos resultados. |
| `RESULT_INSERT_MS` | Soma dos intervalos de inserção no resultado. |
| `WRITE_MS` | Abertura, escrita e fechamento da saída. |

`metricas_imprimir` utiliza o mesmo formato nas falhas, permitindo observar as etapas e contagens já realizadas. Todas as mensagens na tela utilizam `printf`; os diagnósticos em português aparecem em `ERRO=...`, na saída padrão, antes das métricas. Não há formatação intermediária em buffers de erro.

## 16. Responsabilidade pela memória

| Recurso | Responsável por destruir |
| --- | --- |
| Strings de `argv` | Não são alocadas pelo programa; `OPCOES` apenas as referencia. |
| Arquivo de entrada e `TABELA` | `LEITOR`, por `leitor_apagar`. |
| Cabeçalho e nomes de colunas | `TABELA`. |
| Linha original e campos de um item | `ITEM`. |
| Registros carregados com sucesso | Coleção da base. |
| Nós, folhas ou vetor do resultado | Coleção do resultado, sem apagar os registros compartilhados. |
| Chaves de grupos | Seus nós `GRUPO`, que guardam cópias próprias. |
| Critérios e alvos dos filtros | `CONSULTA`. |

A sequência de encerramento usada por `executar` é:

```c
colecao_apagar(&resultado, false);
colecao_apagar(&base, true);
consulta_apagar(&consulta);
leitor_apagar(&leitor);
```

O primeiro passo remove apenas referências adicionais. O segundo libera os únicos registros compartilhados. A consulta permanece válida até o fim do uso dos TADs. O leitor mantém o cabeçalho vivo até depois da exportação.

Os destrutores recebem ponteiro para ponteiro para poder zerar a variável do chamador. Uma chamada com referência nula não tenta liberar memória. Esse padrão permite usar a mesma limpeza quando só uma parte da preparação foi concluída.

## 17. Percurso de um registro real do DATATRAN

O registro `742942`, presente no arquivo fornecido, possui `uf = MG`, `km = 146,1`, `pessoas = 6` e `feridos = 4`.

Ao lê-lo, o parser guarda a linha original e separa os campos. O leitor DATATRAN transforma apenas o valor consultável de `km` em `146.1` e converte os textos acentuados para UTF-8. O registro é inserido na base escolhida.

Na consulta `datatran_proximidade.txt`, ele passa pelos filtros de MG e de pelo menos um ferido. Para `km`, o alvo 150 e a tolerância 50 produzem nota aproximada de `0,922`. Para `pessoas`, a diferença entre 6 e o alvo 3 é igual à tolerância 3, produzindo zero. Com pesos iguais, a média é aproximadamente `0,461`, acima do limiar `0,40`.

A busca insere a mesma referência no resultado. Se este for `lista_ord`, a nota define sua posição conforme ORDER. Se for `lista_gen`, os campos de GROUP definem grupo e subgrupo. Caso esteja entre os itens exportados, a saída usa a linha original e continua contendo `146,1`, sem acrescentar sua nota como coluna.

## 18. Consultas entregues para a Parte 2

`consultas/datatran_graves.txt` seleciona acidentes com pelo menos uma morte. ORDER usa a quantidade de mortos, GROUP usa estado e tipo de acidente, e CROSS usa estado e rodovia. Isso permite examinar os acidentes graves por diferentes organizações. A base fornecida contém 3.491 registros que satisfazem esse filtro.

`consultas/datatran_proximidade.txt` seleciona acidentes com feridos em MG e calcula proximidade do km 150 e de três pessoas envolvidas. Exige média de pelo menos `0.40` e define ordem decrescente de similaridade. GROUP usa rodovia e classificação; CROSS usa rodovia e quilômetro.

Os dois arquivos podem ser usados com qualquer combinação de estruturas, porque incluem os comandos necessários às listas avançadas. Cada estrutura aplica a organização pertinente; ORDER não substitui o acesso próprio de uma fila ou pilha.

## 19. Compilação, testes e leitura recomendada

`make` compila diretamente os módulos e produz `trabalho`, com C99, `-Wall -Wextra -pedantic` e a biblioteca matemática padrão. `make test` executa os testes em C e os testes de integração em Python 3. Python é usado apenas na verificação, sem dependência de execução do programa C.

`testes/teste_colecoes.c` verifica os percursos, ordenação após cada inserção, estabilidade dos empates, retiradas, reutilização após esvaziar, crescimento do vetor, grupos vazios de chave e ambos os eixos da cruzada. Também verifica que destruir um resultado não destrói os registros da base.

`testes/teste_csv.c` confere aspas, delimitadores dentro de campos, campos vazios finais, quebras internas, EOF sem quebra final, números e entradas malformadas. A função `testar_item` também verifica a transferência dos ponteiros no sucesso, a preservação dos argumentos na falha, os acessos públicos, a separação entre campos adaptados e linha original, a conversão numérica e a destruição que zera a referência.

`testes/teste_programa.py` compara as 64 combinações de base e resultado na aplicação NASA com uma referência independente. Também confere argumentos, consultas inválidas, indicadores e uma exportação integral idêntica ao arquivo original.

`testes/teste_datatran.py` repete as 64 combinações para a Parte 2. Confere filtros UTF-8 contra campos Latin-1, números decimais, ORDER e CROSS numéricos, ponto e vírgula citado, leitores incompatíveis e as duas consultas entregues. A exportação integral deve ser idêntica ao DATATRAN original, incluindo codificação e quebras de linha. Os testes não alteram os datasets.

Para revisar o projeto, a ordem sugerida é `main.c`, `opcoes.c`, `item.c`, `tabela.c`, `csv.c`, `leitor.c`, `datatran.c`, `carga.c`, `consulta.c`, os TADs, `busca.c` e `exportacao.c`. Essa sequência acompanha a vida de um registro desde a entrada até a liberação.
