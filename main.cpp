#include <iostream>
#include <string>
#include <limits> 
#include "ContaCorrente.h"
#include "ContaPoupanca.h"
#include "GerenciadorBD.h"
using namespace std;


void rodarAppPrincipal(GerenciadorBD& gerenciador) {
    
    ContaCorrente c1(101, "Tiago Mattos", 1000.0, 15.0);
    ContaPoupanca p1(202, "Maria Silva", 5000.0, 0.02);

    
    cout << "\nCarregando contas do banco de dados..." << endl;
    gerenciador.carregarConta(c1);
    gerenciador.carregarConta(p1);

    
    int num_op;
    double val_op;

    cout << "\n=== App Principal do Banco ===";
    cout << "\n=== Conta Corrente ===";
    c1.exibir(); 
    cout << "Selecione a operação que deseja realizar:\n";
    cout << "(1) Exibir Saldo\n";
    cout << "(2) Depositar\n";
    cout << "(3) Sacar\n";
    cout << "(4) Sair e Salvar\n";
    cin >> num_op;

    while (num_op != 4) {
        if (num_op == 1) {
            c1.exibir();
        }
        else if (num_op == 2) {
            cout << "Digite o valor que deseja depositar: ";
            cin >> val_op;
            c1.depositar(val_op);
        }
        else if (num_op == 3) {
            cout << "Digite o valor que deseja Sacar: ";
            cin >> val_op;
            c1.sacar(val_op);
        }

        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Opção inválida." << endl;
        }

        cout << "\n(1) Saldo, (2) Depositar, (3) Sacar, (4) Sair e Salvar" << endl;
        cin >> num_op;
    }

    
    cout << "Salvando dados das contas..." << endl;
    gerenciador.salvarConta(c1);
    gerenciador.salvarConta(p1);

    cout << "Sessão do banco encerrada." << endl;
}



int main() {
   
    GerenciadorBD gerenciador("banco_contas.db");

    int escolha = 0;
    string usuario, senha;
    bool loginEfetuado = false;

    
    while (!loginEfetuado) {
        cout << "\n--- BEM-VINDO AO SISTEMA BANCÁRIO ---" << endl;
        cout << "1. Fazer Login" << endl;
        cout << "2. Registrar-se" << endl;
        cout << "3. Sair do Programa" << endl;
        cout << "Escolha uma opção: ";

        cin >> escolha;

        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Opção inválida." << endl;
            continue;
        }

        if (escolha == 1) { 
            cout << "Usuário: ";
            cin >> usuario;
            cout << "Senha: ";
            cin >> senha;

            if (gerenciador.verificarLogin(usuario, senha)) {
                loginEfetuado = true; 
            }

        } else if (escolha == 2) { 
            cout << "Novo Usuário: ";
            cin >> usuario;
            cout << "Nova Senha: ";
            cin >> senha;
            gerenciador.registrarUsuario(usuario, senha);

        } else if (escolha == 3) { 
            cout << "Saindo..." << endl;
            return 0; 
        }
    }

    
    rodarAppPrincipal(gerenciador);

    cout << "Programa encerrado." << endl;
    return 0;

}
