#include "Conta.h"
#include <iostream>


Conta::Conta(int numero, string titular, double saldoInicial, string username_dono)
    : numero(numero), titular(titular), saldo(saldoInicial), username_dono(username_dono) {
    // Construtor da classe base
}

// Destrutor
Conta::~Conta() {
}

int Conta::getNumero() const { return numero; }
string Conta::getTitular() const { return titular; }
double Conta::getSaldo() const { return saldo; }

// Métodos 
void Conta::depositar(double valor) {
    if (valor > 0) {
        saldo += valor;
        std::cout << "Depósito de R$" << valor << " realizado. Novo saldo: R$" << saldo << std::endl;
    } else {
        std::cout << "Valor de depósito inválido." << std::endl;
    }
}

void Conta::exibir() const {
    std::cout << "Titular: " << titular << " | Número: " << numero << " | Saldo: R$" << saldo;
}
