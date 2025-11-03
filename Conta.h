#ifndef CONTA_H
#define CONTA_H

#include <string>
using namespace std;

// Classe base
class Conta {
protected:
    int numero;
    string titular;
    double saldo;
    
    friend class GerenciadorBD;
public:

    Conta(int numero, string titular, double saldoInicial);

    virtual ~Conta();

    // metodos de acesso
    int getNumero() const;
    string getTitular() const;
    double getSaldo() const;

    // metodos principais
    virtual void depositar(double valor);
    virtual bool sacar(double valor) = 0;

    virtual void exibir() const; // exibir informações
};

#endif
