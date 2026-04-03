## Objetivo
Esse projeto tem como objetivo aprimorar a compreensão acerca de conceitos de Bases de Dados e Organização de Arquivos, assim como a implementação por trás das funções principais de SQL usando a linguagem C.

## Explicação do projeto
Obter dados de um arquivo de entrada e gerar um arquivo binário com esses dados, bem como realizar operações de busca, inserção, remoção e atualização.

## Tema
Armazenar e recuperar dados relacionados às estações e linhas do metrô e da CPTM (Compainha Paulista de Trens Metropolitanos) da região metropolitana da cidade de São Paulo (SP).
Um exemplo de uso desses dados é: "Eu preciso pegar uma linha de metrô para ir para o Aeroporto de Guarulhos." Os dados manipulados indicam o trajeto que deve ser feito.

Em detalhes, os dados se referem às estações, às linhas, às distâncias entre as estações e aos trajetos.
Em Bases de Dados, é normalizado que esses dados devem ser armazenados em diferentes arquivos, de acordo com a normalização realizada.
Entretanto, como o projeto está sendo implementado na linguagem C e por fins de simplificação, todos os dados serão armazenados em apenas um arquivo.
Isso significa que existe redundância dos dados, ou seja, o mesmo valor de dados é armazenado mais do que uma vez.
