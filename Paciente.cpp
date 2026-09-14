/*
Arthur Lima Rocha Ribeiro Paes - 261010493
04/09/2026

implementacao dos metodos declarados no paciente.h
o prefixo "Paciente::" antes de cada nome indica que o metodo pertence aquela classe, e nao e uma funcao solta do programa.
*/
//Mateus: Adcionei as categorias de Problema, Situação e Gravidade ao paciente.h e ao paciente.cpp para melhor execução da minha parte.
#include "Paciente.h"
#include <iostream>
using namespace std;

Paciente::Paciente() : cpf(""), nome(""), idade(0),telefone(""),problema(""),situa(""),grav(0){} // a parte depois dos : e a "lista de inicializacao" -> preenche os campos antes do corpo do construtor comecar a rodar.
Paciente::Paciente(const string& cpf,
                 const string& nome,
                 int idade, const string& telefone, const string& problema, const string& situa, int grav)
                 : cpf(cpf), nome(nome), idade(idade), telefone(telefone), problema(problema), situa(situa), grav(grav){}
                 //usei "const string&" no lugar de "string": o & evita copiar a string inteira na chamada e o const garante que a funcao nao vai altera-la.

string Paciente::getCpf() const {return cpf;}
string Paciente::getNome() const {return nome;}
int Paciente::getIdade() const {return idade;}
string Paciente::getTelefone() const {return telefone;}
string Paciente :: getProblema() const {return problema;}
string Paciente :: getSitua() const {return situa;}
int Paciente :: getGrav() const {return grav;}

void Paciente::setCpf(const string& cpf) {this->cpf=cpf;}
void Paciente::setNome(const string& nome) {this->nome=nome;}
void Paciente::setIdade(int idade) {this->idade=idade;}
void Paciente::setTelefone(const string& telefone) {this->telefone=telefone;}
void Paciente:: setProblema(const string& problema) {this-> problema=problema;}
void Paciente:: setSitua(const string& situa) {this-> situa=situa;}
void Paciente::setGrav(int grav) {this->grav=grav;}

void Paciente::exibir() const{
    cout << "CPF: "<<cpf<<"\n"
    << "Nome: "<<nome<<"\n"
    <<"Idade: "<<idade<<"\n"
    <<"Telefone: "<<telefone<<"\n"
    <<"Problema: "<<problema<<"\n"
    <<"Situação: "<<situa<<"\n"
    <<"Gravidade: "<<grav<<"\n";
}

