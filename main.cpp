#include <iostream>
#include "ContaCorrente.h"
#include "ContaPoupanca.h"
using namespace std;

int main() {
    ContaCorrente c1(101, "Tiago Mattos", 1000.0, 15.0);
    ContaPoupanca p1(202, "Maria Silva", 5000.0, 0.02); // 2% de rendimento

    cout << "\n=== Conta Corrente ===\n";
    c1.exibir();
    c1.depositar(200);
    c1.sacar(300);
    c1.cobrarTaxaMensal();
    c1.exibir();

    cout << "\n=== Conta Poupança ===\n";
    p1.exibir();
    p1.renderJuros();
    p1.sacar(600);
    p1.exibir();

    return 0;
}
