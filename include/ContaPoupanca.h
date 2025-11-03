#ifndef CONTAPOUPANCA_H
#define CONTAPOUPANCA_H
using namespace std;

#include "Conta.h"

class ContaPoupanca : public Conta {
private:
    double taxaRendimento; // em percentual



    friend class GerenciadorBD;
public:
    ContaPoupanca(int numero, string titular, double saldoInicial, string username_dono, double taxaRendimento);

    bool sacar(double valor) override; // implementação do saqu

    void renderJuros(); // rendimento
    void exibir() const override; // informações
};

#endif

