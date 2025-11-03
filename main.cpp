#include <iostream>
#include <string>
#include <limits>
#include "ContaCorrente.h"
#include "ContaPoupanca.h"
#include "GerenciadorBD.h"
using namespace std;

void pressEnter() {
    cout << "\n\nPressione ENTER para continuar..." << std::flush;

    // Limpa o buffer de entrada de qualquer '\n' que sobrou do 'cin' anterior
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Espera o usuário pressionar ENTER (lê o novo '\n' e o descarta)
    cin.get();
}

void rodarAppPrincipal(GerenciadorBD& gerenciador, const std::string& usuarioLogado) {
    
    ContaCorrente c1(0, "", 0.0, "", 0.0);
    ContaPoupanca p1(0, "", 0.0, "", 0.0);

    // Carrega os dados reais do BD para dentro de c1 e p1
    cout << "\nCarregando contas do usuário " << usuarioLogado << "..." << endl;
    if (!gerenciador.carregarContasDoUsuario(usuarioLogado, c1, p1)) {
        cout << "Erro fatal: não foi possível carregar as contas do usuário." << endl;
        return;
    }

    //  Inicia o Menu
    int num_op;
    double val_op;
    int conta_ativa = 1; // 1=CC, 2=CP

    cout << "\n=== App Principal do Banco ===" << endl;
    cout << "Bem-vindo, " << c1.getTitular() << "!" << endl;

    // --- Menu seleção de conta ---
    while (true) {
        cout << "\n--- Menu Principal ---" << endl;
        if (conta_ativa == 1) {
            cout << ">> CONTA ATIVA: CORRENTE <<" << endl;
            c1.exibir();
        }
        else {
            cout << ">> CONTA ATIVA: POUPANÇA <<" << endl;
            p1.exibir();
        }
        cout << "-----------------------------------" << endl;
        cout << "(1) Exibir Saldo da Conta Ativa" << endl;
        cout << "(2) Depositar na Conta Ativa" << endl;
        cout << "(3) Sacar da Conta Ativa" << endl;
        if (conta_ativa==1) {
            cout << "(4) Trocar para Conta Poupança" << endl;
        }
        else {
            cout << "(4) Trocar para Conta Corrente" << endl;
        }
        cout << "(5) Sair e Salvar" << endl;
        cout << "Escolha uma opção: ";
        cin >> num_op;

        // Limpar buffer
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Opção inválida." << endl;
            pressEnter();
            continue;
        }

        // Processa a conta ativa (CC ou CP)
        Conta* contaAtual = nullptr; // Primeiro, inicialize o ponteiro
        if (conta_ativa == 1) {
            contaAtual = &c1; // Aqui a conversão para Conta* é implícita e válida
        } else {
            contaAtual = &p1; // Aqui também
        }

        if (num_op == 1) {
            contaAtual->exibir();
            pressEnter();
        }
        else if (num_op == 2) {
            cout << "Digite o valor que deseja depositar: ";
            cin >> val_op;
            contaAtual->depositar(val_op);
            pressEnter();

        }
        else if (num_op == 3) {
            cout << "Digite o valor que deseja Sacar: ";
            cin >> val_op;
            contaAtual->sacar(val_op);
            pressEnter();
        }
        else if (num_op == 4) {
            if (conta_ativa == 1) {
                conta_ativa = 2; // Troca para Poupança
            }
            else {
                conta_ativa = 1;
            }

        }
        else if (num_op == 5) {
            break; // Sai do loop while
        }
    }

    // Ao sair do loop, salva o estado final de AMBAS as contas
    cout << "Salvando dados das contas..." << endl;
    gerenciador.salvarConta(c1);
    gerenciador.salvarConta(p1);
    
    cout << "Sessão do banco encerrada." << endl;
}


// --- Ponto de entrada MAIN ---
int main() {
    GerenciadorBD gerenciador("banco_contas.db");

    int escolha = 0;
    string usuario, senha;
    bool loginEfetuado = false;
    string usuarioLogado = ""; // Armazena quem fez o login

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

        if (escolha == 1) { // Login
            cout << "Login: "; cin >> usuario;
            cout << "Senha: "; cin >> senha;
            
            if (gerenciador.verificarLogin(usuario, senha)) {
                loginEfetuado = true;
                usuarioLogado = usuario; // Armazena quem logou
            }

        } else if (escolha == 2) { // Registrar
            string nomeCompleto;
            double saldoCC, saldoCP;
            
            cout << "Login: "; cin >> usuario;
            cout << "Nome Completo (para o titular): ";
            getline(cin, nomeCompleto); // Lê a linha inteira (ex: "Tiago Mattos")
            
            // Limpa o 'Enter' pendente antes de ler o nome completo
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Crie uma senha numérica de 6 dígitos: "; cin >> senha;
            
            cout << "Depósito Inicial na Conta Corrente: R$"; cin >> saldoCC;
            cout << "Depósito Inicial na Conta Poupança: R$"; cin >> saldoCP;

            gerenciador.registrarUsuario(usuario, senha, nomeCompleto, saldoCC, saldoCP);

        } else if (escolha == 3) {
            cout << "Saindo..." << endl;
            return 0;
        }
    }

    // Se saiu do loop, o login foi efetuado
    rodarAppPrincipal(gerenciador, usuarioLogado);
    
    cout << "Programa encerrado." << endl;
    return 0;
}

