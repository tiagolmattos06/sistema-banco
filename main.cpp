#include <iostream>
#include "ContaCorrente.h"
#include "Conta.h"
using namespace std;

int main() {
    ContaCorrente c1(101, "Tiago Mattos", 1000.0, 15.0);

    c1.exibir();
    c1.depositar(200);
    c1.sacar(300);
    c1.cobrarTaxaMensal();
    c1.exibir();

    return 0;
}

