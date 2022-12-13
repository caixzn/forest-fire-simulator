# Florestal fires detector simulator.
Nowadays, one of the great challenges for countries that have a vast forest region is to deal with the high occurrence of forest fires, whether criminally or naturally caused, such as those observed in the Amazon and the Brazilian cerrado. Therefore, the detection of fire outbreaks is of great importance to protect the fauna, flora and, in many cases, the lives of many people.

## Authors
* Name: Matheus Loiola Pinto Curado Silva
* Matriculation: 211057600016
* Name: Caio Padilha Aguiar
* Matriculation: 211057600003
* Name: Leonam Knupp
* Matriculation: 211057600001
* Sistemas Operacionais 1 - Class A

## Desenvolvimento e estrutura do projeto.

Primeirante, ao desenvolver o projeto, pensamos em como faríamos a comunicação entre os sensores, visto que, com isso, teríamos uma ideia de quais atributos precisaríamos implementar na thread central e na thread bombeiro, facilitando o trabalho a longo prazo. Assim, decidimos optar pelo método produtor-consumidor para armazenar as mensagens, pois seria o mais viável de implementar. Dessa forma, quando algum sensor acha um fogo em seus arredores, ele o marca como visitado, para impedir uma detecção duplicada, e cria uma mensagem, que possui como atributos o seu ID, a posição do fogo, um mapa de visitados e outros, e a coloca em sua fila para envio. Após verificar sua área por mais fogos, ele envia as mensagens de sua fila para os sensores vizinhos, sempre verificando se ainda existe um sensor ali e se a mensagem ainda não passou por ele. Isso se repete até a mensagem chegar nos sensores de borda, os quais enviarão a mensagem à thread central, que irá verificar se essa mensagem já não foi vista (por meio de uma árvore AVL com os IDs) e registrá-la no log, além de chamar a thread bombeiro, que irá apagar o fogo.

## Prerequisites
Before starting, it is essential to have installed the `GNU make` tool and the compiler `gcc`, in order to compile the files.

## Running the program

```bash
# Create the main file
$ make

# Execute the simulator
$ ./bin/main

# Remove the files created during the compilation
$ make clean
```
