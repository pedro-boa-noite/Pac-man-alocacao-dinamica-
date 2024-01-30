#include "tPosicao.h"

tPosicao* CriaPosicao(int linha, int coluna){
    tPosicao *posicao = (tPosicao *) malloc (sizeof(tPosicao));

    posicao->linha = linha;
    posicao->coluna = coluna;

    return posicao;
}

tPosicao* ClonaPosicao(tPosicao* posicao){
    
    tPosicao *clone = CriaPosicao(posicao->linha, posicao->coluna);

    return clone;
}

int ObtemLinhaPosicao(tPosicao* posicao){
    return posicao->linha;
}

int ObtemColunaPosicao(tPosicao* posicao){
    return posicao->coluna;
}

void AtualizaPosicao(tPosicao* posicaoAtual, tPosicao* posicaoNova){

    *posicaoAtual = *posicaoNova; 
}

bool SaoIguaisPosicao(tPosicao* posicao1, tPosicao* posicao2){
    if(posicao1->linha == posicao2->linha && posicao1->coluna == posicao2->coluna){
        return true;
    } else{
        return false;
    }
}

void DesalocaPosicao(tPosicao* posicao){
    free(posicao);
}