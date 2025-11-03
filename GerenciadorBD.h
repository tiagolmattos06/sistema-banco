#ifndef SISTEMA_BANCO_GERENCIADORBD_H
#define SISTEMA_BANCO_GERENCIADORBD_H

#include <string>
#include "Conta.h"
#include "ContaCorrente.h"
#include "ContaPoupanca.h"
#include "sqlite3.h"


class GerenciadorBD {
private:
    sqlite3* db;


    void criarTabelaContas();
    void criarTabelaUsuarios();

    int getProximoNumeroConta();

public:
    GerenciadorBD(const std::string& nomeArquivo);
    ~GerenciadorBD();

   
    bool salvarConta(Conta& conta);
    bool carregarContasDoUsuario(
        const std::string& username, 
        ContaCorrente& cc_para_preencher, 
        ContaPoupanca& cp_para_preencher
    );

    
    bool registrarUsuario(
        const std::string& usuario, 
        const std::string& senha, 
        const std::string& nomeCompleto,
        double saldoInicialCC,
        double saldoInicialCP
    );

    bool verificarLogin(const std::string& usuario, const std::string& senha);

    bool excluirUsuario(const std::string& username);
};

#endif //SISTEMA_BANCO_GERENCIADORBD_H


