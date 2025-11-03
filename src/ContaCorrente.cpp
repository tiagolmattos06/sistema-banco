#include "../include/ContaCorrente.h"
#include <iostream>

// Construtor 
ContaCorrente::ContaCorrente(int numero, string titular, double saldoInicial, string username_dono, double taxaManutencao)
    : Conta(numero, titular, saldoInicial, username_dono), // Chama o construtor da classe base
      taxaManutencao(taxaManutencao) {
    // Construtor da classe filha
}

// Métodos 
bool ContaCorrente::sacar(double valor) {
    double valorTotal = valor + taxaManutencao;
    if (valor <= 0) {
        std::cout << "Valor de saque inválido." << std::endl;
        return false;
    }
    if (saldo >= valorTotal) {
        saldo -= valorTotal;
        std::cout << "Saque de R$" << valor << " (taxa de R$" << taxaManutencao << ") realizado. Saldo restante: R$" << saldo << std::endl;
        return true;
    } else {
        std::cout << "Saldo insuficiente (saque + taxa). Saldo atual: R$" << saldo << std::endl;
        return false;
    }
}

void ContaCorrente::cobrarTaxaMensal() {
    saldo -= taxaManutencao;
    std::cout << "Taxa mensal de R$" << taxaManutencao << " cobrada." << std::endl;
}

void ContaCorrente::exibir() const {
    std::cout << "--- CONTA CORRENTE ---" << std::endl;
    Conta::exibir(); // Chama o exibir da classe base
    std::cout << " | Taxa de Operação: R$" << taxaManutencao << std::endl;
}
