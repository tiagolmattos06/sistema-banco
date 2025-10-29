cmake_minimum_required(VERSION 4.0)
project(sistema_banco)

set(CMAKE_CXX_STANDARD 20)

add_executable(sistema_banco main.cpp Conta.cpp ContaCorrente.cpp ContaPoupanca.cpp Banco.cpp)
