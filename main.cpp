// ponto de entrada do programa e menu principal.
// arquivo compartilhado pelos tres integrantes.

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <ctime>
#include <windows.h>
#include "Gravidade.h"
#include "Paciente.h"
#include "Atendimento.h"
#include "TabelaHash.h"

using namespace std;

// indice global de pacientes: cpf -> paciente.
// e o que permite achar qualquer paciente em tempo constante, sem varrer a base inteira. modulo do arthur.

// declarado fora de qualquer funcao (variavel global) para que todas as funcoes do menu possam usar a mesma tabela.
TabelaHash<string, Paciente> indicePacientes;
//Mateus: Esse vector guarda os atendimentos (triagens) realizados e é usado para a ordenação com base no nível de gravidade.
vector<Atendimento> atendimentos;
//Mateus: Fila normal organizada por ordem de chegada, visto que serão 2 tipos de fila, uma prioritaria e uma normal.
//Mateus: As filas guardam Atendimento (e nao Paciente), o paciente e achado depois pelo cpf na tabela hash.
queue<Atendimento> FilaNormal;
queue<Atendimento> FilaPrioritaria;
//Mateus: Usado no sistema de 2P, 1C
//Mateus: Tenho que salvar isso no arquivo de salvamento!!
int pc = 0;

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

// opcao 9: gravar e ler os dados em arquivo, para nao perder tudo ao fechar.
void salvarCarregarDados() {
    /*
    TODO(arthur): gravar e ler os dados em um arquivo texto.
    definir com mateus e paulo o que entra no arquivo: so os pacientes, ou tambem os atendimentos e o estado da fila.
    */
    cout << "\n[TODO Arthur] Persistencia em arquivo.\n";
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

//Mateus: PROVISORIO - data do atendimento pega do relogio do sistema (dd/mm/aaaa).
// o formato (e se o usuario digita ou nao) ainda sera definido com o Paulo, por isso fica isolado nesta funcao.
string dataAtual(){
    time_t agora = time(0);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", localtime(&agora));
    return string(buffer);
}

// opcao 2: classifica o risco do paciente e coloca na fila de prioridade(Feito!).
void realizarTriagem() {
    string cpf;
    string situa;
    string prob;
    int grav;

    //Mateus: pede o cpf de quem esta sendo triado e busca na tabela hash, assim a triagem nao depende do ultimo cadastrado.
    cout << "\nCPF do paciente: ";
    cin >> cpf;
    Paciente* p = indicePacientes.buscar(cpf);
    if(p == nullptr){
        cout << "Paciente nao encontrado. Cadastre o paciente antes da triagem.\n";
        return;
    }

    //Mateus: descarta o \n que o cin >> deixou no buffer, senao o primeiro getline le uma linha vazia.
    cin.ignore(1000, '\n');

    cout << "Insira uma descrição da situação do paciente:\n";
    getline(cin, situa);

    cout << "Insira o problema ou doença que o paciente tem:\n";
    getline(cin, prob);

    while(true){
    cout << "Insira o nivel de gravidade do paciente de acordo com o número de cada cor: \n0.Vermelho \n1.Laranja \n2.Amarelo \n3.Verde \n4.Azul\n";
    if(!(cin >> grav) || grav < 0 || grav > 4){
        cout << "Insira um valor entre 0 e 4!\n";
        cin.clear();
        cin.ignore(1000, '\n');
        continue;
    }
    break;
    }
    
    //Mateus: grav continua sendo lido como int, mas aqui ele vira o enum Gravidade oficial do projeto.
    Gravidade gravidade = (Gravidade)grav;

    //Mateus: prob vira diagnostico e situa vira observacoes.
    Atendimento novoAtendimento(p->getCpf(), dataAtual(), gravidade, prob, situa);
    atendimentos.push_back(novoAtendimento);

    if(gravidade < AMARELO){
        FilaPrioritaria.push(novoAtendimento);
    }
    else{
        FilaNormal.push(novoAtendimento);
    }
    cout << "Triagem realizada: " << nomeGravidade(gravidade) << ".\n";

    //Mateus: Organizei o vector global atendimentos para ficar mais facil de trabalhar com ele
    //Mateus: Quanto MENOR o valor do enum, MAIOR a urgencia, entao o '<' deixa os casos mais graves no inicio.
    //Mateus: Ordenacao por insercao: so troca de lugar quando o anterior e MENOS grave, entao quem chegou primeiro continua na frente no empate (FIFO).
    for(int i = 1; i < atendimentos.size(); i++){
        Atendimento chave = atendimentos[i];
        int j = i - 1;
        while(j >= 0 && chave.getGravidade() < atendimentos[j].getGravidade()){
            atendimentos[j + 1] = atendimentos[j];
            j--;
        }
        atendimentos[j + 1] = chave;
    }
}
//Considerações: Criar um sistema de senha, ao invés de chamar pelo nome e CPF, devo chamar uma senha, apesar de interessante, não sei se é a melhor forma

//Opção 3: Criar um sistema em que ou eu faço a chamda do proximo seguindo o padrão original, 2 pessoas prefrerencial e uma Normal sempre nessa ordem
// Em caso de empate na prioritaria sai quem chegou primeira
//Ou que eu posso fazer uma chamada personalizada, em que posso escolher de qual fila será o proximo a ser chamado.
//Mateus: A fila guarda Atendimento, que so tem o cpf. O nome vem da tabela hash, buscando pelo cpf.
void exibirChamada(const Atendimento& atendimento){
    Paciente* p = indicePacientes.buscar(atendimento.getCpfPaciente());
    if(p == nullptr){
        cout << "Proximo Paciente: Paciente nao encontrado\n";
    }
    else{
        cout << "Proximo Paciente: " << p->getNome() << endl;
    }
    cout << "CPF: " << atendimento.getCpfPaciente() << endl;
}

void chamarProximo(){
    //Mateus: se nao tem ninguem esperando, nao ha o que chamar (front() e pop() em fila vazia derrubam o programa).
    if(FilaPrioritaria.empty() && FilaNormal.empty()){
        cout << "Nao ha pacientes aguardando.\n";
        return;
    }

    //Mateus: o while(true) ficou so para validar a entrada. Depois de uma opcao valida ele sai com break.
    int op = 0;
    while(true){
    cout << "Escolha entre o método de chamada padrão ou a chamada personalizada:\n";
    cout << "1. Chamada Padrão(2P,1C)\n";
    cout << "2. Chamada Personalizada\n";
    if(!(cin >> op) || op < 1 || op > 2){
        cout << "Escolha entre a opção 1 e 2!!\n";
        cin.clear();
        cin.ignore(1000, '\n');
        continue;
    }
    break;
    }

    switch(op){
        // Mateus: Sistema provisorio no Case1, vou buscar ver alguns outro métodos!
        case 1:{
            //Mateus: chama da prioritaria enquanto pc < 2. Se uma das filas estiver vazia, chama da outra.
            if((pc < 2 && !FilaPrioritaria.empty()) || FilaNormal.empty()){
                exibirChamada(FilaPrioritaria.front());
                FilaPrioritaria.pop();
                pc++;
            }
            else{
                exibirChamada(FilaNormal.front());
                FilaNormal.pop();
                pc = 0;
            }
            break;
        }
        case 2:{
            int opchamada = 0;
            while(true){
            cout << "Escolha uma das seguintes opções!\n";
            cout << "1. Chamar Paciente da Fila Normal!\n";
            cout << "2. Chamar Paciente da Fila Prioritária!\n";
            if(!(cin >> opchamada) || opchamada < 1 || opchamada > 2){
                cout << "Escolha entre a opção 1 e 2!!\n";
                cin.clear();
                cin.ignore(1000, '\n');
                continue;
            }
            break;
            }

            switch(opchamada){
                case 1:{
                    if(FilaNormal.empty()){
                        cout << "A fila normal esta vazia.\n";
                    }
                    else{
                        exibirChamada(FilaNormal.front());
                        FilaNormal.pop();
                    }
                    break;
                }
                case 2:{
                    if(FilaPrioritaria.empty()){
                        cout << "A fila prioritaria esta vazia.\n";
                    }
                    else{
                        exibirChamada(FilaPrioritaria.front());
                        FilaPrioritaria.pop();
                    }
                    break;
                }
            }
            break;
        }
    }
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
    SetConsoleOutputCP(CP_UTF8);
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