# Fabrica-de-Bolos-Multithreads
Simulação de uma linha de produção de bolos desenvolvida em C utilizando POSIX Threads (pthread), com múltiplas threads compartilhando filas e recursos sincronizados por mutexes e condition variables.

# 🧁 Fábrica de Bolos Multithreads

Simulação de uma linha de produção de bolos desenvolvida em **C**, utilizando **POSIX Threads (`pthread`)**.

O projeto simula trabalhadores executando diferentes etapas da produção simultaneamente, compartilhando recursos e filas que precisam ser sincronizados para evitar condições de corrida.

## ⚙️ Funcionamento

A produção é dividida entre diferentes trabalhadores:

```text
                    ┌─────────────────────┐
                    │       MAIN          │
                    │  Cria os pedidos    │
                    └──────────┬──────────┘
                               │
                               ▼
                    ┌─────────────────────┐
                    │     Trabalhador 1   │
                    │ Prepara as misturas │
                    └──────────┬──────────┘
                               │
                       Fila de assagem
                               │
                               ▼
                    ┌─────────────────────┐
                    │     Trabalhador 2   │
                    │    Assa os bolos    │
                    └──────────┬──────────┘
                               │
                        Fila de assados
                               │
                               ▼
                    ┌─────────────────────┐
                    │     Trabalhador 3   │
                    │ Embala e despacha   │
                    └──────────┬──────────┘
                               │
                               ▼
                       Bolos finalizados
```

### Trabalhadores

* **Trabalhador 1** — recebe os pedidos e prepara as misturas.
* **Trabalhador 2** — recebe as misturas e assa os bolos.
* **Trabalhador 3** — embala e despacha os bolos.
* **Thread `main`** — inicia a produção, acompanha a execução e coordena o encerramento.

## 🧵 Concorrência

As threads trabalham simultaneamente e compartilham estruturas de dados.

Para controlar o acesso aos recursos compartilhados, o projeto utiliza:

* `pthread_create`
* `pthread_join`
* `pthread_mutex_lock`
* `pthread_mutex_trylock`
* `pthread_mutex_unlock`
* `pthread_cond_wait`
* `pthread_cond_signal`
* `pthread_cond_broadcast`

As filas possuem mutexes próprios para controlar o acesso concorrente.

Uma **variavel condicional** é utilizada para permitir que o Trabalhador 3 aguarde enquanto não existem bolos disponíveis para embalagem, evitando que ele fique executando continuamente enquanto não há trabalho.

## 🗂️ Estrutura de dados

O projeto possui uma implementação própria de **fila encadeada**, utilizada para transportar os bolos entre as diferentes etapas da produção.

A fila suporta duas formas de armazenamento:

* **`ENFILEIRAR_DADO_APONTADO`** — aloca memória e copia o conteúdo fornecido.
* **`ENFILEIRAR_PONTEIRO`** — armazena o ponteiro recebido, permitindo a transferência de ownership entre as etapas.

Essa abordagem permite que um bolo percorra toda a linha de produção utilizando o mesmo bloco de memória, sendo liberado quando deixa de ser necessário.

## 🧠 Gerenciamento de memória

Como o projeto é desenvolvido em C, o gerenciamento de memória é realizado manualmente.

Durante o desenvolvimento foram realizados testes com **Valgrind** para verificar:

* memory leaks;
* blocos de memória ainda alocados;
* erros de acesso à memória;
* liberação correta dos recursos.

Exemplo de resultado:

```text
[ NOTA ] - Trabalhador2: não há bolos para assar
[ NOTA ] - Trabalhador2: não há bolos para assar
[ NOTA ] - Trabalhador2: não há bolos para assar
[ NOTA ] - Trabalhador2: não há bolos para assar

[ NOTA ] - Trabalhador2: não há bolos para assar
[ NOTA ] - Trabalhador2: não há bolos para assar
[ NOTA ] - Trabalhador2: encerramento idenficado. saindo
[ NOTA ] - Trabalhador1: não há bolos pedidos de bolo para misturar
[ NOTA ] - Trabalhador1: encerramento idenficado. saindo
[ NOTA ] - Trabalhador3: encerramento idenficado. saindo
==11154== 
==11154== HEAP SUMMARY:
==11154==     in use at exit: 0 bytes in 0 blocks
==11154==   total heap usage: 5,007 allocs, 5,007 frees, 118,896 bytes allocated
==11154== 
==11154== All heap blocks were freed -- no leaks are possible
==11154== 
==11154== For lists of detected and suppressed errors, rerun with: -s
==11154== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
thermius@arch: 

```

Também foram realizados testes com uma produção maior para verificar o comportamento do gerenciamento de memória sob uma quantidade significativa de elementos.

## 🛑 Encerramento das threads

O programa possui um mecanismo de encerramento controlado.

A thread `main` altera um controlador global e sinaliza a condition variable para que o Trabalhador 3 possa sair de seu estado de espera.

Cada trabalhador verifica o controlador e encerra sua execução de maneira controlada.

Depois disso, a `main` utiliza `pthread_join()` para aguardar o encerramento das threads antes de destruir os mutexes, condition variables e filas.

## 📚 Referência

O projeto utiliza como referência os conceitos apresentados em:

**Modern Operating Systems — Andrew S. Tanenbaum**

Principalmente os conceitos relacionados a:

* processos e threads;
* comunicação e sincronização;
* mutexes;
* condition variables;
* concorrência;
* problemas relacionados ao compartilhamento de recursos.

## 🔨 Compilação

Para compilar:

```bash
gcc main.c -o fabrica -pthread
```

Executar:

```bash
./fabrica
```

## 🔍 Testando com Valgrind

Para executar a verificação de memória:

```bash
valgrind --leak-check=full --show-leak-kinds=all ./fabrica
```

## 👨‍💻 Tecnologias

* **C**
* **POSIX Threads (`pthread`)**
* **Mutex**
* **Condition Variables**
* **Estruturas de dados**
* **Gerenciamento manual de memória**
* **Valgrind**
* **Linux**
