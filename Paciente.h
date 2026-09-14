/*
Arthur Lima Rocha Ribeiro Paes - 261010493
04/09/2026

aqui declaramos a classe paciente: quais dados ela guarda e o que ela sabe fazer.
achei importante separar o .h do .cpp ,pois os outros arquivos que incluem somente o .h nao precisam saber como o metodo funciona
*/

#ifndef PACIENTE_H
#define PACIENTE_H

#include <string>
using namespace std;

class Paciente{
    private:    // deixei privado porque impede que outra parte do sistema mude o cpf de um paciente por acidente. o acesso e feito pelo setter e getter
        string cpf;
        string nome;
        int idade; 
        string telefone;
        string problema;
        string situa;
        int grav;

    public:
        Paciente();
        Paciente(const string& cpf,
                 const string& nome,
                 int idade, const string& telefone, const string& problema, const string& situa, int grav);

    string getCpf() const;
    string getNome() const;
    int getIdade() const;
    string getTelefone() const;
    string getProblema() const;
    string getSitua() const;
    int getGrav() const;
    // o "const" vem de constante, que indica que o metodo nao altera o objeto

    void setCpf(const string& cpf);
    void setNome(const string& nome);
    void setIdade(int idade);
    void setTelefone(const string& telefone);
    void setProblema(const string& problema);
    void setSitua(const string& situa);
    void setGrav(int grav);
    // nao tem setCpf pq o cpf e a chave na tabela hash, alterar a chave depois de inserido quebraria a busca

    
    void exibir() const;
    // mostra a ficha do paciente
};

#endif