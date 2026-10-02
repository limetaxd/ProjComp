// ponto de entrada do programa e menu principal.
// arquivo compartilhado pelos tres integrantes.

#include <iostream>
#include <string>
#include <fstream>
#include "Gravidade.h"
#include "Paciente.h"
#include "TabelaHash.h"

using namespace std;

// indice global de pacientes: cpf -> paciente.
// e o que permite achar qualquer paciente em tempo constante, sem varrer a base inteira. modulo do arthur.

// declarado fora de qualquer funcao (variavel global) para que todas as funcoes do menu possam usar a mesma tabela.
TabelaHash<string, Paciente> indicePacientes;

// desenha o menu na tela.
void exibirMenu() {
    cout << "\n======================================\n";
    cout << "     TRIAGEM PS - PRONTO SOCORRO\n";
    cout << "======================================\n";
    cout << "  1 - Cadastrar paciente\n";
    cout << "  2 - Realizar triagem (classificar risco)\n";
    cout << "  3 - Chamar proximo atendimento\n";
    cout << "  4 - Consultar prontuario por CPF\n";
    cout << "  5 - Navegar historico do paciente\n";
    cout << "  6 - Ver recorrencia de diagnosticos\n";
    cout << "  7 - Ver estado atual da fila\n";
    cout << "  8 - Relatorios e estatisticas\n";
    cout << "  9 - Salvar / Carregar dados\n";
    cout << "  0 - Sair\n";
    cout << "--------------------------------------\n";
    cout << "  Opcao: ";
}

// ============================================================
// ARTHUR - nucleo de dados e busca
// ============================================================

// opcao 1: le os dados do teclado e guarda o paciente na tabela hash.
void cadastrarPaciente() {
    string cpf, nome, telefone;
    int idade;

    cout << "\nCPF: ";
    cin >> cpf;

    // a busca antes da insercao evita cpf duplicado.
    // buscar devolve nullptr quando nao encontra, entao "diferente de nullptr" significa "ja existe".
    if (indicePacientes.buscar(cpf) != nullptr) {
        cout << "Ja existe paciente cadastrado com este CPF.\n";
        return;
    }

    cin.ignore();
    cout << "Nome: ";
    getline(cin, nome);
    cout << "Idade: ";
    cin >> idade;
    cout << "Telefone: ";
    cin >> telefone;

    Paciente p(cpf, nome, idade, telefone);
    indicePacientes.inserir(cpf, p);
    cout << "Paciente cadastrado.\n";
}

// opcao 4: busca o paciente pelo cpf e mostra a ficha.
void consultarProntuario() {
    string cpf;
    cout << "\nCPF: ";
    cin >> cpf;

    // guarda o retorno em um ponteiro e so acessa depois de conferir que nao e nullptr. acessar um ponteiro nulo derruba o programa.
    Paciente* p = indicePacientes.buscar(cpf);
    if (p == nullptr) {
        cout << "Paciente nao encontrado.\n";
        return;
    }
    p->exibir();    // -> e o acesso a metodo por meio de ponteiro

    // TODO(paulo): exibir aqui o historico de atendimentos da lde
}

// Persiste os dados dos pacientes em um arquivo txt
void salvarPacientes(){
    ofstream arquivoPacientes("pacientes.txt");
    if (!arquivoPacientes.is_open()) {
        cout<<"Erro ao abrir o arquivo de pacientes!!"<<endl;
        return;
    }

    vector<string> chaves = indicePacientes.listarChaves();
    
    for(auto it = chaves.begin(); it != chaves.end(); it++){

        Paciente *paciente = indicePacientes.buscar(*it); 
        string cpf = paciente->getCpf();
        string nome = paciente->getNome();
        int idade = paciente->getIdade();
        string telefone = paciente->getTelefone();
        
        arquivoPacientes<<cpf<<";";
        arquivoPacientes<<nome<<";";
        arquivoPacientes<<idade<<";";
        arquivoPacientes<<telefone<<endl;
    }

    arquivoPacientes.close();

    cout<<"Dados salvos em paciente.txt com sucesso."<<endl;
}

// Lê os dados armazenados em um arquivo txt dos pacientes
void carregarPacientes(){
    ifstream arquivoPacientes("pacientes.txt");

    if (!arquivoPacientes.is_open()) {
        cout<<"Erro ao abrir o arquivo de pacientes!!"<<endl;
        return;
    }

    string linha;
    int local = 0;
    

    //Variaveis usada para armazenar temporariamente os dados de um paciente
    string cpf, nome,telefone;
    int idade, quantidade = 0;

    //cpf;nome;idade;telefone
    while(getline(arquivoPacientes, linha)){

        local = linha.find(';');
        cpf = linha.substr(0, local);
        linha = linha.substr(local + 1, linha.length());

        local = linha.find(';');
        nome = linha.substr(0, local);
        linha = linha.substr(local + 1, linha.length());

        local = linha.find(';');
        idade = stoi(linha.substr(0, local));
        linha = linha.substr(local + 1, linha.length());

        telefone = linha.substr(0, linha.length());
        
        Paciente p(cpf, nome, idade, telefone);
        indicePacientes.inserir(cpf, p);

        quantidade++;
    }

    cout<<quantidade<<" pacientes carregados com sucesso."<<endl;
}

// opcao 9: gravar e ler os dados em arquivo, para nao perder tudo ao fechar.
void salvarCarregarDados() {
    int aux;

    cout<<"1 - Salvar dados"<<endl;
    cout<<"2 - Carregar dados"<<endl;
    cin>>aux;

    switch(aux){
        case 1:{
            salvarPacientes();
            break;
        }
        case 2:{
            carregarPacientes();
            break;

        }

    }

}

// opcao 8: numeros do sistema. as tres primeiras linhas servem de material para a analise de desempenho da tabela hash.
void relatorios() {
    cout << "\nPacientes cadastrados: " << indicePacientes.tamanho() << "\n";
    cout << "Colisoes na tabela hash: " << indicePacientes.colisoes() << "\n";
    cout << "Fator de carga: " << indicePacientes.fatorDeCarga() << "\n";

    // TODO(mateus): relatorios ordenados com mergesort e quicksort
}

// ============================================================
// MATEUS - fila de atendimento
// ============================================================

// opcao 2: classifica o risco do paciente e coloca na fila de prioridade.
void realizarTriagem() {
    cout << "\n[TODO Mateus] Classificar risco e inserir na fila.\n";
}

// opcao 3: retira da fila o paciente de maior gravidade.
// em caso de empate, sai primeiro quem chegou antes.
void chamarProximo() {
    cout << "\n[TODO Mateus] Remover o paciente de maior prioridade.\n";
}

// opcao 7: mostra a fila inteira na ordem em que sera atendida.
// esta opcao influencia a escolha da estrutura: com heap binario a fila
// nao sai ordenada sem esvaziar uma copia. com lista ordenada, sai direto.
void verEstadoDaFila() {
    cout << "\n[TODO Mateus] Listar a fila na ordem de atendimento.\n";
}

// ============================================================
// PAULO - prontuario e indice temporal
// ============================================================

// opcao 5: percorre o historico do paciente nos dois sentidos.e o que justifica a lista duplamente encadeada em vez da simples.
void navegarHistorico() {
    cout << "\n[TODO Paulo] Percorrer a LDE (anterior / proxima).\n";
}

// opcao 6: conta quantas vezes cada diagnostico aparece no historico do proprio paciente.
void verRecorrencia() {
    cout << "\n[TODO Paulo] Contar ocorrencias de cada diagnostico do paciente.\n";
}

// ============================================================

// le um numero digitado pelo usuario com seguranca.
//
// problema que resolve: se o usuario digitar uma letra, o cin entra em estado de falha e para de ler qualquer coisa. o laco do menu ficaria repetindo para sempre 
// cin.clear() tira o cin do estado de falha.
// cin.ignore(10000, '\n') descarta o que sobrou ate o fim da linha.
int lerOpcao() {
    int valor;
    while (!(cin >> valor)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Entrada invalida. Digite um numero: ";
    }
    return valor;
}

int main() {
    int opcao;

    // do-while em vez de while: garante que o menu aparece pelo menos uma vez antes de qualquer teste de saida.
    do {
        exibirMenu();
        opcao = lerOpcao();

        switch (opcao) {
            case 1: cadastrarPaciente();   break;
            case 2: realizarTriagem();     break;
            case 3: chamarProximo();       break;
            case 4: consultarProntuario(); break;
            case 5: navegarHistorico();    break;
            case 6: verRecorrencia();      break;
            case 7: verEstadoDaFila();     break;
            case 8: relatorios();          break;
            case 9: salvarCarregarDados(); break;
            case 0: cout << "\nEncerrando.\n"; break;

            // default trata qualquer numero fora das opcoes acima
            default: cout << "\nOpcao invalida.\n"; break;
        }
    } while (opcao != 0);

    return 0;  
}