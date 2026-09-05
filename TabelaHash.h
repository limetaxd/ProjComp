/*
Tabela Hash generica com tratamento de colisao por encadeamento separado
Serve para guardar pares chave-valor e encontrar qualquer um deles em tempo constante, sem precisar percorrer a colecao inteira.
No sistema, a chave e o cpf e o valor e o paciente. 
Tudo fica no .h (sem .cpp) porque a classe e um template: o compilador so consegue gerar o codigo quando sabe quais tipos serao usados, e para 
isso ele precisa enxergar a implementacao inteira no momento do #include.
*/
#ifndef TABELA_HASH_H
#define TABELA_HASH_H

#include <string>
using namespace std;

/*
funcao de dispersao: transforma uma string em um numero grande. e o coracao da tabela hash. esse numero, dividido pela capacidade,
diz em qual posicao do vetor a chave deve ficar.
*/

inline unsigned long hashCode(const string& chave){
    unsigned long h=0;
    for (size_t i=0; i<chave.size();i++){
        h=h*31 +(unsigned char)chave[i];
    }
    return h;
}

template <typename K, typename V>
class TabelaHash{
    private: 
        struct No{
            K chave;
            V valor;
            No* proximo;    // aponta para o proximo no da mesma posicao, ou nullptr

            No(const K& c, const V& v) : chave(c), valor (v), proximo(nullptr) {}
        };
        
        No** tabela;    // vetor de ponteiros: cada posicao aponta para o inicio de uma lista
        int capacidade; // quantas posicoes o vetor tem
        int totalElementos; // quantos pares estao guardados no total
        int totalColisoes;  // contador para os relatorios e benchmarks

        // calcula em qual posicao do vetor a chave deve ficar. o "% garante que o resultado sempre cai dentro do intervalo de 0 ate capacidade - 1.
        int indiceDe(const K& chave) const{
            return (int)(hashCode(chave)%(unsigned long)capacidade);
        }
        /*construtor de copia e operador de atribuicao declarados aqui no private e sem implementacao*/
        TabelaHash(const TabelaHash&);
        TabelaHash& operator=(const TabelaHash&);

    public:
        TabelaHash(int capacidade= 101){
            this->capacidade= capacidade;
            this->totalElementos= 0;
            this->totalColisoes= 0;

            tabela = new No*[capacidade];   // aloca o vetor de ponteiros
            for(int i=0;i<capacidade;i++){
                tabela[i]=nullptr;
            }
        }
        /*
        destrutor: roda automaticamente quando a tabela deixa de existir. precisa liberar cada no de cada lista, um por um, senao a memoria alocada com new fica ocupada ate o programa terminar
        */
        ~TabelaHash(){
            for (int i=0; i<capacidade; i++){
                No* atual=tabela[i];
                while(atual!=nullptr){
                    //guarda o proximo antes de apagar o atual
                    No* proximo = atual -> proximo;
                    delete atual;
                    atual = proximo;
                }
            }   
            delete[] tabela;
        }
        // insere um par chave-valor. retorna false se a chave ja existir. isso e o que impede cadastrar dois pacientes com o mesmo cpf.
        bool inserir(const K& chave, const V& valor){
            int i= indiceDe(chave);

        // primeiro percorre a lista daquela posicao procurando a chave
        No* atual = tabela[i];
        while (atual != nullptr) {
            if (atual->chave == chave) {
                return false;   // ja existe
            }
            atual = atual->proximo;
        }
 
        // se a posicao ja tinha alguem, esta insercao e uma colisao
        if (tabela[i] != nullptr) {
            totalColisoes++;
        }
 
        // insere no inicio da lista: e O(1), nao precisa percorrer ate o fim
        No* novo = new No(chave, valor);
        novo->proximo = tabela[i];
        tabela[i] = novo;
        totalElementos++;
        return true;
    }
 
    /*
    procura uma chave.
    devolve o endereco do valor guardado, para quem chamou poder ler e ate alterar o objeto sem fazer copia. devolve nullptr se nao achar.
    quem usar precisa sempre testar se o retorno e nullptr antes de acessar, senao o programa quebra.
    */ 
    V* buscar(const K& chave) {
        int i = indiceDe(chave);            // vai direto na posicao certa
        No* atual = tabela[i];
        while (atual != nullptr) {          // percorre so a lista daquela posicao
            if (atual->chave == chave) {
                return &(atual->valor);     // & devolve o endereco do valor
            }
            atual = atual->proximo;
        }
        return nullptr;                     // chave nao encontrada
    }
 
    // remove um par pela chave. retorna false se a chave nao existir.
    bool remover(const K& chave) {
        int i = indiceDe(chave);
        No* atual = tabela[i];
        No* anterior = nullptr;     // guarda o no de tras para religar a lista
 
        while (atual != nullptr) {
            if (atual->chave == chave) {
                if (anterior == nullptr) {
                    // era o primeiro da lista: a posicao passa a apontar para o segundo
                    tabela[i] = atual->proximo;
                } else {
                    // estava no meio ou no fim: o anterior pula por cima dele
                    anterior->proximo = atual->proximo;
                }
                delete atual;       // so agora libera a memoria
                totalElementos--;
                return true;
            }
            anterior = atual;
            atual = atual->proximo;
        }
        return false;
    }
 
    // quantos pares estao guardados
    int tamanho() const  { return totalElementos; }
 
    // quantas colisoes aconteceram. serve para o relatorio e para comparar funcoes de dispersao diferentes nos benchmarks.
    int colisoes() const { return totalColisoes; }
 
    // fator de carga = elementos por posicao do vetor. abaixo de 1.0 a busca fica proxima de tempo constante. acima disso as listas ficam longas e a busca comeca a virar linear.
    double fatorDeCarga() const {
        return (double)totalElementos / (double)capacidade;
    }
};
 
#endif
