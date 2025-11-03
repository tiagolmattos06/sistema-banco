
#ifndef CONTACORRENTE_H
#define CONTACORRENTE_H

#include "Conta.h"

class ContaCorrente : public Conta {
private:
    double taxaManutencao;

    friend class GerenciadorBD;
public:
    ContaCorrente(int numero, string titular, double saldoInicial, double taxaManutencao);

    
    bool sacar(double valor) override; //sacar da classe base

    
    void cobrarTaxaMensal(); //método específico da conta corrente

    //exibir informações
    void exibir() const override;
};

#endif
