#include <iostream>
#include "../include/GerenciadorBD.h"
#include "../include/ContaCorrente.h"
#include "../include/ContaPoupanca.h"

/**
 * @brief Gera um hash de senha simples e inseguro (apenas para fins de projeto)
 * @param senha A senha em texto cru
 * @return A senha "hasheada" (invertida e com prefixo)
 */
std::string hashSenhaSimples(const std::string& senha) {
    std::string hash;
    // Simplesmente inverte a string da senha
    for (int i = senha.length() - 1; i >= 0; i--) {
        hash += senha[i];
    }
    return "hash_" + hash;
}

// --- Construtor ---
/**
 * @brief Construtor da classe. Abre a conexão com o banco de dados.
 * Se o banco não existir, ele é criado.
 * Também garante que as tabelas Contas e Usuarios existam.
 * @param nomeArquivo O nome do arquivo do banco de dados (ex: "banco.db")
 */
GerenciadorBD::GerenciadorBD(const std::string& nomeArquivo) {
    // Tenta abrir o banco. Se o arquivo não existir, o SQLite o cria
    if (sqlite3_open(nomeArquivo.c_str(), &db) != SQLITE_OK) {
        // Se a abertura falhar, registra o erro e seta o ponteiro db como nulo
        std::cerr << "Erro ao abrir banco de dados: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr; // Usa nullptr para indicar que a conexão falhou
    } else {
        std::cout << "Banco de dados aberto com sucesso!" << std::endl;
        // Garante que a estrutura (schema) do banco está correta
        criarTabelaContas();
        criarTabelaUsuarios();
    }
}

// --- Destrutor ---
/**
 * @brief Destrutor da classe. Fecha a conexão com o banco de dados.
 * É crucial fechar a conexão para liberar os recursos.
 */
GerenciadorBD::~GerenciadorBD() {
    // Só tenta fechar o banco se a conexão foi aberta com sucesso
    if (db != nullptr) {
        sqlite3_close(db);
        std::cout << "Banco de dados fechado" << std::endl;
    }
}

// --- Funções de Tabela ---

/**
 * @brief Cria a tabela Contas se ela ainda não existir.
 * Esta tabela armazena os dados de todas as contas (CC e CP).
 * Ela usa uma FOREIGN KEY para se ligar à tabela Usuarios.
 */
void GerenciadorBD::criarTabelaContas() {
    // SQL ATUALIZADO com USERNAME_DONO e FOREIGN KEY
    const char* sql =
        "CREATE TABLE IF NOT EXISTS Contas ("
        "NUMERO INT PRIMARY KEY NOT NULL, "    // Chave única da conta
        "TITULAR TEXT NOT NULL, "              // Nome completo do dono
        "SALDO REAL NOT NULL, "                // Saldo atual
        "TIPO INT NOT NULL, "                  // 1 = CC, 2 = CP
        "TAXAOPERACAO REAL, "                  // taxa de operação (para CC)
        "RENDIMENTO REAL, "                    // % de rendimento (para CP)
        "USERNAME_DONO TEXT NOT NULL, "        // Chave estrangeira
        "FOREIGN KEY(USERNAME_DONO) REFERENCES Usuarios(USERNAME)" // Link para a tabela de usuários
        ");";

    char* errMsg = nullptr;

    // sqlite3_exec é para comandos que NÃO retornam dados (como CREATE, INSERT, UPDATE)
    // Os nullptr são para a função de callback
    int resultado = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (resultado != SQLITE_OK) {
        std::cerr << "Erro ao criar tabela 'Contas': " << errMsg << std::endl;
        sqlite3_free(errMsg); // Libera a memória da mensagem de erro
    } else {
        std::cout << "Tabela 'Contas' verificada com sucesso!" << std::endl;
    }
}

/**
 * @brief Cria a tabela Usuarios se ela ainda não existir.
 * Esta tabela armazena os dados de login (usuário e senha hasheada).
 */
void GerenciadorBD::criarTabelaUsuarios() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS Usuarios ("
        "USERNAME TEXT PRIMARY KEY NOT NULL, "  // O login único do usuário
        "PASSWORD_HASH TEXT NOT NULL);";        // A senha após o hash

    char* errMsg = nullptr;
    int resultado = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (resultado != SQLITE_OK) {
        std::cerr << "Erro ao criar tabela 'Usuarios': " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Tabela 'Usuarios' verificada com sucesso!" << std::endl;
    }
}

// --- Método Auxiliar ---

/**
 * @brief Encontra o próximo número de conta disponível.
 * @note Esta é uma lógica simples que pega o maior número de conta
 * existente e soma 1.
 * @return O próximo ID de conta inteiro.
 */
int GerenciadorBD::getProximoNumeroConta() {
    sqlite3_stmt* stmt; // Um statement preparado para a consulta
    const char* sql = "SELECT MAX(NUMERO) FROM Contas;";
    int proximoNumero = 101; // Número inicial se o banco estiver vazio

    // sqlite3_prepare_v2 é o primeiro passo para executar um SQL que RETORNA dados
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {

        // sqlite3_step() executa o statement preparado
        if (sqlite3_step(stmt) == SQLITE_ROW) { // Se encontramos um resultado...

            // Verifica se o resultado não é null (o que acontece se a tabela estiver vazia)
            if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
                proximoNumero = sqlite3_column_int(stmt, 0) + 1; // Pega o max e soma 1
            }
        }
    }
    // Finaliza e libera o statement, o que evitando vazamentos de memória.
    sqlite3_finalize(stmt);

    return proximoNumero;
}

// --- Métodos de Usuário  ---

/**
 * @brief Registra um novo usuário e cria suas duas contas (CC e CP).
 * @return true se o registro e a criação das contas forem bem-sucedidos.
 * @return false se o nome de usuário já existir ou se houver um erro no banco.
 */
bool GerenciadorBD::registrarUsuario(const std::string& usuario, const std::string& senha, const std::string& nomeCompleto, double saldoInicialCC, double saldoInicialCP) {
    if (!db) return false; // Proteção: não faz nada se o banco não abriu

    // Hasheia a senha
    std::string hash = hashSenhaSimples(senha);

    // Prepara o SQL para inserir o usuário
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO Usuarios (USERNAME, PASSWORD_HASH) VALUES (?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Erro ao preparar (registrar): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Bind: Liga os valores das variáveis C++ aos '?' do SQL.
    // previne SQL Injection.
    sqlite3_bind_text(stmt, 1, usuario.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hash.c_str(), -1, SQLITE_STATIC);

    // Executa
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        // Se não deu DONE, provavelmente é porque o USERNAME (PRIMARY KEY) já existe.
        std::cerr << "\n[ERRO] Nome de usuário já existe." << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt); // Libera o statement

    // Cria as contas bancárias para este usuário
    std::cout << "\n[SUCESSO] Usuário registrado! Criando contas bancárias..." << std::endl;

    int numCC = getProximoNumeroConta();
    int numCP = numCC + 1; // Lógica simples para o número da poupança

    // Cria os objetos de conta na stack
    ContaCorrente novaCC(numCC, nomeCompleto, saldoInicialCC, usuario, 15.0); // Taxa fixa de 15.0
    ContaPoupanca novaCP(numCP, nomeCompleto, saldoInicialCP, usuario, 0.02); // Rendimento fixo de 2%

    // Salva as novas contas no banco de dados
    // A lógica 'INSERT OR REPLACE' dentro de 'salvarConta' vai funcionar como um INSERT.
    salvarConta(novaCC);
    salvarConta(novaCP);

    std::cout << "Conta Corrente " << numCC << " e Conta Poupança " << numCP << " criadas para " << usuario << "." << std::endl;
    return true;
}

/**
 * @brief Verifica as credenciais de login de um usuário.
 * @return true se o usuário existir e a senha hasheada corresponder.
 * @return false caso contrário.
 */
bool GerenciadorBD::verificarLogin(const std::string& usuario, const std::string& senha) {
    if (!db) return false;

    sqlite3_stmt* stmt;
    const char* sql = "SELECT PASSWORD_HASH FROM Usuarios WHERE USERNAME = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Erro ao preparar (login): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, usuario.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt); // Executa

    if (rc == SQLITE_ROW) { // Encontra o usuário
        std::string hashArmazenado = (const char*)sqlite3_column_text(stmt, 0);
        std::string hashDigitado = hashSenhaSimples(senha);
        sqlite3_finalize(stmt);

        if (hashArmazenado == hashDigitado) {
            std::cout << "\n[SUCESSO] Login efetuado." << std::endl;
            return true;
        } else {
            std::cerr << "\n[ERRO] Senha incorreta." << std::endl;
            return false;
        }
    } else { // rc == SQLITE_DONE (não encontrou) ou um erro
        std::cerr << "\n[ERRO] Usuário não encontrado." << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
}

/**
 * @brief Exclui um usuário e todas as suas contas associadas (CC e CP).
 * @note Esta função usa uma transação (BEGIN, COMMIT, ROLLBACK).
 * Isso garante coesão: ou tudo é excluído (usuário e contas),
 * ou nada é excluído. Se a exclusão do usuário falhar após
 * excluir as contas, um rollback reverteria a exclusão das contas.
 * @param username O usuário a ser excluído.
 * @return true se a transação for concluída com sucesso.
 */
bool GerenciadorBD::excluirUsuario(const std::string& username) {
    if (!db) return false;

    sqlite3_stmt *stmtContas, *stmtUsuario;
    const char* sqlContas = "DELETE FROM Contas WHERE USERNAME_DONO = ?;";
    const char* sqlUsuario = "DELETE FROM Usuarios WHERE USERNAME = ?;";

    // 1. Inicia a Transação
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::cerr << "Erro ao iniciar transação: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // 2. Prepara e executa a exclusão das CONTAS
    if (sqlite3_prepare_v2(db, sqlContas, -1, &stmtContas, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr); // Cancela a transação
        return false;
    }
    sqlite3_bind_text(stmtContas, 1, username.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmtContas) != SQLITE_DONE) {
        sqlite3_finalize(stmtContas);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
    sqlite3_finalize(stmtContas);

    // 3. Prepara e executa a exclusão do USUÁRIO
    if (sqlite3_prepare_v2(db, sqlUsuario, -1, &stmtUsuario, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
    sqlite3_bind_text(stmtUsuario, 1, username.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmtUsuario) != SQLITE_DONE) {
        sqlite3_finalize(stmtUsuario);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
    sqlite3_finalize(stmtUsuario);

    // 4. Se tudo deu certo, confirma (COMMIT) a transação
    if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::cerr << "Erro ao confirmar transação: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}



// --- Métodos de Conta ---

/**
 * @brief Salva o estado de um objeto Conta (CC ou CP) no banco de dados.
 * Usa "INSERT OR REPLACE" para criar uma nova linha se a conta
 * não existir, ou atualizar a linha existente se ela já existir.
 * @param conta Uma referência ao objeto Conta (pode ser ContaCorrente ou ContaPoupanca).
 * @return true se a operação for bem-sucedida.
 */
bool GerenciadorBD::salvarConta(Conta& conta) {
    if (!db) return false;

    sqlite3_stmt* stmt;
    // "INSERT OR REPLACE" é um atalho do SQLite:
    // Se a PRIMARY KEY (NUMERO) não existir, ele age como INSERT.
    // Se a PRIMARY KEY (NUMERO) já existir, ele age como UPDATE, deletando a antiga e inserindo a nova.
    const char* sql = "INSERT OR REPLACE INTO Contas (NUMERO, TITULAR, SALDO, TIPO, TAXAOPERACAO, RENDIMENTO, USERNAME_DONO) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?);"; // 7 '?'

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Erro ao preparar SQL (salvar): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Mapeamento Objeto-Relacional (OOM)
    // são acessados os membros da classe Conta para "ligar" (bind) aos '?' do SQL.

    // Binds Comuns (da classe base 'Conta')
    sqlite3_bind_int(stmt, 1, conta.numero);
    sqlite3_bind_text(stmt, 2, conta.titular.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, conta.saldo);
    sqlite3_bind_text(stmt, 7, conta.username_dono.c_str(), -1, SQLITE_STATIC);

    // Binds Específicos (das classes filhas)
    // É usado dynamic_cast para tentar converter o ponteiro da classe base para um ponteiro de classe filha.

    // Tenta converter para ContaCorrente
    if (ContaCorrente* cc = dynamic_cast<ContaCorrente*>(&conta)) {
        sqlite3_bind_int(stmt, 4, 1); // Tipo 1 (CC)
        sqlite3_bind_double(stmt, 5, cc->taxaManutencao); // Valor específico da CC
        sqlite3_bind_null(stmt, 6); // Rendimento é nulo para CC
    }
    // Se falhar, tenta converter para ContaPoupanca
    else if (ContaPoupanca* cp = dynamic_cast<ContaPoupanca*>(&conta)) {
        sqlite3_bind_int(stmt, 4, 2); // Tipo 2 (CP)
        sqlite3_bind_null(stmt, 5); // Taxa é nula para CP
        sqlite3_bind_double(stmt, 6, cp->taxaRendimento); // Valor específico da CP
    }
    else {
        // Isso não deve acontecer se só tivermos CC e CP
        sqlite3_finalize(stmt); return false;
    }

    // Executa o statement (INSERT OR REPLACE)
    bool sucesso = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!sucesso) {
        std::cerr << "Erro ao executar SQL (salvar conta " << conta.numero << "): " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return sucesso;
}

/**
 * @brief Carrega os dados das contas (CC e CP) de um usuário do banco
 * e preenche os objetos fornecidos.
 *
 * @param username O usuário (USERNAME_DONO) cujas contas devem ser carregadas.
 * @param cc_para_preencher Uma referência a um objeto ContaCorrente vazio
 * que será preenchido com os dados do banco.
 * @param cp_para_preencher Uma referência a um objeto ContaPoupanca vazio
 * que será preenchido com os dados do banco.
 * @return true se ambas as contas (CC e CP) forem encontradas e carregadas.
 */
bool GerenciadorBD::carregarContasDoUsuario(const std::string& username, ContaCorrente& cc_para_preencher, ContaPoupanca& cp_para_preencher) {
    if (!db) return false;

    sqlite3_stmt* stmt;
    // Busca todas as colunas das contas que pertencem a este usuário
    const char* sql = "SELECT NUMERO, TITULAR, SALDO, TIPO, TAXAOPERACAO, RENDIMENTO "
                      "FROM Contas WHERE USERNAME_DONO = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Erro ao preparar (carregar contas): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    bool encontrouCC = false;
    bool encontrouCP = false;

    // Como um usuário tem duas contas (CC e CP), este loop deve executar duas vezes
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int numero = sqlite3_column_int(stmt, 0);
        string titular = (const char*)sqlite3_column_text(stmt, 1);
        double saldo = sqlite3_column_double(stmt, 2);
        int tipoConta = sqlite3_column_int(stmt, 3);

        // Mapeamento Relacional-Objeto (RMO)
        // É feito o inverso do salvarConta. São lidos os dados
        // do banco e usados "preencher" os objetos C++
        // que foram passados por referência.
        // O friend class permite escrever diretamente nos membros.

        if (tipoConta == 1) { // Conta Corrente
            cc_para_preencher.numero = numero;
            cc_para_preencher.titular = titular;
            cc_para_preencher.saldo = saldo;
            cc_para_preencher.username_dono = username;
            cc_para_preencher.taxaManutencao = sqlite3_column_double(stmt, 4);
            encontrouCC = true;
        }
        else if (tipoConta == 2) { // Conta Poupança
            cp_para_preencher.numero = numero;
            cp_para_preencher.titular = titular;
            cp_para_preencher.saldo = saldo;
            cp_para_preencher.username_dono = username;
            cp_para_preencher.taxaRendimento = sqlite3_column_double(stmt, 5);
            encontrouCP = true;
        }
    }

    sqlite3_finalize(stmt);

    if (!encontrouCC || !encontrouCP) {
        // Isso não deve acontecer se o registro funcionou
        std::cerr << "Aviso: Contas do usuário " << username << " não foram totalmente carregadas." << std::endl;
        return false;
    }

    std::cout << "Contas de " << username << " carregadas do BD." << std::endl;
    return true;
}