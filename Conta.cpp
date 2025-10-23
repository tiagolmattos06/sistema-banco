#include "Conta.h"
#include <iostream>
using namespace std;


Conta::Conta(int numero, string titular, double saldoInicial) {
    this->numero = numero;
    this->titular = titular;
    this->saldo = saldoInicial;
}

Conta::~Conta() {

}


int Conta::getNumero() const {
    return numero;
}

string Conta::getTitular() const {
    return titular;
}

double Conta::getSaldo() const {
    return saldo;
}

// depósito, todo tipo de conta vai ter
void Conta::depositar(double valor) {
    if (valor > 0) {
        saldo += valor;
        cout << "Depósito de R$" << valor << " realizado com sucesso.\n";
    } else {
        cout << "Valor inválido para depósito.\n";
    }
}

// exibir na tela informações
void Conta::exibir() const {
    cout << "-----------------------------\n";
    cout << "Conta nº: " << numero << endl;
    cout << "Titular: " << titular << endl;
    cout << "Saldo: R$" << saldo << endl;
    cout << "-----------------------------\n";
}
