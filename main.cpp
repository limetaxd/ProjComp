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
//Mateus: Serão 2 tipos de fila, uma prioritaria (Vermelho e Laranja) e uma normal (Amarelo, Verde e Azul), as duas saem por gravidade.
//Mateus: As filas guardam Atendimento (e nao Paciente), o paciente e achado depois pelo cpf na tabela hash.
queue<Atendimento> FilaNormal;
queue<Atendimento> FilaPrioritaria;

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

//Mateus: o relatorio ordenado fica la embaixo, na parte do Mateus. Essa linha (prototipo) avisa o compilador que a funcao existe,
//Mateus: senao ele reclama, porque o relatorios() vem antes dela no arquivo.
void relatoriosOrdenados();

// opcao 8: numeros do sistema. as tres primeiras linhas servem de material para a analise de desempenho da tabela hash.
void relatorios() {
    cout << "\nPacientes cadastrados: " << indicePacientes.tamanho() << "\n";
    cout << "Colisoes na tabela hash: " << indicePacientes.colisoes() << "\n";
    cout << "Fator de carga: " << indicePacientes.fatorDeCarga() << "\n";

    //Mateus: relatorios ordenados com mergesort e quicksort (Feito!)
    relatoriosOrdenados();
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

//Mateus: Ordenacao por insercao pela gravidade. Antes ela ficava dentro do realizarTriagem, passei para uma funcao
//Mateus: porque agora ela tambem e usada para organizar as duas filas.
//Mateus: Quanto MENOR o valor do enum, MAIOR a urgencia, entao o '<' deixa os casos mais graves no inicio.
//Mateus: So troca de lugar quando o anterior e MENOS grave, entao quem chegou primeiro continua na frente no empate (FIFO).
void ordenarPorGravidade(vector<Atendimento>& v){
    for(int i = 1; i < v.size(); i++){
        Atendimento chave = v[i];
        int j = i - 1;
        while(j >= 0 && chave.getGravidade() < v[j].getGravidade()){
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = chave;
    }
}

//Mateus: As duas filas saem por gravidade. Na prioritaria um Vermelho que chega depois passa na frente do Laranja,
//Mateus: e na normal o Amarelo passa na frente do Verde, que passa na frente do Azul. Empate: quem chegou primeiro.
//Mateus: A queue nao deixa mexer no meio, entao passo tudo para um vector, ordeno e devolvo para a fila na ordem certa.
//Mateus: A fila vem com & para a funcao mexer na fila de verdade (FilaPrioritaria ou FilaNormal), e nao numa copia.
void organizarFila(queue<Atendimento>& fila){
    vector<Atendimento> aux;
    while(!fila.empty()){
        aux.push_back(fila.front());
        fila.pop();
    }
    ordenarPorGravidade(aux);
    for(int i = 0; i < aux.size(); i++){
        fila.push(aux[i]);
    }
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
        organizarFila(FilaPrioritaria);     //Mateus: Vermelho passa na frente do Laranja
    }
    else{
        FilaNormal.push(novoAtendimento);
        organizarFila(FilaNormal);          //Mateus: Amarelo, depois Verde, depois Azul
    }
    cout << "Triagem realizada: " << nomeGravidade(gravidade) << ".\n";

    //Mateus: Organizei o vector global atendimentos para ficar mais facil de trabalhar com ele
    ordenarPorGravidade(atendimentos);
}
//Considerações: Criar um sistema de senha, ao invés de chamar pelo nome e CPF, devo chamar uma senha, apesar de interessante, não sei se é a melhor forma

//Opção 3: Criar um sistema em que ou eu faço a chamda do proximo seguindo o padrão original de primeiro chamar os preferenciais e depois os Normais
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
    cout << "1. Chamada Padrão\n";
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
            //Mateus: chama primeiro da prioritaria (ja ordenada por gravidade). Se ela estiver vazia, chama da normal.
            if(!(FilaPrioritaria.empty())){
                exibirChamada(FilaPrioritaria.front());
                FilaPrioritaria.pop();
                
            }
            else{
                exibirChamada(FilaNormal.front());
                FilaNormal.pop();
                
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
//Mateus: Feito! Usei o exibirChamada como base.
//Mateus: A queue so deixa ver o front(), entao para ver todo mundo tem que ir tirando com pop().
//Mateus: Para nao apagar a fila de verdade, a funcao recebe a fila SEM o & (por valor), ou seja, ela recebe uma COPIA.
//Mateus: Os pop() acontecem so na copia e a fila original continua igual.
void mostrarFila(queue<Atendimento> copia, string nomeFila){
    cout << "\n--- " << nomeFila << " (" << copia.size() << " aguardando) ---\n";
    if(copia.empty()){
        cout << "Ninguem aguardando.\n";
        return;
    }
    int cont = 1;
    while(!copia.empty()){
        Atendimento atendimento = copia.front();
        Paciente* p = indicePacientes.buscar(atendimento.getCpfPaciente());
        cout << cont << ". ";
        if(p == nullptr){
            cout << "Paciente nao encontrado";
        }
        else{
            cout << p->getNome();
        }
        cout << " | CPF: " << atendimento.getCpfPaciente();
        cout << " | Gravidade: " << nomeGravidade(atendimento.getGravidade()) << endl;
        copia.pop();
        cont++;
    }
}

void verEstadoDaFila() {
    if(FilaPrioritaria.empty() && FilaNormal.empty()){
        cout << "\nNao ha pacientes aguardando.\n";
        return;
    }
    //Mateus: Na chamada padrao a prioritaria e chamada toda antes, entao mostrar nessa ordem ja e a ordem de atendimento.
    cout << "\nOrdem de atendimento (chamada padrao): primeiro a fila prioritaria, depois a normal.\n";
    mostrarFila(FilaPrioritaria, "Fila Prioritaria");
    mostrarFila(FilaNormal, "Fila Normal");
}

// ============================================================
// MATEUS - relatorios ordenados (opcao 8)
// ============================================================

//Mateus: Os relatorios ordenam uma COPIA do vector atendimentos, para nao baguncar a ordem por gravidade que a triagem usa.
//Mateus: O contador de comparacoes serve para comparar o desempenho do Merge Sort com o do Quick Sort no relatorio.
int comparacoes = 0;

//Mateus: MERGE SORT pelo diagnostico (ordem alfabetica).
//Mateus: Escolhi o Merge aqui porque ele e ESTAVEL: atendimentos com o mesmo diagnostico continuam na ordem
//Mateus: em que estavam no vector, ou seja, os mais graves primeiro.
//Mateus: intercalar junta duas metades que ja estao ordenadas: v[inicio..meio] e v[meio+1..fim].
void intercalar(vector<Atendimento>& v, int inicio, int meio, int fim){
    vector<Atendimento> temp;
    int i = inicio;
    int j = meio + 1;
    while(i <= meio && j <= fim){
        comparacoes++;
        if(v[i].getDiagnostico() <= v[j].getDiagnostico()){     //o '=' e o que deixa estavel
            temp.push_back(v[i]);
            i++;
        }
        else{
            temp.push_back(v[j]);
            j++;
        }
    }
    while(i <= meio){       //sobrou na metade esquerda
        temp.push_back(v[i]);
        i++;
    }
    while(j <= fim){        //sobrou na metade direita
        temp.push_back(v[j]);
        j++;
    }
    for(int k = 0; k < temp.size(); k++){
        v[inicio + k] = temp[k];    //inicio + k porque essa parte do vector nao comeca no 0
    }
}
//Mateus: Funcao recursiva: divide no meio ate sobrar 1 elemento (que ja esta ordenado) e depois vai juntando.
void mergeSort(vector<Atendimento>& v, int inicio, int fim){
    if(inicio >= fim){
        return;
    }
    int meio = (inicio + fim) / 2;
    mergeSort(v, inicio, meio);
    mergeSort(v, meio + 1, fim);
    intercalar(v, inicio, meio, fim);
}

//Mateus: QUICK SORT pelo CPF, para deixar os atendimentos de um mesmo paciente juntos.
//Mateus: particionar escolhe o ultimo como pivo, joga os menores que ele para a esquerda e coloca o pivo logo depois deles.
//Mateus: O pivo fica na posicao final dele, e a funcao devolve essa posicao.
int particionar(vector<Atendimento>& v, int inicio, int fim){
    string pivo = v[fim].getCpfPaciente();
    int i = inicio - 1;
    for(int j = inicio; j < fim; j++){
        comparacoes++;
        if(v[j].getCpfPaciente() < pivo){
            i++;
            Atendimento aux = v[i];
            v[i] = v[j];
            v[j] = aux;
        }
    }
    Atendimento aux = v[i + 1];
    v[i + 1] = v[fim];
    v[fim] = aux;
    return i + 1;
}
//Mateus: Ordena a esquerda e a direita do pivo (o pivo ja esta no lugar certo, por isso p - 1 e p + 1).
void quickSort(vector<Atendimento>& v, int inicio, int fim){
    if(inicio >= fim){
        return;
    }
    int p = particionar(v, inicio, fim);
    quickSort(v, inicio, p - 1);
    quickSort(v, p + 1, fim);
}

//Mateus: mostra uma linha do relatorio
void exibirAtendimento(const Atendimento& atendimento){
    Paciente* p = indicePacientes.buscar(atendimento.getCpfPaciente());
    if(p == nullptr){
        cout << "Paciente nao encontrado";
    }
    else{
        cout << p->getNome();
    }
    cout << " | CPF: " << atendimento.getCpfPaciente();
    cout << " | " << atendimento.getData();
    cout << " | " << nomeGravidade(atendimento.getGravidade());
    cout << " | Diagnostico: " << atendimento.getDiagnostico() << endl;
}

// ============================================================
// MATEUS - analise de doencas: ocorrencia, preparo e previsao
// ============================================================
//Mateus: Essa parte usa o vector atendimentos (os atendimentos feitos e, quando a opcao de salvar/carregar do Arthur
//Mateus: estiver pronta, tambem os carregados do arquivo). Nao muda nada nos dados, so le e calcula.

//Mateus: Deixa o diagnostico todo em minusculo, para "Gripe", "GRIPE" e "gripe" contarem como a mesma doenca.
string padronizar(string texto){
    for(int i = 0; i < texto.size(); i++){
        if(texto[i] >= 'A' && texto[i] <= 'Z'){
            texto[i] = texto[i] - 'A' + 'a';    //na tabela ASCII a minuscula fica 32 posicoes depois da maiuscula
        }
    }
    return texto;
}

//Mateus: Transforma "dd/mm/aaaa" em um numero aaaammdd (ex.: 01/11/2026 vira 20261101).
//Mateus: Assim da para comparar datas com < e >, porque o ano vem primeiro, depois o mes e depois o dia.
int dataParaNumero(string data){
    int dia = stoi(data.substr(0, 2));      //substr(inicio, tamanho) pega um pedaco da string; stoi converte para int
    int mes = stoi(data.substr(3, 2));
    int ano = stoi(data.substr(6, 4));
    return ano * 10000 + mes * 100 + dia;
}

//Mateus: Arredonda para 1 casa decimal (52.381 vira 52.4): multiplica por 10, soma 0.5, corta a parte quebrada com (int) e divide por 10.
double arredondar(double x){
    return (int)(x * 10 + 0.5) / 10.0;
}

//Mateus: Guarda os numeros de UMA doenca. Os atributos ficam publicos porque e so um "pacote" de contadores, igual um No.
class EstatisticaDoenca{
public:
    string nome;
    int total;
    int porCor[5];          //quantos casos em cada cor (o enum e o indice: 0 = Vermelho ... 4 = Azul)
    int somaIdade;
    int qtdComIdade;        //so conta a idade de quem foi achado na tabela hash
    EstatisticaDoenca(){
        nome = "N/A";
        total = 0;
        for(int i = 0; i < 5; i++){
            porCor[i] = 0;
        }
        somaIdade = 0;
        qtdComIdade = 0;
    }
    int graves(){           //Vermelho + Laranja
        return porCor[VERMELHO] + porCor[LARANJA];
    }
    Gravidade corMaisComum(){
        int maior = 0;
        for(int i = 1; i < 5; i++){
            if(porCor[i] > porCor[maior]){
                maior = i;
            }
        }
        return (Gravidade)maior;
    }
};

//Mateus: Percorre os atendimentos e monta um vector com uma EstatisticaDoenca para cada doenca diferente.
//Mateus: Busca linear no vector de estatisticas: se a doenca ja existe soma nela, se nao cria uma nova.
vector<EstatisticaDoenca> calcularEstatisticas(){
    vector<EstatisticaDoenca> estat;
    for(int i = 0; i < atendimentos.size(); i++){
        string doenca = padronizar(atendimentos[i].getDiagnostico());
        int pos = -1;
        for(int j = 0; j < estat.size(); j++){
            if(estat[j].nome == doenca){
                pos = j;
                break;
            }
        }
        if(pos == -1){
            EstatisticaDoenca nova;
            nova.nome = doenca;
            estat.push_back(nova);
            pos = estat.size() - 1;
        }
        estat[pos].total++;
        estat[pos].porCor[atendimentos[i].getGravidade()]++;
        Paciente* p = indicePacientes.buscar(atendimentos[i].getCpfPaciente());
        if(p != nullptr){
            estat[pos].somaIdade += p->getIdade();
            estat[pos].qtdComIdade++;
        }
    }
    //Mateus: ordena do mais frequente para o menos (insertion, estavel). Empate: ordem alfabetica.
    for(int i = 1; i < estat.size(); i++){
        EstatisticaDoenca chave = estat[i];
        int j = i - 1;
        while(j >= 0 && (estat[j].total < chave.total || (estat[j].total == chave.total && estat[j].nome > chave.nome))){
            estat[j + 1] = estat[j];
            j--;
        }
        estat[j + 1] = chave;
    }
    return estat;
}

//Mateus: Ranking das doencas: casos, porcentagem do total, porcentagem de casos graves, idade media e cor mais comum.
void rankingDoencas(){
    vector<EstatisticaDoenca> estat = calcularEstatisticas();
    int totalGeral = atendimentos.size();
    cout << "\n--- Ocorrencia de doencas (" << totalGeral << " atendimentos) ---\n";
    for(int i = 0; i < estat.size(); i++){
        //Mateus: (double) antes da divisao para nao dar divisao inteira (3 / 4 daria 0 sem ele)
        double porcentagem = (double)estat[i].total / totalGeral * 100;
        double porcGraves = (double)estat[i].graves() / estat[i].total * 100;
        cout << i + 1 << ". " << estat[i].nome << ": " << estat[i].total << " caso(s) | " << arredondar(porcentagem) << "% do total";
        cout << " | " << arredondar(porcGraves) << "% graves | cor mais comum: " << nomeGravidade(estat[i].corMaisComum());
        if(estat[i].qtdComIdade > 0){
            cout << " | idade media: " << arredondar((double)estat[i].somaIdade / estat[i].qtdComIdade);
        }
        cout << endl;
    }
}

//Mateus: Preparo: para cada doenca, quantos casos de cada cor e a porcentagem. Ajuda a saber quantos leitos de
//Mateus: emergencia separar (ex.: se 40% dos casos de "infarto" sao Vermelho, precisa de sala vermelha livre).
void preparoPorGravidade(){
    vector<EstatisticaDoenca> estat = calcularEstatisticas();
    cout << "\n--- Preparo: gravidade de cada doenca ---\n";
    for(int i = 0; i < estat.size(); i++){
        cout << estat[i].nome << " (" << estat[i].total << "):";
        for(int c = 0; c < 5; c++){
            if(estat[i].porCor[c] > 0){
                cout << " " << nomeGravidade((Gravidade)c) << " " << estat[i].porCor[c];
                cout << " (" << arredondar((double)estat[i].porCor[c] / estat[i].total * 100) << "%)";
            }
        }
        cout << endl;
    }
}

//Mateus: Lista as datas diferentes que aparecem nos atendimentos, ja em ordem (insertion pela data em numero).
vector<string> datasDistintas(){
    vector<string> datas;
    for(int i = 0; i < atendimentos.size(); i++){
        bool existe = false;
        for(int j = 0; j < datas.size(); j++){
            if(datas[j] == atendimentos[i].getData()){
                existe = true;
                break;
            }
        }
        if(!existe){
            datas.push_back(atendimentos[i].getData());
        }
    }
    for(int i = 1; i < datas.size(); i++){
        string chave = datas[i];
        int j = i - 1;
        while(j >= 0 && dataParaNumero(datas[j]) > dataParaNumero(chave)){
            datas[j + 1] = datas[j];
            j--;
        }
        datas[j + 1] = chave;
    }
    return datas;
}

//Mateus: Quantos casos de uma doenca em uma data
int casosNoDia(string doenca, string data){
    int cont = 0;
    for(int i = 0; i < atendimentos.size(); i++){
        if(atendimentos[i].getData() == data && padronizar(atendimentos[i].getDiagnostico()) == doenca){
            cont++;
        }
    }
    return cont;
}

/*
Mateus: Comparacao entre os dias e previsao.
Para cada doenca:
- mostra quantos casos teve em cada dia
- compara o ULTIMO dia com a MEDIA dos dias anteriores -> tendencia (subindo, estavel ou caindo)
- previsao para o proximo dia = media dos ultimos 3 dias (media movel). E uma estimativa simples, nao e exata.
- ALERTA de possivel surto: ultimo dia com pelo menos 3 casos e pelo menos o dobro da media anterior.
Precisa de atendimentos em pelo menos 2 datas diferentes para comparar.
*/
void tendenciaEPrevisao(){
    vector<string> datas = datasDistintas();
    if(datas.size() < 2){
        cout << "\nPara comparar e prever e preciso ter atendimentos em pelo menos 2 dias diferentes.\n";
        return;
    }
    vector<EstatisticaDoenca> estat = calcularEstatisticas();
    cout << "\n--- Comparacao entre dias e previsao (" << datas.size() << " dias, de " << datas[0] << " ate " << datas[datas.size() - 1] << ") ---\n";
    for(int i = 0; i < estat.size(); i++){
        string doenca = estat[i].nome;
        vector<int> casos;      //casos[d] = casos dessa doenca no dia datas[d]
        for(int d = 0; d < datas.size(); d++){
            casos.push_back(casosNoDia(doenca, datas[d]));
        }
        cout << "\n" << doenca << ": ";
        for(int d = 0; d < casos.size(); d++){
            cout << datas[d].substr(0, 5) << "=" << casos[d] << " ";   //substr(0, 5) mostra so dd/mm
        }
        cout << endl;

        //media dos dias anteriores ao ultimo
        int ultimo = casos[casos.size() - 1];
        double somaAnteriores = 0;
        for(int d = 0; d < casos.size() - 1; d++){
            somaAnteriores += casos[d];
        }
        double mediaAnterior = somaAnteriores / (casos.size() - 1);
        cout << "   Ultimo dia: " << ultimo << " | media dos dias anteriores: " << arredondar(mediaAnterior);
        if(ultimo > mediaAnterior * 1.2){
            cout << " | tendencia: SUBINDO\n";
        }
        else if(ultimo < mediaAnterior * 0.8){
            cout << " | tendencia: CAINDO\n";
        }
        else{
            cout << " | tendencia: ESTAVEL\n";
        }

        //previsao: media dos ultimos 3 dias (ou de todos, se tiver menos de 3)
        int qtdDias = 3;
        if(casos.size() < 3){
            qtdDias = casos.size();
        }
        double soma = 0;
        for(int d = casos.size() - qtdDias; d < casos.size(); d++){
            soma += casos[d];
        }
        cout << "   Previsao para o proximo dia: cerca de " << arredondar(soma / qtdDias) << " caso(s)\n";

        if(ultimo >= 3 && ultimo >= mediaAnterior * 2){
            cout << "   ALERTA: possivel surto de " << doenca << "! Preparar equipe e leitos.\n";
        }
    }
}

void analiseDoencas(){
    if(atendimentos.empty()){
        cout << "Nenhum atendimento para analisar.\n";
        return;
    }
    int op = 0;
    while(true){
    cout << "\nAnalise de doencas:\n";
    cout << "1. Ocorrencia e porcentagem de cada doenca\n";
    cout << "2. Preparo: gravidade de cada doenca\n";
    cout << "3. Comparacao entre dias e previsao\n";
    cout << "0. Voltar\n";
    if(!(cin >> op) || op < 0 || op > 3){
        cout << "Escolha entre a opção 0 e 3!!\n";
        cin.clear();
        cin.ignore(1000, '\n');
        continue;
    }
    break;
    }
    switch(op){
        case 1:{
            rankingDoencas();
            break;
        }
        case 2:{
            preparoPorGravidade();
            break;
        }
        case 3:{
            tendenciaEPrevisao();
            break;
        }
        case 0:{
            break;
        }
    }
}

void relatoriosOrdenados(){
    //Mateus: quantidade de atendimentos por cor. O enum vale de 0 a 4, entao ele serve direto como indice do vetor.
    int quantidade[5] = {0, 0, 0, 0, 0};
    for(int i = 0; i < atendimentos.size(); i++){
        quantidade[atendimentos[i].getGravidade()]++;
    }
    cout << "\nTotal de atendimentos: " << atendimentos.size() << "\n";
    for(int i = 0; i < 5; i++){
        cout << nomeGravidade((Gravidade)i) << ": " << quantidade[i] << "\n";
    }
    if(atendimentos.empty()){
        cout << "Nenhum atendimento realizado ainda.\n";
        return;
    }

    int op = 0;
    while(true){
    cout << "\nEscolha o relatorio:\n";
    cout << "1. Atendimentos por diagnostico (Merge Sort)\n";
    cout << "2. Atendimentos por paciente/CPF (Quick Sort)\n";
    cout << "3. Analise de doencas (ocorrencia, preparo e previsao)\n";
    cout << "0. Voltar\n";
    if(!(cin >> op) || op < 0 || op > 3){
        cout << "Escolha entre a opção 0 e 3!!\n";
        cin.clear();
        cin.ignore(1000, '\n');
        continue;
    }
    break;
    }

    vector<Atendimento> copia = atendimentos;
    comparacoes = 0;
    switch(op){
        case 1:{
            mergeSort(copia, 0, copia.size() - 1);
            cout << "\n--- Atendimentos por diagnostico ---\n";
            //Mateus: como o vector ja esta ordenado, os diagnosticos iguais ficam juntos. Da para contar a recorrencia so olhando o vizinho.
            int repeticoes = 1;
            for(int i = 0; i < copia.size(); i++){
                exibirAtendimento(copia[i]);
                if(i + 1 < copia.size() && copia[i + 1].getDiagnostico() == copia[i].getDiagnostico()){
                    repeticoes++;
                }
                else{
                    cout << "   -> " << copia[i].getDiagnostico() << ": " << repeticoes << " atendimento(s)\n";
                    repeticoes = 1;
                }
            }
            cout << "Comparacoes feitas pelo Merge Sort: " << comparacoes << "\n";
            break;
        }
        case 2:{
            quickSort(copia, 0, copia.size() - 1);
            cout << "\n--- Atendimentos por paciente (CPF) ---\n";
            for(int i = 0; i < copia.size(); i++){
                exibirAtendimento(copia[i]);
            }
            cout << "Comparacoes feitas pelo Quick Sort: " << comparacoes << "\n";
            break;
        }
        case 3:{
            analiseDoencas();
            break;
        }
        case 0:{
            break;
        }
    }
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