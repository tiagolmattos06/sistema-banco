O sistema de banco divide as contas entre corrente e poupança, possibilita realizar movimentações e mostrar saldos, além de criar uma lista com as contas criadas.
Esse projeto é da diciplina de Estruturas de Dados Orientadas ao Objeto - 2025.2, referente ao curso de Sistemas de Informação da UFPE.

# Sistema de Caixa Eletrônico (C++)

Este projeto é uma simulação de **caixa eletrônico em C++**, desenvolvida como parte de um trabalho acadêmico com foco em **Programação Orientada a Objetos (POO)** e **persistência de dados com SQLite**.

A aplicação permite que o usuário **crie uma conta, faça login e realize operações bancárias básicas**, como consultar saldo, efetuar saques e depósitos — tudo isso em um ambiente de console simples e funcional.

---

## O que o sistema faz

* Criação e autenticação de usuários com armazenamento de senha via *hash* (didático).
* Gerenciamento de múltiplas contas por usuário, podendo alternar entre **Corrente** e **Poupança**.
* Operações bancárias:

  * Consulta de saldo;
  * Depósito em conta;
  * Saque com validação de saldo e regras específicas por tipo de conta.
* Persistência local com SQLite, garantindo que os dados sejam mantidos entre execuções.
* Estrutura modular e orientada a objetos, com separação clara entre interface, lógica e persistência.

---

## Como o sistema é estruturado

O código foi dividido em **três camadas principais**, para manter a organização e facilitar futuras manutenções:

### 1. Interface (Camada de Apresentação)

Arquivo: `main.cpp`
Responsável por exibir os menus, receber as entradas do usuário e controlar o fluxo principal da aplicação.
Principais funções:

* `exibirMenuPrincipal()`
* `rodarAppPrincipal()`

### 2. Lógica de Negócio

Arquivos: `Conta.h`, `ContaCorrente.cpp`, `ContaPoupanca.cpp`
Define as regras do sistema bancário e o comportamento das contas.

Classes principais:

* **Conta (classe base)** – define atributos comuns (saldo, titular, número da conta).
* **ContaCorrente** – aplica taxas de manutenção.
* **ContaPoupanca** – aplica rendimento sobre o saldo.

### 3. Persistência de Dados

Arquivo: `GerenciadorBD.cpp`
Isola completamente a lógica de acesso ao banco de dados, tornando o código mais limpo e seguro.

Responsabilidades:

* Criar e gerenciar o arquivo `banco_contas.db`.
* Converter informações entre o banco e os objetos C++.
* Executar transações seguras e garantir a consistência dos dados.

---

## Pilares de POO em prática

* **Herança:** `ContaCorrente` e `ContaPoupanca` derivam da classe `Conta`.
* **Polimorfismo:** uso de ponteiros da classe base (`Conta*`) para tratar objetos de diferentes tipos de conta de forma uniforme.
* **Encapsulamento:** a classe `GerenciadorBD` esconde toda a complexidade do SQLite.
* **Amizade (friend):** `GerenciadorBD` tem acesso direto aos dados de `Conta`, permitindo salvar e carregar informações com eficiência.

---

## Estrutura do Banco de Dados

Banco: `banco_contas.db`
Tabelas:

* **Usuarios** – guarda credenciais e dados básicos de autenticação.
* **Contas** – armazena informações de saldo, titular e tipo de conta.

As operações são realizadas de forma transacional, garantindo integridade e segurança.

---

## Como executar o projeto

### Pré-requisitos

* Compilador **C++17** ou superior
* Biblioteca **SQLite3** instalada

### Compilação

```bash
g++ main.cpp Conta.cpp ContaCorrente.cpp ContaPoupanca.cpp GerenciadorBD.cpp -lsqlite3 -o caixa
```

### Execução

```bash
./caixa
```

---

## Equipe de Desenvolvimento

| Nome                  | Função                         | GitHub                                               |
| --------------------- | ------------------------------ | ---------------------------------------------------- |
| **Vinicius Oliveira** | UX, Interface e Banco de Dados | [@vinilauter](https://github.com/vinilauter)         |
| **Caíque Machado**    | Persistência de Dados          | [@Caique-Machado](https://github.com/Caique-Machado) |
| **Ricardo Lira**      | Persistência de Dados          | [@rcl4-beep](https://github.com/rcl4-beep)           |
| **Tiago Mattos**      | Lógica de Negócio e POO        | [@tiagolmattos06](https://github.com/tiagolmattos06) |
