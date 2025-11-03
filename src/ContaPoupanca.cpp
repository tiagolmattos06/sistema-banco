#include "../include/ContaPoupanca.h"
#include <iostream>

// Construtor
ContaPoupanca::ContaPoupanca(int numero, string titular, double saldoInicial, string username_dono, double taxaRendimento)
    : Conta(numero, titular, saldoInicial, username_dono), // Chama o construtor da classe base
      taxaRendimento(taxaRendimento) {
    // Construtor da classe filha
}

// Métodos 
bool ContaPoupanca::sacar(double valor) {
    if (valor <= 0) {
        std::cout << "Valor de saque inválido." << std::endl;
        return false;
    }
    if (saldo >= valor) {
        saldo -= valor;
        std::cout << "Saque de R$" << valor << " realizado. Saldo restante: R$" << saldo << std::endl;
        return true;
    } else {
        std::cout << "Saldo insuficiente. Saldo atual: R$" << saldo << std::endl;
        return false;
    }
}

void ContaPoupanca::renderJuros() {
    double juros = saldo * taxaRendimento;
    saldo += juros;
    std::cout << "Rendimento de R$" << juros << " aplicado. Novo saldo: R$" << saldo << std::endl;
}

void ContaPoupanca::exibir() const {
    std::cout << "--- CONTA POUPANÇA ---" << std::endl;
    Conta::exibir(); // Chama o exibir da classe base
    std::cout << " | Rendimento: " << (taxaRendimento * 100) << "%" << std::endl;
}
