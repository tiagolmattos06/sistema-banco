// --- Bibliotecas Padrão ---
#include <iostream>       // Para entrada e saída de console (cin, cout)
#include <string>         // Para usar o tipo de dado std::string
#include <limits>         // Para 'numeric_limits' (usado para limpar o buffer do cin)

// --- Cabeçalhos do Projeto ---
#include "../include/ContaCorrente.h"
#include "../include/ContaPoupanca.h"
#include "../include/GerenciadorBD.h"

// Evita ter que digitar 'std::' antes de cada cout, cin, etc
using namespace std;

/**
 * @brief Pausa a execução do console e aguarda o usuário pressionar ENTER.
 *
 * Esta é uma função de UX para evitar que a
 * tela seja limpa ou que o menu reapareça antes que o usuário
 * possa ler a mensagem da operação anterior.
 *
 * @note Ela usa cin.ignore() primeiro para limpar qualquer Enter ("\n")
 * que tenha sobrado de um cin anterior, garantindo que o
 * cin.get() subsequente realmente pause e espere por uma nova entrada.
 */
void pressEnter() {
    cout << "\n\nPressione ENTER para continuar..." << std::flush;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

/**
 * @brief Executa o loop principal do aplicativo
 * após o usuário ter feito o login com sucesso.
 *
 * @param gerenciador Uma referência (&) ao GerenciadorBD. Passar por referência
 * evita criar uma cópia e garante que seja usado o mesmo
 * gerenciador (e a mesma conexão de banco) da main.
 * @param usuarioLogado O nome de usuário (login) que é usado como chave para
 * buscar as contas corretas no banco de dados.
 */
void rodarAppPrincipal(GerenciadorBD& gerenciador, const std::string& usuarioLogado) {

    // Cria instâncias vazias das contas na stack
    // Elas servirão como recipientes para os dados que virão do banco
    ContaCorrente c1(0, "", 0.0, "", 0.0);
    ContaPoupanca p1(0, "", 0.0, "", 0.0);

    // Carrega os dados do BD
    // O GerenciadorBD é responsável por preencher os objetos c1 e p1 com os dados do banco de dados
    cout << "\nCarregando contas do usuário " << usuarioLogado << "..." << endl;
    if (!gerenciador.carregarContasDoUsuario(usuarioLogado, c1, p1)) {
        cout << "Erro fatal: não foi possível carregar as contas do usuário." << endl;
        pressEnter();
        return; // Sai da função rodarAppPrincipal
    }

    // Inicia o Menu
    int num_op;            // Guarda a escolha do menu de operações (1, 2, 3...)
    double val_op;         // Guarda o valor monetário para saque/depósito
    int conta_ativa = 1;   // Controla qual conta está sendo utilizada (1=CC, 2=CP)

    bool encerrarSessao = false; // Flag para controlar o loop principal do app
    bool contaExcluida = false;  // Flag de segurança para evitar salvar uma conta deletada

    cout << "\n=== App Principal do Banco ===" << endl;
    cout << "Bem-vindo(a), " << c1.getTitular() << "!" << endl;

    // Menu seleção de conta (Loop Principal do App)
    // Este loop while rodará até que a flag encerrarSessao seja true
    while (!encerrarSessao) { 
        cout << "\n--- Menu Principal ---" << endl;
        
        // Mostra o saldo e o tipo da conta que está em foco no momento.
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
        
        // Menu de Opções Dinâmico
        // A opção (4) muda dependendo de qual conta está ativa
        if (conta_ativa == 1) {
            cout << "(4) Trocar para Conta Poupança" << endl;
        }
        else {
            cout << "(4) Trocar para Conta Corrente" << endl;
        }
        cout << "(5) Sair e Salvar" << endl;
        cout << "(6) EXCLUIR MINHA CONTA " << endl;
        cout << "Escolha uma opção: ";
        cin >> num_op;

        // Validação de Entrada
        // Checa se o usuário digitou algo que não é um número (ex: "abc")
        if (cin.fail()) {
            cin.clear(); // Limpa o "estado de falha" do cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descarta a entrada inválida
            cout << "Opção inválida." << endl;
            pressEnter();
            continue; // Pula o resto do loop e volta para o início (reimprime o menu)
        }

        // É criado um ponteiro da classe base Conta
        // Agora, quando for chamado contaAtual->metodo(), o C++ saberá qual versão do método chamar (da Corrente ou da Poupança)
        Conta* contaAtual = nullptr; // Ponteiro Polimórfico
        if (conta_ativa == 1) {
            contaAtual = &c1; // O ponteiro 'Conta' agora aponta para o objeto ContaCorrente c1
        } else {
            contaAtual = &p1; // O ponteiro 'Conta' agora aponta para o objeto ContaPoupanca p1
        }

        // Processamento das Operações
        if (num_op == 1) {
            contaAtual->exibir(); // Chama o método virtual exibir()
            pressEnter();
        }
        else if (num_op == 2) {
            cout << "Digite o valor que deseja depositar: ";
            cin >> val_op;
            contaAtual->depositar(val_op); // Chama depositar()
            pressEnter();
        }
        else if (num_op == 3) {
            cout << "Digite o valor que deseja Sacar: ";
            cin >> val_op;
            contaAtual->sacar(val_op); // Chama sacar()
            pressEnter();
        }
        else if (num_op == 4) {
            // Lógica para alternar a conta ativa
            if (conta_ativa == 1) {
                conta_ativa = 2; // Troca para Poupança
                cout << "Conta ativa: Poupança." << endl;
            }
            else {
                conta_ativa = 1; // Troca para Corrente
                cout << "Conta ativa: Corrente." << endl;
            }
        }
        else if (num_op == 5) { // Sair e Salvar
            encerrarSessao = true; // Marca a flag para sair do loop 'while'
        }
        else if (num_op == 6) { // Excluir Conta
            char confirmacao;
            cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!! ATENÇÃO !!!!!!!!!!!!!!!!!!!!!!!!!!\n";
            cout << "Você tem CERTEZA que deseja excluir seu usuário (" << usuarioLogado << ")?\n";
            cout << "TODAS as suas contas (Corrente e Poupança) serão apagadas.\n";
            cout << "Esta ação é IRREVERSÍVEL.\n";
            cout << "Digite 'S' para confirmar ou 'N' para cancelar: ";
            cin >> confirmacao;

            if (confirmacao == 'S' || confirmacao == 's') {
                // atribui a lógica de exclusão para o GerenciadorBD
                if (gerenciador.excluirUsuario(usuarioLogado)) {
                    cout << "Usuário e contas excluídos com sucesso." << endl;
                    encerrarSessao = true; // Força a saída do loop principal
                    contaExcluida = true;  // Marca que não devemos salvar os dados no final
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
             // Captura qualquer número que não seja uma opção válida (ex: 7, 8)
             cout << "Opção inválida." << endl;
             pressEnter();
        }
    }

    
    // Lógica de Saída
    // Só são salvas as contas de volta no banco se elas NÃO foram excluídas.
    if (!contaExcluida) {
        cout << "Salvando dados das contas..." << endl;
        gerenciador.salvarConta(c1);
        gerenciador.salvarConta(p1);
    }
    cout << "Sessão do banco encerrada." << endl;
}


// Entry Point
// A execução do programa começa aqui

int main() {
    // Inicializa o construtor do gerenciador do banco de dados
    // já abre a conexão com o banco e chama o criarTabela()
    GerenciadorBD gerenciador("banco_contas.db");

    // Loop de Autenticação
    // O programa fica preso no while(true) até que o usuário escolha a opção (3) "Sair do Programa" ou realizar login
    while (true) {
        int escolha = 0;
        string usuario, senha;
        bool loginEfetuado = false;
        string usuarioLogado; // Armazena quem fez o login

        // Loop de Login/Cadastro
        // O usuário fica preso aqui até fazer um login válido
        // ou sair do programa (opção 3, que retorna de main)
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
                pressEnter();
                continue;
            }

            if (escolha == 1) { // Fluxo de Login
                cout << "Login: "; cin >> usuario;
                cout << "Senha: "; cin >> senha;

                // A lógica de SELECT * FROM usuarios... está encapsulada dentro do GerenciadorBD
                if (gerenciador.verificarLogin(usuario, senha)) {
                    loginEfetuado = true;    // Seta a flag para quebrar o loop while(!loginEfetuado)
                    usuarioLogado = usuario; // Armazena quem logou
                    cout << "Login efetuado com sucesso!" << endl;
                    pressEnter(); // Pausa para o usuário ler o sucesso
                } else {
                    // Se o login falhar, o loginEfetuado continua false
                    cout << "Login ou senha inválidos." << endl;
                    pressEnter(); // Pausa se o login falhar
                }

            } else if (escolha == 2) { // Fluxo de Registro
                string nomeCompleto;
                double saldoCC, saldoCP;

                cout << "Novo Login (Não utilize espaços): "; cin >> usuario;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Nome Completo (para o titular): ";
                getline(cin, nomeCompleto); // getline lê a linha inteira

                cout << "Crie uma senha: "; cin >> senha;
                cout << "Depósito Inicial na Conta Corrente: R$"; cin >> saldoCC;
                cout << "Depósito Inicial na Conta Poupança: R$"; cin >> saldoCP;

                // O main só chama o método, O GerenciadorBD é quem sabe como fazer os inserts nas tabelas.
                gerenciador.registrarUsuario(usuario, senha, nomeCompleto, saldoCC, saldoCP);
                pressEnter();

            } else if (escolha == 3) {
                cout << "Saindo..." << endl;
                return 0; // Encerra o programa
            }
        }

        // Transição para o App Principal
        // Se o código chegou até aqui, significa que loginEfetuado é true
        // Agora, o controle é passado para a função principal do aplicativo
        rodarAppPrincipal(gerenciador, usuarioLogado);
        
        // Após rodarAppPrincipal terminar, significa usuário fez logout,
        // o while(true) da main recomeça, voltando para a tela de Login/Cadastro.
    }
}