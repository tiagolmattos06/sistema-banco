#ifndef SISTEMA_BANCO_GERENCIADORBD_H
#define SISTEMA_BANCO_GERENCIADORBD_H

// Bibliotecas Padrão e de Terceiros
#include <string>           // Para usar std::string
#include "../lib/sqlite3.h"  // Header da biblioteca SQLite

#include "Conta.h"
#include "ContaCorrente.h"
#include "ContaPoupanca.h"

// Declaração da classe GerenciadorBD
class GerenciadorBD {
private:
    // Membros Privados

    /**
     * @brief O ponteiro principal para a conexão com o banco de dados SQLite.
     * Este ponteiro é inicializado no construtor (com sqlite3_open)
     * e fechado no destrutor (com sqlite3_close).
     * Se for 'nullptr', significa que a conexão com o banco falhou.
     */
    sqlite3* db;

    // Métodos Auxiliares Privados
    // Estes métodos são chamados internamente (pelo construtor)
    // para garantir que o banco de dados esteja pronto para uso.

    /**
     * @brief Garante que a tabela 'Contas' exista no banco de dados.
     * É chamado pelo construtor.
     */
    void criarTabelaContas();

    /**
     * @brief Garante que a tabela 'Usuarios' exista no banco de dados.
     * É chamado pelo construtor.
     */
    void criarTabelaUsuarios();

    /**
     * @brief Método auxiliar para encontrar o próximo ID de conta disponível.
     * Usado durante o registro de um novo usuário.
     * @return Um 'int' com o próximo número de conta vago.
     */
    int getProximoNumeroConta();

public:
    // Construtor e Destrutor

    /**
     * @brief Construtor. Abre (ou cria) o arquivo de banco de dados.
     * @param nomeArquivo O caminho para o arquivo .db (ex: "banco_contas.db").
     */
    GerenciadorBD(const std::string& nomeArquivo);

    /**
     * @brief Destrutor. Fecha a conexão com o banco de dados.
     */
    ~GerenciadorBD();

    // API Pública de Gerenciamento de Contas

    /**
     * @brief Salva (Insere ou Atualiza) o estado de uma conta no banco.
     * Usa 'INSERT OR REPLACE' para persistir os dados do objeto.
     * @param conta Uma referência ao objeto Conta (CC ou CP) a ser salvo.
     * @return true se a operação foi bem-sucedida, false caso contrário.
     */
    bool salvarConta(Conta& conta);

    /**
     * @brief Carrega os dados da Conta Corrente e da Conta Poupança de um usuário.
     * Este método "preenche" os objetos passados por referência com os dados
     * lidos do banco de dados.
     * @param username O login do usuário dono das contas.
     * @param cc_para_preencher Referência a um objeto ContaCorrente que será preenchido.
     * @param cp_para_preencher Referência a um objeto ContaPoupanca que será preenchido.
     * @return true se ambas as contas foram encontradas e carregadas, false caso contrário.
     */
    bool carregarContasDoUsuario(
        const std::string& username,
        ContaCorrente& cc_para_preencher,
        ContaPoupanca& cp_para_preencher
    );

    // API Pública de Gerenciamento de Usuários

    /**
     * @brief Registra um novo usuário no sistema.
     * Este método cria uma entrada na tabela 'Usuarios' e também
     * cria as duas contas (CC e CP) iniciais na tabela 'Contas'.
     * @param usuario O novo nome de login (deve ser único).
     * @param senha A senha em texto plano (será hasheada).
     * @param nomeCompleto O nome do titular das contas.
     * @param saldoInicialCC Saldo inicial para a Conta Corrente.
     * @param saldoInicialCP Saldo inicial para a Conta Poupança.
     * @return true se o usuário e as contas foram criados, false se o usuário já existir.
     */
    bool registrarUsuario(
        const std::string& usuario,
        const std::string& senha,
        const std::string& nomeCompleto,
        double saldoInicialCC,
        double saldoInicialCP
    );

    /**
     * @brief Verifica se um usuário e senha correspondem aos registros do banco.
     * @param usuario O login que está tentando entrar.
     * @param senha A senha em texto plano digitada (será hasheada para comparação).
     * @return true se o login for válido, false caso contrário.
     */
    bool verificarLogin(const std::string& usuario, const std::string& senha);

    /**
     * @brief Exclui um usuário e todas as suas contas associadas (CC e CP).
     * Esta operação é executada dentro de uma transação (COMMIT/ROLLBACK)
     * para garantir a integridade dos dados (ou tudo é excluído, ou nada é).
     * @param username O login do usuário a ser excluído.
     * @return true se a exclusão foi bem-sucedida, false caso contrário.
     */
    bool excluirUsuario(const std::string& username);
};

#endif //SISTEMA_BANCO_GERENCIADORBD_H