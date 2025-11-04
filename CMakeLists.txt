cmake_minimum_required(VERSION 3.20)
project(sistema_banco)

set(CMAKE_CXX_STANDARD 17)

include_directories(include lib)

add_executable(sistema_banco)

target_sources(sistema_banco PRIVATE
        src/main.cpp
        src/GerenciadorBD.cpp
        src/Conta.cpp
        src/ContaCorrente.cpp
        src/ContaPoupanca.cpp
        lib/sqlite3.c
)

# 4. Para Linux/macOS
# Linka as bibliotecas que o SQLite pode precisar

target_link_libraries(sistema_banco PRIVATE pthread)
