# Sistema de Gerenciamento de Dados de Transporte (Metrô/CPTM)

## Visão Geral
Este sistema, desenvolvido em C, gerencia dados de estações e linhas da rede de transporte metropolitano de São Paulo. O software é capaz de processar arquivos binários, realizar consultas complexas, manipular registros com reaproveitamento de espaço e indexar dados para otimizar a performance.

## Funcionalidades
- **Manipulação de Dados:** Importação de arquivos CSV, persistência em binários e operações de leitura, escrita e atualização de registros.
- **Gerenciamento de Espaço:** Implementação de pilha de registros logicamente removidos para otimização de espaço em disco.
- **Indexação:** Estrutura de Árvore-B (B-Tree) integrada para realizar buscas eficientes de estações baseadas em códigos identificadores.
- **Consultas Relacionais:** Implementação de operações de Junção (Join) para combinar dados entre arquivos, incluindo:
    - *Nested Loop Join* (força bruta).
    - *Index Join* (usando índices de Árvore-B).
    - *Sort-Merge Join* (intercalação de arquivos ordenados).

## Contexto dos Dados
O projeto utiliza informações sobre estações, linhas, distâncias e integrações da rede de transporte de São Paulo. Embora modelos de banco de dados recomendem a normalização em diversos arquivos, esta implementação concentra os dados para fins de eficiência na manipulação em baixo nível, lidando com redundâncias inerentes à estrutura escolhida.

## Como utilizar
- `make` para compilar o projeto.
- `make run` para iniciar a execução.
- `make clean` para remover arquivos temporários e binários.
