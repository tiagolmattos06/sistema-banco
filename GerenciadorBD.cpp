# include <iostream>
# include "GerenciadorBD.h"

GerenciadorBD::GerenciadorBD(const std::string& nomeArquivo) {

    // Conversão da string C++ para C
    const char* nomeArquivoChar = nomeArquivo.c_str();

    // definição de onde guardar a conexão
    if (sqlite3_open(nomeArquivoChar, &db) != SQLITE_OK) {
        std::cerr << "Erro ao abrir banco de dados: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
    else {
        std::cout << "Banco de dados aberto com sucesso!" << std::endl;

        criarTabela();
    }
}

GerenciadorBD::~GerenciadorBD() {
    if (db!=nullptr) {
        sqlite3_close(db);
        std::cout << "Banco de dados fechado" << std::endl;
    }
}

void GerenciadorBD::criarTabela() {

    // Criador de tabela
    const char* sql =
        "CREATE TABLE IF NOT EXISTS ("
        "NUMERO INT PRIMARY KEY NOT NULL, " // Chave única da conta
        "TITULAR TEXT NOT NULL, "           // Nome do titular da conta
        "SALDO REAL NOT NULL, "             // Saldo
        "TIPO INT NOT NULL, "               // 1=Corrente ou 2=Poupança
        "RENDIMENTO REAL);";                // Taxa de rendimento

    // Variável para salvar msg de erro do sqlite
    char* errMsg = 0;

    // Execução do SQL
    int resultado = sqlite3_exec(db, sql, 0, 0, &errMsg);

    // Verificação do teste
    if (resultado!=SQLITE_OK) {
        std::cerr << "Erro ao criar tabela: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_free(errMsg);
    }
    else {
        std::cout << "Tabela verificada com sucesso!" << std::endl;
    }
}



