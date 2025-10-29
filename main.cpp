#include <iostream>
#include "ContaCorrente.h"
#include "ContaPoupanca.h"
using namespace std;

int main() {
    ContaCorrente c1(101, "Tiago Mattos", 1000.0, 15.0);
    ContaPoupanca p1(202, "Maria Silva", 5000.0, 0.02); // 2% de rendimento

    int num_op;
    double val_op;

    cout << "\n=== Conta Corrente ===\n";
    cout << "Selecione a operação que deseja realizar digitando o número correspondente:\n";
    cout << "(1) Exibir Saldo\n";
    cout << "(2) Depositar\n";
    cout << "(3) Sacar\n";
    cin >> num_op;

    if (num_op == 1) {
        c1.exibir();
    }
    else if (num_op == 2) {
        cout << "Digite o valor que deseja depositar: ";
        cin>>val_op;
        c1.depositar(val_op);
    }
    else if (num_op == 3) {
        cout << "Digite o valor que deseja Sacar: ";
        cin>>val_op;
        c1.sacar(val_op);
    }

    return 0;
}
