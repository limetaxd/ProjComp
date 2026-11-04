triagem: main.cpp Paciente.cpp Atendimento.cpp Historico.cpp
	g++ -Wall -Wextra -std=c++11 -o triagem main.cpp Paciente.cpp Atendimento.cpp Historico.cpp

clean:
	rm -f triagem triagem.exe *.o
