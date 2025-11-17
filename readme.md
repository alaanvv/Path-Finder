# Path-Finder

Um programa **C** que gerencia rotas entre cidades usando  
conceitos de grafos e matemática discreta.  

- **IMPORTANTE** - Esse programa foi feito usando interface no terminal

Então caso a janela do terminal esteja muito pequena, a visualização pode   
quebrar. Além disso, features como cores são exclusivos de terminais ANSI.

## Como usar

Rode o programa a partir de uma pasta contendo `instancia.txt`  
A primeira linha desse arquivo deve conter a quantidade de pontos  
descrito nele, e as próximas são pares de inteiros descrevendo o  
**x** e **y** dos pontos. Ex:  

```
12
1 5
4 6
7 5
5 4
9 4
2 3
4 2
6 2
1 1
5 1
3 0
9 0
```

A primeira parte do programa é a seleção dos pontos de interesse,  
onde o usuário escolhe a partir dos pontos lidos do arquivo quais  
ele vai querer usar no programa.

()[]

Basta digitar o número do ponto e apertar **ENTER** pra marcá-lo  
Se o ponto já estiver marcado, ele desmarca.  

> Os pontos marcados ficam destacados na visualização.

()[]

Após selecionar os pontos de interesse desejados, use o comando  
`run` pra iniciar o gerenciamento de rotas.  

()[]

Nessa tela, apenas os pontos de interesse são mostrados, e temos  
acesso aos comandos do programa.  

- `dist` - Calcula matriz de distâncias
Após ver a matriz, aperte **ENTER** pra sair.

()[]

- `conn <id1> <id2>` - Conecta/desconecta dois pontos
Digite `conn` e o índice dos pontos que você quer conectar.  
Se você digitar o índice de pontos já conectados eles se desconectam.  
Pontos conectados são mostrados com uma linha entre eles.

()[]

- `routes <id1> <id2>` - Calcula todas rotas entre duas cidades
Digite `routes` e o índice dos pontos que você quer ver as rotas.  
Isso vai mostrar todas as rotas possíveis, uma por vez, basta apertar  
**ENTER** pra passar pra próxima.

()[]

Quando uma rota for a menor possível, será mostrado uma mensagem na  
frente da distância total.  

()[]

- `count <n> <r>` - Calcula combinações e permutações
Digite `count` seguido dos valores `n` e `r`.
Mostra:
  - nPr (permutações de n tomados r)
  - nCr (combinações de n tomados r)
  - n!  (fatorial de n)
Pra sair da tela, aperte **ENTER**.

()[]

- `count <n> <r>` - Encerra o programa
Fecha o programa

## TODO

- [ ] Gravar uma demo
- [ ] Fazer versões linux e windows
