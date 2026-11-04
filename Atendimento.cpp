#include "Atendimento.h"
#include <iostream>
using namespace std;

Atendimento::Atendimento(const string& cpfPaciente, const string& data, Gravidade gravidade,
            const string& diagnostico, const string& observacoes)
	: cpfPaciente(cpfPaciente), data(data), gravidade(gravidade), diagnostico(diagnostico), observacoes(observacoes){}
