/*
define os niveis de risco do protocolo de manchester.
este arquivo e a interface compartilhada com o modulo do mateus (fila de prioridade).
NAO ALTERAR A ORDEM DOS VALORES SEM AVISAR O GRUPO.
*/

#ifndef GRAVIDADE_H
#define GRAVIDADE_H

#include<string>
using namespace std;

// enum e um tipo que so aceita um conjunto fixo de valores nomeados. aqui ele substitui numeros soltos: em vez de gravidade == 0, escrevemos gravidade == VERMELHO, que se le sozinho.
// os numeros nao sao arbitrarios: menor valor significa maior prioridade. isso permite ao mateus ordenar a fila comparando direto (a < b), sem precisar de tabela de conversao.

enum Gravidade{
    VERMELHO =0,
    LARANJA =1,
    AMARELO =2,
    VERDE =3,
    AZUL =4
};

// "inline" e necessario porque essa funcao tem corpo dentro de um .h. sem o inline, se dois arquivos incluissem o gravidade.h, o compilador reclamaria que a funcao foi definidade 2 vezes

inline string nomeGravidade(Gravidade g){
    switch (g){
        case VERMELHO: return "Vermelho";
        case LARANJA: return "Laranja";
        case AMARELO: return "Amarelo";
        case VERDE: return "Verde";
        case AZUL: return "Azul";
    }
    return "Desconhecido"; // so acontece se alguem passar um valor fora do enum
}
#endif