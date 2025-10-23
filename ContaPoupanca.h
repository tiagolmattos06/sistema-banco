#ifndef CONTAPOUPANCA_H
#define CONTAPOUPANCA_H
using namespace std;

#include "Conta.h"

class ContaPoupanca : public Conta {
private:
    double taxaRendimento; // em percentual

public:
    ContaPoupanca(int numero, string titular, double saldoInicial, double taxaRendimento);

    bool sacar(double valor) override; // implementação do saque
    void renderJuros(); // rendimento 
    void exibir() const override; // informações
};

#endif
