//Este arquivo é um rascunho provisório 
#ifndef ATENDIMENTO_H
#define ATENDIMENTO_H

#include <string>
#include "Gravidade.h"
using namespace std;

class Atendimento{
    string      cpfPaciente;
    string      data;
    Gravidade   gravidade;
    string      diagnostico;
    string      observacoes;

    public:
    Atendimento(const string& cpfPaciente, const string& data, Gravidade gravidade,
            const string& diagnostico, const string& observacoes);


    string getCpfPaciente() const { return cpfPaciente; }
    string getData() const { return data; }
    Gravidade getGravidade() const { return gravidade; }
    string getDiagnostico() const { return diagnostico; }
    string getObservacoes() const { return observacoes; }
};

#endif