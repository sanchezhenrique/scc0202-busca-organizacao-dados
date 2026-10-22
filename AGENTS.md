# Convenções de código

Durante este projeto, siga como referência o estilo encontrado em `/home/doxz1n/repositorios/aed`.

- Escreva código em C no estilo usado nos exercícios existentes: identificadores e comentários em português quando fizer sentido.
- Use `snake_case` para funções e variáveis. Use nomes em maiúsculas para tipos públicos definidos com `typedef` (por exemplo, `LISTA`) e mantenha estruturas internas opacas quando a interface usar esse padrão.
- Separe interfaces em arquivos `.h` e implementações em `.c`; proteja headers com include guards (`#ifndef`, `#define`, `#endif`).
- Use chaves em funções e blocos de controle, indentação de dois espaços, declarações claras e uma instrução por linha. Mantenha o formato consistente dentro de cada arquivo.
- Verifique ponteiros e resultados de alocação antes de usá-los. Para destruição, aceite ponteiro para ponteiro quando isso permitir liberar e zerar a referência do chamador.
- Utilize somente as bibliotecas padrão `stdlib.h`, `stdio.h`, `stdbool.h`, `math.h` e `string.h`. Se a tarefa exigir qualquer outra biblioteca, solicite autorização ao usuário antes de usá-la.
- Prefira funções pequenas e responsabilidades diretas, acompanhando a organização dos TADs e exercícios da pasta `aed`.
- Use programação estruturada; não utilize `goto`.
- Mantenha a representação dos TADs oculta nos arquivos `.c`, expondo tipos opacos e suas operações nos headers públicos.
- Não faça reformatação ou mudanças de estilo em código fora do escopo da tarefa; ao editar arquivos existentes, preserve as convenções locais quando houver conflito com os padrões acima.
