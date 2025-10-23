#include "ContaPoupanca.h"
#include <iostream>
using namespace std;

ContaPoupanca::ContaPoupanca(int numero, string titular, double saldoInicial, double taxa) // controi a partir da base "Conta"
    : Conta(numero, titular, saldoInicial) {
    this->taxaRendimento = taxa;
}

bool ContaPoupanca::sacar(double valor) {
    if (valor <= 0) {
        cout << "Valor inválido para saque.\n";
        return false;
    }

    if (valor > saldo) {
        cout << "Saldo insuficiente.\n";
        return false;
    }

    saldo -= valor;
    cout << "Saque de R$" << valor << " realizado.\n";
    return true;
}

void ContaPoupanca::renderJuros() { //método de rendimento
    double rendimento = saldo * taxaRendimento;
    saldo += rendimento;
    cout << "Rendimento de R$" << rendimento << " aplicado (taxa " << taxaRendimento * 100 << "%).\n";
}

void ContaPoupanca::exibir() const { //exibe 
    Conta::exibir(); //usa método exbir
    cout << "Taxa de rendimento: " << taxaRendimento * 100 << "%\n";
}
