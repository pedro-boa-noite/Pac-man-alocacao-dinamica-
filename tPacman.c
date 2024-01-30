#include "tPacman.h"


tPacman *CriaPacman(tPosicao *posicao)
{
    tPacman *pac = (tPacman *)calloc(1, sizeof(tPacman));
    
    if (pac == NULL || posicao == NULL)  return NULL;

    pac->historicoDeMovimentosSignificativos = (tMovimento **) calloc (1, sizeof(tMovimento));
    pac->posicaoAtual = posicao;
    pac->estaVivo = 1;

    return pac;
}

tPacman *ClonaPacman(tPacman *pacman)
{
    tPosicao* posicao = ClonaPosicao(pacman->posicaoAtual);

    tPacman *clone = CriaPacman(posicao);

    return clone;
}

tMovimento **ClonaHistoricoDeMovimentosSignificativosPacman(tPacman *pacman) 
{ 

    tMovimento **clone = (tMovimento **)malloc(ObtemNumeroMovimentosSignificativosPacman(pacman)*sizeof(tMovimento*));
    for (int i = 0; i < ObtemNumeroMovimentosSignificativosPacman(pacman); i++)
    {
        int numeroDoMovimento = ObtemNumeroMovimento(pacman->historicoDeMovimentosSignificativos[i]);
        COMANDO comando = ObtemComandoMovimento(pacman->historicoDeMovimentosSignificativos[i]);
        char* acao = ObtemAcaoMovimento(pacman->historicoDeMovimentosSignificativos[i]);

        clone[i] = CriaMovimento(numeroDoMovimento, comando, acao);
    }

    return clone;
}

tPosicao *ObtemPosicaoPacman(tPacman *pacman){
    return pacman->posicaoAtual;
}

int EstaVivoPacman(tPacman *pacman){
    return pacman->estaVivo;
}

void MovePacman(tPacman *pacman, tMapa *mapa, COMANDO comando){
    int flag=0, flag_tunel=0, flag_tunel_imediato=0, linha, coluna;

    int linha_antiga = ObtemLinhaPosicao(pacman->posicaoAtual);
    int coluna_antiga = ObtemColunaPosicao(pacman->posicaoAtual);
    tPosicao* posicao_antiga_1 = CriaPosicao(linha_antiga, coluna_antiga);

    if(comando == 0){
        linha = ObtemLinhaPosicao(pacman->posicaoAtual);
        coluna = ObtemColunaPosicao(pacman->posicaoAtual)-1;
        (pacman->nMovimentosEsquerda)++;
        tPosicao* posicao = CriaPosicao(linha, coluna);
        if(EncontrouParedeMapa(mapa, posicao)){
            flag = 1;
            (pacman->nColisoesParedeEsquerda)++;
            InsereNovoMovimentoSignificativoPacman(pacman, comando, "colidiu com a parede");
            if(PossuiTunelMapa(mapa)){
                if(AcessouTunelMapa(mapa, posicao_antiga_1)){
                    flag_tunel_imediato++;
                }    
            }
        }
        if(EncontrouComidaMapa(mapa, posicao)){
            (pacman->nFrutasComidasEsquerda)++;
            InsereNovoMovimentoSignificativoPacman(pacman, comando, "pegou comida");
        }
        if(ObtemItemMapa(mapa, posicao) == '@'){
           flag_tunel = 1;
        }
        DesalocaPosicao(posicao);
    } else if(comando == 1){
        linha = ObtemLinhaPosicao(pacman->posicaoAtual)-1;
        coluna = ObtemColunaPosicao(pacman->posicaoAtual);
        (pacman->nMovimentosCima)++;
        tPosicao* posicao = CriaPosicao(linha, coluna);
        if(EncontrouParedeMapa(mapa, posicao)){
            flag = 1;
            (pacman->nColisoesParedeCima)++;
            InsereNovoMovimentoSignificativoPacman(pacman, comando, "colidiu com a parede");
            if(PossuiTunelMapa(mapa)){
                if(AcessouTunelMapa(mapa, posicao_antiga_1)){
                    flag_tunel_imediato++;
                }    
            }
        }
        if(EncontrouComidaMapa(mapa, posicao)){
            (pacman->nFrutasComidasCima)++;
            InsereNovoMovimentoSignificativoPacman(pacman, comando, "pegou comida");
        }
        if(ObtemItemMapa(mapa, posicao) == '@'){
           flag_tunel = 1;
        }
        DesalocaPosicao(posicao);
    } else if(comando == 2){
        linha = ObtemLinhaPosicao(pacman->posicaoAtual)+1;
        coluna = ObtemColunaPosicao(pacman->posicaoAtual);
        (pacman->nMovimentosBaixo)++;
        tPosicao* posicao = CriaPosicao(linha, coluna);
        if(EncontrouParedeMapa(mapa, posicao)){
            flag = 1;
            (pacman->nColisoesParedeBaixo)++;
            InsereNovoMovimentoSignificativoPacman(pacman, comando, "colidiu com a parede");
            if(PossuiTunelMapa(mapa)){
                if(AcessouTunelMapa(mapa, posicao_antiga_1)){
                    flag_tunel_imediato++;
                }    
            }
        }
        if(EncontrouComidaMapa(mapa, posicao)){
            (pacman->nFrutasComidasBaixo)++;
            InsereNovoMovimentoSignificativoPacman(pacman, comando, "pegou comida");
        }
        if(ObtemItemMapa(mapa, posicao) == '@'){
           flag_tunel = 1;
        }
        DesalocaPosicao(posicao);
    } else if(comando == 3){
        linha = ObtemLinhaPosicao(pacman->posicaoAtual);
        coluna = ObtemColunaPosicao(pacman->posicaoAtual)+1;
        (pacman->nMovimentosDireita)++;
        tPosicao* posicao = CriaPosicao(linha, coluna);
        if(EncontrouParedeMapa(mapa, posicao)){
            flag = 1;
            (pacman->nColisoesParedeDireita)++;
            InsereNovoMovimentoSignificativoPacman(pacman, comando, "colidiu com a parede");
            if(PossuiTunelMapa(mapa)){
                if(AcessouTunelMapa(mapa, posicao_antiga_1)){
                    flag_tunel_imediato++;
                }    
            }
        }
        if(EncontrouComidaMapa(mapa, posicao)){
            (pacman->nFrutasComidasDireita)++;
            InsereNovoMovimentoSignificativoPacman(pacman, comando, "pegou comida");
        }
        if(ObtemItemMapa(mapa, posicao) == '@'){
           flag_tunel = 1;
        }
        DesalocaPosicao(posicao);
    }

    tPosicao *posicao = CriaPosicao(linha, coluna);
    tPosicao *posicao_antiga_2 = posicao;

    if(flag == 0){
            if(flag_tunel == 1){
                AtualizaPosicao(pacman->posicaoAtual, posicao);
                AtualizaTrilhaPacman(pacman);
                EntraTunelMapa(mapa, posicao);
                AtualizaPosicao(pacman->posicaoAtual, posicao);
                AtualizaTrilhaPacman(pacman);
            } else{
                AtualizaPosicao(pacman->posicaoAtual, posicao);
            }
    } 

    AtualizaTrilhaPacman(pacman);
    DesalocaPosicao(posicao);

    if(PossuiTunelMapa(mapa) && EntrouTunel(ObtemTunelMapa(mapa), posicao_antiga_1)){
        if(flag_tunel_imediato == 0){
            AtualizaItemMapa(mapa, pacman->posicaoAtual, '@');    
        }
    } else if(flag == 0){
        AtualizaItemMapa(mapa, posicao_antiga_1, ' ');    
    }
    DesalocaPosicao(posicao_antiga_1);

}

void CriaTrilhaPacman(tPacman *pacman, int nLinhas, int nColunas){
    pacman->nLinhasTrilha = nLinhas;
    pacman->nColunasTrilha = nColunas;

    pacman->trilha = (int **) calloc (1, nLinhas * sizeof(int*));
    for(int i = 0; i < nLinhas; i++){
        pacman->trilha[i] = (int *) calloc (1, nColunas * sizeof(int));
        for(int j = 0; j < nColunas; j++){
            pacman->trilha[i][j] = -1;
        }
    }
}

void AtualizaTrilhaPacman(tPacman *pacman){

    pacman->trilha[ObtemLinhaPosicao(pacman->posicaoAtual)][ObtemColunaPosicao(pacman->posicaoAtual)] = ObtemNumeroAtualMovimentosPacman(pacman);

}

void SalvaTrilhaPacman(tPacman *pacman){
    FILE *arquivo = fopen("trilha.txt", "w");

    for(int i=0; i < pacman->nLinhasTrilha; i++){
        for(int j=0; j < pacman->nColunasTrilha; j++){
            if(pacman->trilha[i][j] == -1){
                fprintf(arquivo, "#");
            } else{
                fprintf(arquivo, "%d", pacman->trilha[i][j]);    
            }
            if(j != (pacman->nColunasTrilha - 1)){
                fprintf(arquivo, " ");
            }
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
}

void InsereNovoMovimentoSignificativoPacman(tPacman *pacman, COMANDO comando, const char *acao){

    (pacman->nMovimentosSignificativos)++;

    pacman->historicoDeMovimentosSignificativos = (tMovimento **) realloc (pacman->historicoDeMovimentosSignificativos, pacman->nMovimentosSignificativos*sizeof(tMovimento*));
    pacman->historicoDeMovimentosSignificativos[pacman->nMovimentosSignificativos-1] = CriaMovimento(ObtemNumeroAtualMovimentosPacman(pacman), comando, acao);
    
}

void MataPacman(tPacman *pacman){
    pacman->estaVivo = 0;
}

void DesalocaPacman(tPacman *pacman){
    if(pacman != NULL){
        
        for (int i = 0; i < pacman->nMovimentosSignificativos; i++)
        {
        DesalocaMovimento(pacman->historicoDeMovimentosSignificativos[i]);
        }
        free(pacman->historicoDeMovimentosSignificativos);

        for (int i = 0; i < pacman->nLinhasTrilha; i++)
        {
            free(pacman->trilha[i]);
        }
        free(pacman->trilha);  

        DesalocaPosicao(pacman->posicaoAtual);
        free(pacman);
    }
}

int ObtemNumeroAtualMovimentosPacman(tPacman *pacman){
    return (ObtemNumeroMovimentosBaixoPacman(pacman) + ObtemNumeroMovimentosCimaPacman(pacman) + ObtemNumeroMovimentosDireitaPacman(pacman) + ObtemNumeroMovimentosEsquerdaPacman(pacman));
}

int ObtemNumeroMovimentosSemPontuarPacman(tPacman *pacman){
    return (ObtemNumeroAtualMovimentosPacman(pacman) - ObtemPontuacaoAtualPacman(pacman));
}

int ObtemNumeroColisoesParedePacman(tPacman *pacman){
    return (ObtemNumeroColisoesParedeBaixoPacman(pacman) + ObtemNumeroColisoesParedeCimaPacman(pacman) + ObtemNumeroColisoesParedeDireitaPacman(pacman) + ObtemNumeroColisoesParedeEsquerdaPacman(pacman));
}

int ObtemNumeroMovimentosBaixoPacman(tPacman *pacman){
    return pacman->nMovimentosBaixo;
}

int ObtemNumeroFrutasComidasBaixoPacman(tPacman *pacman){
    return pacman->nFrutasComidasBaixo;
}

int ObtemNumeroColisoesParedeBaixoPacman(tPacman *pacman){
    return pacman->nColisoesParedeBaixo;
}

int ObtemNumeroMovimentosCimaPacman(tPacman *pacman){
    return pacman->nMovimentosCima;
}

int ObtemNumeroFrutasComidasCimaPacman(tPacman *pacman){
    return pacman->nFrutasComidasCima;
}

int ObtemNumeroColisoesParedeCimaPacman(tPacman *pacman){
    return pacman->nColisoesParedeCima;
}

int ObtemNumeroMovimentosEsquerdaPacman(tPacman *pacman){
    return pacman->nMovimentosEsquerda;
}

int ObtemNumeroFrutasComidasEsquerdaPacman(tPacman *pacman){
    return pacman->nFrutasComidasEsquerda;
}

int ObtemNumeroColisoesParedeEsquerdaPacman(tPacman *pacman){
    return pacman->nColisoesParedeEsquerda;
}

int ObtemNumeroMovimentosDireitaPacman(tPacman *pacman){
    return pacman->nMovimentosDireita;
}

int ObtemNumeroFrutasComidasDireitaPacman(tPacman *pacman){
    return pacman->nFrutasComidasDireita;
}

int ObtemNumeroColisoesParedeDireitaPacman(tPacman *pacman){
    return pacman->nColisoesParedeDireita;
}

int ObtemNumeroMovimentosSignificativosPacman(tPacman *pacman){
    return pacman->nMovimentosSignificativos;
}

int ObtemPontuacaoAtualPacman(tPacman *pacman){
    return (pacman->nFrutasComidasBaixo + pacman->nFrutasComidasCima + pacman->nFrutasComidasDireita + pacman->nFrutasComidasEsquerda);
}