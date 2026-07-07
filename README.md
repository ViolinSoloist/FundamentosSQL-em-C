# Projeto de Organização de Arquivos (SCC0215)

## Objetivo
Este projeto consiste na implementação de um gerenciador de arquivos em C, simulando funcionalidades de sistemas de banco de dados relacionais. Ao longo da disciplina, evoluímos de um sistema básico de manipulação de dados para uma estrutura complexa com indexação e algoritmos de junção.

## O que o projeto faz
O sistema gerencia dados de estações e linhas de metrô/CPTM de São Paulo, permitindo as seguintes operações:
- **Trabalho Introdutório:** Leitura de CSV, criação de arquivo binário, busca, inserção e remoção (com reaproveitamento de espaço via pilha de removidos).
- **Trabalho 1:** Indexação dos dados utilizando Árvore-B para buscas rápidas (log(n)).
- **Trabalho 2:** Implementação de operações avançadas de junção (Join) entre arquivos, incluindo:
    - *Nested Loop Join* (força bruta).
    - *Index Join* (usando a Árvore-B).
    - *Sort-Merge Join* (ordenação externa seguida de intercalação).

## Estrutura do Projeto
O código é modularizado para separar a lógica de negócio das operações de baixo nível:
- `funcionalidades/`: comandos principais (SQL-like: CREATE TABLE, SELECT, INSERT, etc).
- `funcionalidades_aux/`: utilitários de busca, manipulação de arquivos e serialização.
- `arvoreB/`: lógica da árvore-B (inserção, remoção, split e busca).
- `juncao/`: algoritmos de join e ordenação.
- `funcoes_fornecidas/`: utilitários básicos da disciplina.

## Como rodar
1. Certifique-se de ter o `gcc` instalado.
2. Utilize o `Makefile` na raiz:
   - Para compilar: `make`
   - Para rodar: `make run`
   - Para limpar os binários: `make clean`
