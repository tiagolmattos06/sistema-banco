cmake_minimum_required(VERSION 3.10)
project(sistema_banco)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
        sistema_banco

        
        main.cpp
        Conta.cpp
        ContaCorrente.cpp
        ContaPoupanca.cpp
        GerenciadorBD.cpp
        
        sqlite3.c

)
