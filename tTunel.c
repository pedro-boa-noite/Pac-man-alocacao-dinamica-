#include "tTunel.h"


tTunel* CriaTunel(int linhaAcesso1, int colunaAcesso1, int linhaAcesso2, int colunaAcesso2){
    tTunel *tunel = (tTunel *) malloc (sizeof(tTunel));

    if(tunel == NULL){
        return NULL;
    } else{
        tunel->acesso1 = CriaPosicao(linhaAcesso1, colunaAcesso1);
        tunel->acesso2 = CriaPosicao(linhaAcesso2, colunaAcesso2);
        return tunel; 
    }
    
}

bool EntrouTunel(tTunel* tunel, tPosicao* posicao){
    if(SaoIguaisPosicao(tunel->acesso1, posicao) || SaoIguaisPosicao(tunel->acesso2, posicao)){
        return true;
    } else{
        return false;
    }
}

void LevaFinalTunel(tTunel* tunel, tPosicao* posicao){

    if(ObtemLinhaPosicao(posicao) == ObtemLinhaPosicao(tunel->acesso1) && ObtemColunaPosicao(posicao) == ObtemColunaPosicao(tunel->acesso1)){
        AtualizaPosicao(posicao, tunel->acesso2);
    } else{
        AtualizaPosicao(posicao, tunel->acesso1);
    }
}

void DesalocaTunel(tTunel* tunel){
    if(tunel != NULL){
        free(tunel->acesso1);
        free(tunel->acesso2); 
        free(tunel);
    }
}