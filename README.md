
# CI1338 - Geometria Computacional
## INFO7061 - Tópicos em Geometria Computacional
### Primeiro Trabalho

## 1. Introdução
O trabalho consiste em implementar uma solução para o problema de classificação de polígonos e pontos interiores.

### Definição do problema:
- **Entrada**: Um conjunto **S** de linhas poligonais fechadas no plano e um conjunto **P** de pontos no plano.
- **Objetivo**:
  - Classificar cada polígono como: 
    - "não simples",
    - "simples e não convexos", 
    - "simples e convexos".
  - Para cada ponto **P** ∈ **P**, determinar quais polígonos simples (convexos ou não) contêm o ponto **P**.

## 2. Resolução do Problema
A resolução do problema envolve a implementação de dois componentes principais:

### 2.1 Classificação de Polígonos
O algoritmo deve classificar cada polígono dado como:
- **Polígono não simples**: Se o polígono tiver interseções em seus lados, ou seja, não for possível desenhá-lo sem cruzar linhas.
- **Polígono simples e não convexo**: Se o polígono for simples (não possui interseções) mas não é convexo, ou seja, possui um ou mais ângulos internos maiores que 180°.
- **Polígono simples e convexo**: Se o polígono for simples e todos os seus ângulos internos forem menores que 180°, caracterizando-o como convexo.

### 2.2 Determinação de Pontos Dentro de Polígonos
O algoritmo também deve verificar, para cada ponto **P** ∈ **P**, se ele está dentro de algum polígono simples (convexo ou não). Para isso, deve-se utilizar um método de teste de inclusão de ponto, como o algoritmo de Ray-Casting ou o de ângulos, verificando se o ponto está dentro dos limites de cada polígono.

## 3. Algoritmos
A implementação dos algoritmos é realizada da seguinte forma:

### 3.1 Algoritmo de Classificação de Polígono
1. **Verificação de Simplicidade**: O algoritmo percorre cada polígono e verifica se há interseções entre os lados. Para isso, é necessário verificar todas as arestas e se elas se cruzam entre si.
2. **Verificação de Convexidade**: Uma vez que o polígono é classificado como simples, é feita a verificação de convexidade. Isso é feito verificando se todos os ângulos internos são menores que 180°.
3. **Resultado**: Cada polígono será classificado como "não simples", "simples e não convexo", ou "simples e convexo" dependendo dos resultados das verificações anteriores.

### 3.2 Algoritmo de Inclusão de Ponto
1. **Ray-Casting ou Algoritmo de Ângulos**: O algoritmo escolhe uma linha imaginária partindo do ponto e verifica o número de interseções que essa linha faz com os lados do polígono. Se o número de interseções for ímpar, o ponto está dentro do polígono. Caso contrário, o ponto está fora.
2. **Resultado**: Para cada ponto, a lista de polígonos que contêm o ponto será gerada e exibida.

## 4. Exemplos

### Exemplo de Entrada:
```
3 3
4
1 4
15 4
15 20
1 20
3
8 12
25 8
25 18
5
2 6
2 1
15 2
20 1
20 6
12 12
25 2
5 5
```

### Exemplo de Saída:
```
1 simples e convexo
2 simples e convexo
3 simples e nao convexo
1:1 2
2:
3:1 3
```

## 5. Conclusão
A implementação proposta resolve o problema de forma eficiente, classificando corretamente os polígonos e determinando os pontos contidos dentro deles. A utilização dos algoritmos de Ray-Casting e de verificação de interseções permite a identificação precisa das categorias dos polígonos e a localização dos pontos.

