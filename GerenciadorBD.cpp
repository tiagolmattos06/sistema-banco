#include <iostream>
#include "GerenciadorBD.h"
#include "ContaCorrente.h"
#include "ContaPoupanca.h"

// metodo para armazenar senhas
std::string hashSenhaSimples(const std::string& senha) {
    std::string hash = "";
    for (int i = senha.length() - 1; i >= 0; i--) {
        hash += senha[i];
    }
    return "hash_" + hash;
}



GerenciadorBD::GerenciadorBD(const std::string& nomeArquivo) {
    const char* nomeArquivoChar = nomeArquivo.c_str();

    if (sqlite3_open(nomeArquivoChar, &db) != SQLITE_OK) {
        std::cerr << "Erro ao abrir banco de dados: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
    else {
        std::cout << "Banco de dados aberto com sucesso!" << std::endl;
        criarTabelaContas();
        criarTabelaUsuarios();
    }
}

GerenciadorBD::~GerenciadorBD() {
    if (db != nullptr) {
        sqlite3_close(db);
        std::cout << "Banco de dados fechado" << std::endl;
    }
}



void GerenciadorBD::criarTabelaContas() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS Contas ("
        "NUMERO INT PRIMARY KEY NOT NULL, "
        "TITULAR TEXT NOT NULL, "
        "SALDO REAL NOT NULL, "
        "TIPO INT NOT NULL, "
        "TAXAOPERACAO REAL, "
        "RENDIMENTO REAL),
        "USERNAME_DONO TEXT NOT NULL,
        "FOREIGN KEY(USERNAME_DONO) REFERENCES Usuarios(USERNAME)"
        ");";

    char* errMsg = 0;
    int resultado = sqlite3_exec(db, sql, 0, 0, &errMsg);

    if (resultado != SQLITE_OK) {
        std::cerr << "Erro ao criar tabela 'Contas': " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    else {
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
    }
    else {
        std::cout << "Tabela 'Usuarios' verificada com sucesso!" << std::endl;
    }
}



bool GerenciadorBD::salvarConta(Conta& conta) {
    if (!db) return false;

    sqlite3_stmt* stmt;
    const char* sql = "INSERT OR REPLACE INTO Contas (NUMERO, TITULAR, SALDO, TIPO, TAXAOPERACAO, RENDIMENTO) "
                      "VALUES (?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Erro ao preparar SQL (salvar): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_int(stmt, 1, conta.numero);
    sqlite3_bind_text(stmt, 2, conta.titular.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, conta.saldo);
    if (ContaCorrente* cc = dynamic_cast<ContaCorrente*>(&conta)) {
        sqlite3_bind_int(stmt, 4, 1);
        sqlite3_bind_double(stmt, 5, cc->taxaManutencao);
        sqlite3_bind_null(stmt, 6);
    }
    else if (ContaPoupanca* cp = dynamic_cast<ContaPoupanca*>(&conta)) {
        sqlite3_bind_int(stmt, 4, 2);
        sqlite3_bind_null(stmt, 5);
        sqlite3_bind_double(stmt, 6, cp->taxaRendimento);
    }
    else {
        sqlite3_finalize(stmt);
        return false;
    }
    bool sucesso = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!sucesso) {
        std::cerr << "Erro ao executar SQL (salvar): " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    return sucesso;
}

bool GerenciadorBD::carregarConta(Conta& conta) {
    
    if (!db) return false;
    
    sqlite3_stmt* stmt;
    const char* sql = "SELECT TITULAR, SALDO, TIPO, TAXAOPERACAO, RENDIMENTO "
                      "FROM Contas WHERE NUMERO = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Erro ao preparar SQL (carregar): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_int(stmt, 1, conta.getNumero());
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        conta.titular = (const char*)sqlite3_column_text(stmt, 0);
        conta.saldo = sqlite3_column_double(stmt, 1);
        int tipoConta = sqlite3_column_int(stmt, 2);
        if (tipoConta == 1) {
            ContaCorrente* cc = dynamic_cast<ContaCorrente*>(&conta);
            if (cc) {
                cc->taxaManutencao = sqlite3_column_double(stmt, 3);
            }
        }
        else if (tipoConta == 2) {
            ContaPoupanca* cp = dynamic_cast<ContaPoupanca*>(&conta);
            if (cp) {
                cp->taxaRendimento = sqlite3_column_double(stmt, 4);
            }
        }
        sqlite3_finalize(stmt);
        std::cout << "Conta " << conta.getNumero() << " carregada do BD." << std::endl;
        return true;
    }
    else {
        std::cout << "Conta " << conta.getNumero() << " não encontrada. Salvando estado inicial no BD..." << std::endl;
        sqlite3_finalize(stmt);
        return salvarConta(conta);
    }
}




bool GerenciadorBD::registrarUsuario(const std::string& usuario, const std::string& senha) {
    if (!db) return false;

    
    std::string hash = hashSenhaSimples(senha);

  
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO Usuarios (USERNAME, PASSWORD_HASH) VALUES (?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Erro ao preparar (registrar): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, usuario.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hash.c_str(), -1, SQLITE_STATIC);


    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
       
        if (rc == SQLITE_CONSTRAINT) {
            std::cerr << "\n[ERRO] Nome de usuário já existe." << std::endl;
        } else {
            std::cerr << "\n[ERRO] Falha ao registrar usuário: " << sqlite3_errmsg(db) << std::endl;
        }
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    std::cout << "\n[SUCESSO] Usuário registrado!" << std::endl;
    return true;
}

bool GerenciadorBD::verificarLogin(const std::string& usuario, const std::string& senha) {
    if (!db) return false;

    
    sqlite3_stmt* stmt;
    const char* sql = "SELECT PASSWORD_HASH FROM Usuarios WHERE USERNAME = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Erro ao preparar (login): " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, usuario.c_str(), -1, SQLITE_STATIC);

   
    int rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) { // Encontrou o usuário
        std::string hashArmazenado = (const char*)sqlite3_column_text(stmt, 0);
        std::string hashDigitado = hashSenhaSimples(senha);

        sqlite3_finalize(stmt);

        if (hashArmazenado == hashDigitado) {
            std::cout << "\n[SUCESSO] Login efetuado." << std::endl;
            return true; // Senha correta!
        } else {
            std::cerr << "\n[ERRO] Senha incorreta." << std::endl;
            return false;
        }
    } else { // Não encontrou o usuário 
        std::cerr << "\n[ERRO] Usuário não encontrado." << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

}


