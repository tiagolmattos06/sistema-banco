#include <iostream>
#include <string>
#include <limits>
#include "ContaCorrente.h"
#include "ContaPoupanca.h"
#include "GerenciadorBD.h"
using namespace std;

// Função auxiliar para pausar a tela
void pressEnter() {
    cout << "\n\nPressione ENTER para continuar..." << std::flush;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void rodarAppPrincipal(GerenciadorBD& gerenciador, const std::string& usuarioLogado) {

    
    ContaCorrente c1(0, "", 0.0, "", 0.0);
    ContaPoupanca p1(0, "", 0.0, "", 0.0);

    // 2. Carrega os dados reais do BD
    cout << "\nCarregando contas do usuário " << usuarioLogado << "..." << endl;
    if (!gerenciador.carregarContasDoUsuario(usuarioLogado, c1, p1)) {
        cout << "Erro fatal: não foi possível carregar as contas do usuário." << endl;
        pressEnter();
        return;
    }

    // 3. Inicia o Menu
    int num_op;
    double val_op;
    int conta_ativa = 1; // 1=CC, 2=CP

    bool encerrarSessao = false;
    bool contaExcluida = false; // Flag para não salvar se a conta for excluída

    cout << "\n=== App Principal do Banco ===" << endl;
    cout << "Bem-vindo(a), " << c1.getTitular() << "!" << endl;

    // --- Menu seleção de conta ---
    while (!encerrarSessao) { 
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
        if (conta_ativa == 1) {
            cout << "(4) Trocar para Conta Poupança" << endl;
        }
        else {
            cout << "(4) Trocar para Conta Corrente" << endl;
        }
        cout << "(5) Sair e Salvar" << endl;
        cout << "(7) EXCLUIR MINHA CONTA " << endl; // <-- O menu diz 7
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
        Conta* contaAtual = nullptr;
        if (conta_ativa == 1) {
            contaAtual = &c1;
        } else {
            contaAtual = &p1;
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
                cout << "Conta ativa: Poupança." << endl;
            }
            else {
                conta_ativa = 1;
                cout << "Conta ativa: Corrente." << endl;
            }
        }
        else if (num_op == 5) { // Sair e Salvar
            encerrarSessao = true; // <-- CORREÇÃO: Marca a flag para sair
        }
        else if (num_op == 7) { // <-- CORREÇÃO: O número deve ser 7 (não 6)
            char confirmacao;
            cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!! ATENÇÃO !!!!!!!!!!!!!!!!!!!!!!!!!!\n";
            cout << "Você tem CERTEZA que deseja excluir seu usuário (" << usuarioLogado << ")?\n";
            cout << "TODAS as suas contas (Corrente e Poupança) serão apagadas.\n";
            cout << "Esta ação é IRREVERSÍVEL.\n";
            cout << "Digite 'S' para confirmar ou 'N' para cancelar: ";
            cin >> confirmacao;

            if (confirmacao == 'S' || confirmacao == 's') {
                if (gerenciador.excluirUsuario(usuarioLogado)) {
                    cout << "Usuário e contas excluídos com sucesso." << endl;
                    encerrarSessao = true; // Encerra a sessão
                    contaExcluida = true;  // Marca que não deve salvar
                    pressEnter();
                } else {
                    cout << "Erro: Não foi possível excluir o usuário." << endl;
                    pressEnter();
                }
            } else {
                cout << "Exclusão cancelada." << endl;
                pressEnter();
            }
        }
        else {
             cout << "Opção inválida." << endl;
             pressEnter();
        }
    } // Fim do loop while (!encerrarSessao)

    

    // 4. Lógica de Saída (Salvar se a conta NÃO foi excluída)
    if (!contaExcluida) {
        cout << "Salvando dados das contas..." << endl;
        gerenciador.salvarConta(c1);
        gerenciador.salvarConta(p1);
    }
    cout << "Sessão do banco encerrada." << endl;
    pressEnter();
} // Fim da função rodarAppPrincipal


int main() {
    GerenciadorBD gerenciador("banco_contas.db");

    // Loop principal do programa
    while (true) {
        int escolha = 0;
        string usuario, senha;
        bool loginEfetuado = false;
        string usuarioLogado = ""; // Armazena quem fez o login

        // Loop de Login/Cadastro
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
                } else {
                    pressEnter(); // Pausa se o login falhar
                }

            } else if (escolha == 2) { // Registrar
                string nomeCompleto;
                double saldoCC, saldoCP;

                cout << "Novo Login: "; cin >> usuario;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Nome Completo (para o titular): ";
                getline(cin, nomeCompleto); // Lê a linha inteira (ex: "Tiago Mattos")

                cout << "Crie uma senha: "; cin >> senha;
                cout << "Depósito Inicial na Conta Corrente: R$"; cin >> saldoCC;
                cout << "Depósito Inicial na Conta Poupança: R$"; cin >> saldoCP;

                gerenciador.registrarUsuario(usuario, senha, nomeCompleto, saldoCC, saldoCP);
                pressEnter();

            } else if (escolha == 3) {
                cout << "Saindo..." << endl;
                return 0; // Encerra o programa
            }
        }

        rodarAppPrincipal(gerenciador, usuarioLogado);
    }

    return 0;
}
