#include "ContaCorrente.h"
#include <iostream>
using namespace std;

ContaCorrente::ContaCorrente(int numero, string titular, double saldoInicial, double taxa)
    : Conta(numero, titular, saldoInicial) // chama o construtor da base "Conta"
{
    this->taxaManutencao = taxa;
}

//saque implementado
bool ContaCorrente::sacar(double valor) {
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

// metodo específico da conta corrente
void ContaCorrente::cobrarTaxaMensal() {
    saldo -= taxaManutencao;
    cout << "Taxa mensal de R$" << taxaManutencao << " cobrada.\n";
}

// exibe informações
void ContaCorrente::exibir() const {
    Conta::exibir();
    cout << "Taxa de manutenção: R$" << taxaManutencao << endl;
}
