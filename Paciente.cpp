/*
Arthur Lima Rocha Ribeiro Paes - 261010493
04/09/2026

implementacao dos metodos declarados no paciente.h
o prefixo "Paciente::" antes de cada nome indica que o metodo pertence aquela classe, e nao e uma funcao solta do programa.
*/

#include "Paciente.h"
#include <iostream>
using namespace std;

Paciente::Paciente() : cpf(""), nome(""), idade(0),telefone(""){} // a parte depois dos : e a "lista de inicializacao" -> preenche os campos antes do corpo do construtor comecar a rodar.
Paciente::Paciente(const string& cpf,
                 const string& nome,
                 int idade, const string& telefone)
                 : cpf(cpf), nome(nome), idade(idade), telefone(telefone){}
                 //usei "const string&" no lugar de "string": o & evita copiar a string inteira na chamada e o const garante que a funcao nao vai altera-la.

string Paciente::getCpf() const {return cpf;}
string Paciente::getNome() const {return nome;}
int Paciente::getIdade() const {return idade;}
string Paciente::getTelefone() const {return telefone;}

void Paciente::setNome(const string& nome) {this->nome=nome;}
void Paciente::setIdade(int idade) {this->idade=idade;}
void Paciente::setTelefone(const string& telefone) {this->telefone=telefone;}

void Paciente::exibir() const{
    cout << "CPF: "<<cpf<<"\n"
    << "Nome: "<<nome<<"\n"
    <<"Idade: "<<idade<<"\n"
    <<"Telefone: "<<telefone<<"\n";
}