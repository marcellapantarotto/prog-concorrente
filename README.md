# Programação Concorrente

Estudos dirigidos e atividades da disciplina de Programação Concorrente da Universidade de Brasília (UnB)

Para compilar qualquer dos programas abaixo, utilizar o seguinte comando: <br>
```gcc -pthread programa.c -o programa``` <br>
Para executar: ```./programa```

#### ORDEM:

1. ```tarefa3.c``` <br>
Começando a trabalhar com threads e a biblioteca pthreads.h

2. ```leitores_escritores_mux.c``` <br>
Problema clássico de leitores e escritores, resolvido com locks.

3. ```macacos_mutex.c``` <br>
Problema clássico dos macacos, resovido com locks.

4. ```macacos_gorilas.c``` <br>
Problema dos macacos, em que os gorilas têm preferência de passagem. Resolvido com locks e variáveis condicionais.

5. ```produtor_consumidor.c``` <br>
Problema clássico de produtores e consumidores de dados de um buffer compartilhado. Resolvido com locks e variáveis condicionais.

6. ```canibais.c``` <br>
Canibais competindo por porções de comida e um cozinheiro que utiliza a região crítica para cozinha e só a libera após terminar todas as porções, problema resolvido com locks recursivos. <br>
Para executar: <br>
```./canibais numero_canibais numero_porções```