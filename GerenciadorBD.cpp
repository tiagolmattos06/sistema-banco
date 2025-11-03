#include <iostream>
#include "GerenciadorBD.h"
#include "ContaCorrente.h"
#include "ContaPoupanca.h"

// --- Função de Hashing  ---
std::string hashSenhaSimples(const std::string& senha) {
    std::string hash = "";
    for (int i = senha.length() - 1; i >= 0; i--) {
        hash += senha[i];
    }
    return "hash_" + hash;
}

// --- Construtor ---
GerenciadorBD::GerenciadorBD(const std::string& nomeArquivo) {
    if (sqlite3_open(nomeArquivo.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Erro ao abrir banco de dados: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    } else {
        std::cout << "Banco de dados aberto com sucesso!" << std::endl;
        criarTabelaContas();
        criarTabelaUsuarios();
    }
}

// --- Destrutor ---
GerenciadorBD::~GerenciadorBD() {
    if (db != nullptr) {
        sqlite3_close(db);
        std::cout << "Banco de dados fechado" << std::endl;
    }
}

// --- Funções de Tabela ---
void GerenciadorBD::criarTabelaContas() {
    // SQL ATUALIZADO com USERNAME_DONO e FOREIGN KEY
    const char* sql =
        "CREATE TABLE IF NOT EXISTS Contas ("
        "NUMERO INT PRIMARY KEY NOT NULL, "
        "TITULAR TEXT NOT NULL, "
        "SALDO REAL NOT NULL, "
        "TIPO INT NOT NULL, "
        "TAXAOPERACAO REAL, "
        "RENDIMENTO REAL, "
        "USERNAME_DONO TEXT NOT NULL, " // <-- ADICIONADO
        "FOREIGN KEY(USERNAME_DONO) REFERENCES Usuarios(USERNAME)"
        ");";

    char* errMsg = 0;
    int resultado = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (resultado != SQLITE_OK) {
        std::cerr << "Erro ao criar tabela 'Contas': " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Tabela 'Contas' verificada com sucesso!" << std::endl;
    }
}

void GerenciadorBD::criarTabelaUsuarios() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS Usuarios ("
        "USERNAME TEXT PRIMARY KEY NOT NULL, "
        "PASSWORD_HASH TEXT NOT NULL);";

    char* errMsg = 0;
    int resultado = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (resultado != SQLITE_OK) {
        std::cerr << "Erro ao criar tabela 'Usuarios': " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Tabela 'Usuarios' verificada com sucesso!" << std::endl;
    }
}

// --- Método Auxiliar ---
int GerenciadorBD::getProximoNumeroConta() {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT MAX(NUMERO) FROM Contas;";
    int proximoNumero = 101; // Número inicial se o banco estiver vazio

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // Verifica se o resultado não é NULL (banco vazio)
            if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
                proximoNumero = sqlite3_column_int(stmt, 0) + 1;
            }
        }
    }
    sqlite3_finalize(stmt);
    
    // Garante que CC e CP não tenham o mesmo número 
    // Esta é uma lógica simples, assume que o próximo + 1 também está vago
    return proximoNumero;
}

// --- Métodos de Usuário  ---

bool GerenciadorBD::registrarUsuario(const std::string& usuario, const std::string& senha, const std::string& nomeCompleto, double saldoInicialCC, double saldoInicialCP) {
    if (!db) return false;

    // 1. Hashear a senha
    std::string hash = hashSenhaSimples(senha);

    // 2. Preparar o SQL para inserir
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO Usuarios (USERNAME, PASSWORD_HASH) VALUES (?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Erro ao preparar (registrar): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, usuario.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hash.c_str(), -1, SQLITE_STATIC);

    // 3. Executar
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "\n[ERRO] Nome de usuário já existe." << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    // 4. Criar as contas bancárias para este usuário
    std::cout << "\n[SUCESSO] Usuário registrado! Criando contas bancárias..." << std::endl;

    int numCC = getProximoNumeroConta();
    int numCP = numCC + 1; // Número da poupança será o próximo

    // Cria os objetos de conta
    ContaCorrente novaCC(numCC, nomeCompleto, saldoInicialCC, usuario, 15.0); // Taxa fixa de 15.0
    ContaPoupanca novaCP(numCP, nomeCompleto, saldoInicialCP, usuario, 0.02); // Rendimento fixo de 2%

    // Salva as novas contas no banco de dados
    salvarConta(novaCC);
    salvarConta(novaCP);

    std::cout << "Conta Corrente " << numCC << " e Conta Poupança " << numCP << " criadas para " << usuario << "." << std::endl;
    return true;
}

bool GerenciadorBD::verificarLogin(const std::string& usuario, const std::string& senha) {
    // (Este método não muda, mas está aqui para completude)
    if (!db) return false;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT PASSWORD_HASH FROM Usuarios WHERE USERNAME = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Erro ao preparar (login): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, usuario.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
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
    } else {
        std::cerr << "\n[ERRO] Usuário não encontrado." << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
}

// --- Opção de excluir a conta ---
bool GerenciadorBD::excluirUsuario(const std::string& username) {
    if (!db) return false;

    sqlite3_stmt *stmtContas, *stmtUsuario;
    const char* sqlContas = "DELETE FROM Contas WHERE USERNAME_DONO = ?;";
    const char* sqlUsuario = "DELETE FROM Usuarios WHERE USERNAME = ?;";


    if (sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0) != SQLITE_OK) {
        std::cerr << "Erro ao iniciar transação: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }


    if (sqlite3_prepare_v2(db, sqlContas, -1, &stmtContas, 0) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0); // Cancela a transação
        return false;
    }
    sqlite3_bind_text(stmtContas, 1, username.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmtContas) != SQLITE_DONE) {
        sqlite3_finalize(stmtContas);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return false;
    }
    sqlite3_finalize(stmtContas);


    if (sqlite3_prepare_v2(db, sqlUsuario, -1, &stmtUsuario, 0) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return false;
    }
    sqlite3_bind_text(stmtUsuario, 1, username.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmtUsuario) != SQLITE_DONE) {
        sqlite3_finalize(stmtUsuario);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return false;
    }
    sqlite3_finalize(stmtUsuario);


    if (sqlite3_exec(db, "COMMIT;", 0, 0, 0) != SQLITE_OK) {
        std::cerr << "Erro ao confirmar transação: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}



// --- Métodos de Conta ---

bool GerenciadorBD::salvarConta(Conta& conta) {
    if (!db) return false;

    sqlite3_stmt* stmt;
    // SQL ATUALIZADO
    const char* sql = "INSERT OR REPLACE INTO Contas (NUMERO, TITULAR, SALDO, TIPO, TAXAOPERACAO, RENDIMENTO, USERNAME_DONO) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?);"; // 7 '?'

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Erro ao preparar SQL (salvar): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Acesso 'friend' aos membros 'protected' e 'private'
    sqlite3_bind_int(stmt, 1, conta.numero);
    sqlite3_bind_text(stmt, 2, conta.titular.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, conta.saldo);
    sqlite3_bind_text(stmt, 7, conta.username_dono.c_str(), -1, SQLITE_STATIC); // <-- BIND DO DONO

    if (ContaCorrente* cc = dynamic_cast<ContaCorrente*>(&conta)) {
        sqlite3_bind_int(stmt, 4, 1); // Tipo 1
        sqlite3_bind_double(stmt, 5, cc->taxaManutencao);
        sqlite3_bind_null(stmt, 6);
    }
    else if (ContaPoupanca* cp = dynamic_cast<ContaPoupanca*>(&conta)) {
        sqlite3_bind_int(stmt, 4, 2); // Tipo 2
        sqlite3_bind_null(stmt, 5);
        sqlite3_bind_double(stmt, 6, cp->taxaRendimento);
    }
    else {
        sqlite3_finalize(stmt); return false;
    }

    bool sucesso = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!sucesso) {
        std::cerr << "Erro ao executar SQL (salvar conta " << conta.numero << "): " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return sucesso;
}

// NOVO MÉTODO
bool GerenciadorBD::carregarContasDoUsuario(const std::string& username, ContaCorrente& cc_para_preencher, ContaPoupanca& cp_para_preencher) {
    if (!db) return false;

    sqlite3_stmt* stmt;
    // Busca todas as colunas das contas que pertencem a este usuário
    const char* sql = "SELECT NUMERO, TITULAR, SALDO, TIPO, TAXAOPERACAO, RENDIMENTO "
                      "FROM Contas WHERE USERNAME_DONO = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Erro ao preparar (carregar contas): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    bool encontrouCC = false;
    bool encontrouCP = false;

    // Loop (enquanto houver contas para este usuário)
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int numero = sqlite3_column_int(stmt, 0);
        string titular = (const char*)sqlite3_column_text(stmt, 1);
        double saldo = sqlite3_column_double(stmt, 2);
        int tipoConta = sqlite3_column_int(stmt, 3);
        
        // Preenche o objeto correto (CC ou CP) usando acesso 'friend'
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
        // Isso não deveria acontecer se o registro funcionou
        std::cerr << "Aviso: Contas do usuário " << username << " não foram totalmente carregadas." << std::endl;
        return false;
    }
    
    std::cout << "Contas de " << username << " carregadas do BD." << std::endl;
    return true;
}
