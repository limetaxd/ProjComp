/*
(Paulo) Lista Duplamente Encadeada (LDE) com o historico de atendimentos de UM paciente.

Existe um Historico por paciente, achado pelo cpf atraves de uma TabelaHash<string,
Historico*> (ver main.cpp). Por isso esta classe nunca guarda nem recebe cpf, e nunca
filtra nada por paciente aqui dentro: cada objeto Historico ja pertence, por construcao,
a um unico paciente - ele so sabe navegar os atendimentos que sao dele mesmo.
*/

#ifndef HISTORICO_H
#define HISTORICO_H

#include "Atendimento.h"
#include <stdexcept>

class Historico{
    private:
        struct No{
            Atendimento atendimento;
            No* anterior;
            No* proximo;

            No(const Atendimento& a) : atendimento(a), anterior(nullptr), proximo(nullptr) {}
        };

        No* inicio;     // primeiro atendimento (o mais antigo)
        No* fim;        // ultimo atendimento (o mais recente)
        No* cursor;     // onde a navegacao esta parada agora - e o que faz a posicao nao se perder entre uma chamada de navegarHistorico() e outra
        int quantidade;

        Historico(const Historico&);
        Historico& operator=(const Historico&);

    public:
        // historico novo, vazio - usado quando um paciente ainda nao tem nenhum atendimento registrado.
        Historico() : inicio(nullptr), fim(nullptr), cursor(nullptr), quantidade(0) {}

        // destrutor: percorre a cadeia inteira liberando um No por vez. 
        ~Historico() {
            No* atual = inicio;
            while (atual != nullptr) {
                No* proximo = atual->proximo;
                delete atual;
                atual = proximo;
            }
        }

        void inserir(Atendimento *novoAtendimento){
            No* novoNo = new No(*novoAtendimento);

            if(inicio == nullptr){
                inicio = novoNo;
                fim = novoNo;
                cursor = novoNo;
            } 
            
            else {
                novoNo->anterior = fim;
                fim->proximo = novoNo;
                fim = novoNo;
                cursor = novoNo;
            }

            quantidade++;
        }

        bool anterior(){
            if(cursor == nullptr){
                return false;
            }

            if(cursor->anterior != nullptr){
                cursor = cursor->anterior;
                return true;
            }

            return false;
        }

        bool proximo(){
            if(cursor == nullptr){
                return false;
            }

            if(cursor->proximo != nullptr){
                cursor = cursor->proximo;
                return true;
            }

            return false;
        }

        // Coloca o cursor no primeiro atendimento (o mais antigo)
        bool irParaInicio() {
            cursor = inicio;
            return cursor != nullptr; // Retorna true se a lista não estiver vazia
        }

        // Coloca o cursor no último atendimento (o mais recente)
        bool irParaFim() {
            cursor = fim;
            return cursor != nullptr;
        }

        Atendimento atual(){
            if(cursor != nullptr){
                return cursor->atendimento;
            }

            throw std::runtime_error("Lista vazia ou cursor nulo.");
        }
};

#endif
