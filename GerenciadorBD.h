#ifndef SISTEMA_BANCO_GERENCIADORBD_H
#define SISTEMA_BANCO_GERENCIADORBD_H

#endif //SISTEMA_BANCO_GERENCIADORBD_H

#include <string>
#include "Conta.h"
#include "sqlite3.h"

class GerenciadorBD {
    private:
        sqlite3* db; // ponteiro do banco de dados
        void criarTabela();

    public:
        GerenciadorBD(const std::string& nomeArquivo); // Construtor
        ~GerenciadorBD(); // Destrutor
};