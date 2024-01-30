#include <stdio.h>
#include <stdlib.h>
#include "tMapa.h"
#include "tMovimento.h"
#include "tPacman.h"
#include "tPosicao.h"
#include "tTunel.h"

#define MAX_FANTASMAS 4
#define TOTAL_MOV 4

typedef struct tJogo {
    /* direcao indicada */
    char direcao;
    char movimentos[TOTAL_MOV];
} tJogo;

typedef struct tFantasma {
    /* Posição atual do fantasma (linha,coluna) */
    tPosicao *posicaoAtual;
    int flag_comida;
    int flag_parede;
    char tipo;
} tFantasma;

void PerdeJogo(tJogo* jogo, tPacman* pacman, tMapa* mapa, FILE *resumo, FILE *ranking, FILE *estatisticas);
void AtualizaFlag(tFantasma *fantasma);
tFantasma *CriaFantasma(tPosicao *posicao, char tipo);
tPosicao *ObtemPosicaoFantasma(tFantasma *fantasma);
void IncrementaComida(tPacman *pacman, tJogo *jogo);
void MoveFantasma(tJogo *jogo, tFantasma *fantasma, tPacman *pacman, tMapa *mapa, int linha_old_pac, int coluna_old_pac, tPosicao* posicao_nova_pacman);
void DesalocaFantasma(tFantasma *fantasma);

void DesalocaJogo(tJogo* jogo){
    free(jogo);
}

//CONFERE SE EXISTE CADA FANTASMA
int Existe(tMapa* mapa, char tipo){
    for(int i = 0; i < ObtemNumeroLinhasMapa(mapa); i++){
        for(int j = 0; j < ObtemNumeroColunasMapa(mapa); j++){
            tPosicao* posicao = CriaPosicao(i, j);
            char c = ObtemItemMapa(mapa, posicao);
            DesalocaPosicao(posicao);
            if(c == tipo){
                return 1;
            }
        }
    }
    return 0;
}

void PrintaMapa(tMapa* mapa){

    for(int i = 0; i < ObtemNumeroLinhasMapa(mapa); i++){
        for(int j = 0; j < ObtemNumeroColunasMapa(mapa); j++){
            tPosicao* posicao = CriaPosicao(i, j);
            char c = ObtemItemMapa(mapa, posicao);
            printf("%c", c);
            DesalocaPosicao(posicao);
        }
        printf("\n");
    }
}

void PrintaPosJogada(tPacman* pacman, tJogo* jogo, tMapa* mapa)
{
    printf("Estado do jogo apos o movimento '%c':\n", jogo->direcao);

    PrintaMapa(mapa);
 
    printf("Pontuacao: %d\n\n", ObtemPontuacaoAtualPacman(pacman));
}

void PrintaVitoria(tPacman* pacman)
{
    printf("Voce venceu!\nPontuacao final: %d", ObtemPontuacaoAtualPacman(pacman));
}

void PrintaDerrota(tPacman* pacman)
{
    printf("Game over!\nPontuacao final: %d", ObtemPontuacaoAtualPacman(pacman));
}

void ReordenaRanking(tJogo* jogo, int i, int j)
{
    // COLOCA AS DIRECOES 'W', 'S', 'D' E 'A' NA ORDEM REQUERIDA
    char savevalue;

    savevalue = jogo->movimentos[i];
    jogo->movimentos[i] = jogo->movimentos[j];
    jogo->movimentos[j] = savevalue;
}

int ObtemNumeroFrutasComidas(char letra, tPacman* pacman){
    if(letra == 'a'){
        return ObtemNumeroFrutasComidasEsquerdaPacman(pacman);
    } else if(letra == 'd'){
        return ObtemNumeroFrutasComidasDireitaPacman(pacman);
    } else if(letra == 'w'){
        return ObtemNumeroFrutasComidasCimaPacman(pacman);
    } else if(letra == 's'){
        return ObtemNumeroFrutasComidasBaixoPacman(pacman);
    }
}

int ObtemNumeroColisoesParede(char letra, tPacman* pacman){
    if(letra == 'a'){
        return ObtemNumeroColisoesParedeEsquerdaPacman(pacman);
    } else if(letra == 'd'){
        return ObtemNumeroColisoesParedeDireitaPacman(pacman);
    } else if(letra == 'w'){
        return ObtemNumeroColisoesParedeCimaPacman(pacman);
    } else if(letra == 's'){
        return ObtemNumeroColisoesParedeBaixoPacman(pacman);
    }
}

int ObtemNumeroMovimentos(char letra, tPacman* pacman){
    if(letra == 'a'){
        return ObtemNumeroMovimentosEsquerdaPacman(pacman);
    } else if(letra == 'd'){
        return ObtemNumeroMovimentosDireitaPacman(pacman);
    } else if(letra == 'w'){
        return ObtemNumeroMovimentosCimaPacman(pacman);
    } else if(letra == 's'){
        return ObtemNumeroMovimentosBaixoPacman(pacman);
    }
}

void ImprimeRanking(tPacman* pacman, tJogo* jogo, FILE *ranking)
{
    // CONTROLA OS PARAMETROS DE PRIORIDADE NA REORDENACAO DO RANKING E O IMPRIME
    for (int i = 0; i < 3; i++)
    {
        for (int j = i + 1; j < 4; j++)
        {
            if (ObtemNumeroFrutasComidas(jogo->movimentos[i], pacman) < ObtemNumeroFrutasComidas(jogo->movimentos[j], pacman))
            {
                ReordenaRanking(jogo, i, j);
            }
            else if (ObtemNumeroFrutasComidas(jogo->movimentos[i], pacman) == ObtemNumeroFrutasComidas(jogo->movimentos[j], pacman))
            {
                if (ObtemNumeroColisoesParede(jogo->movimentos[i], pacman) > ObtemNumeroColisoesParede(jogo->movimentos[j], pacman))
                {
                    ReordenaRanking(jogo, i, j);
                }
                else if (ObtemNumeroColisoesParede(jogo->movimentos[i], pacman) == ObtemNumeroColisoesParede(jogo->movimentos[j], pacman))
                {
                    if (ObtemNumeroMovimentos(jogo->movimentos[i], pacman) < ObtemNumeroMovimentos(jogo->movimentos[j], pacman))
                    {
                        ReordenaRanking(jogo, i, j);
                    }
                    else if (ObtemNumeroMovimentos(jogo->movimentos[i], pacman) == ObtemNumeroMovimentos(jogo->movimentos[j], pacman))
                    {
                        if (jogo->movimentos[i] > jogo->movimentos[j])
                        {
                            ReordenaRanking(jogo, i, j);
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        fprintf(ranking, "%c,%d,%d,%d\n", jogo->movimentos[i], ObtemNumeroFrutasComidas(jogo->movimentos[i], pacman), ObtemNumeroColisoesParede(jogo->movimentos[i], pacman), ObtemNumeroMovimentos(jogo->movimentos[i], pacman));
    }
}

void ImprimeEstatisticas(tPacman *pacman, FILE *estatisticas)
{

    fprintf(estatisticas, "Numero de movimentos: %d\nNumero de movimentos sem pontuar: %d\nNumero de colisoes com parede: %d\nNumero de movimentos para baixo: %d\nNumero de movimentos para cima: %d\nNumero de movimentos para esquerda: %d\nNumero de movimentos para direita: %d\n", ObtemNumeroAtualMovimentosPacman(pacman), ObtemNumeroMovimentosSemPontuarPacman(pacman), ObtemNumeroColisoesParedePacman(pacman), ObtemNumeroMovimentosBaixoPacman(pacman), ObtemNumeroMovimentosCimaPacman(pacman), ObtemNumeroMovimentosEsquerdaPacman(pacman), ObtemNumeroMovimentosDireitaPacman(pacman));
}

char ObtemLetraDirecao(COMANDO comando){
    if(comando == MOV_ESQUERDA){
        return 'a';
    } else if(comando == MOV_CIMA){
        return 'w';
    } else if(comando == MOV_BAIXO){
        return 's';
    } else if(comando == MOV_DIREITA){
        return 'd';
    }
}

void PerdeJogo(tJogo* jogo, tPacman* pacman, tMapa* mapa, FILE *resumo, FILE *ranking, FILE *estatisticas)
{
    //IMPRIME RESUMO A PARTIR DO HISTORICO DE MOVIMENTOS SIGNIFICATIVOS
    tMovimento** historico = ClonaHistoricoDeMovimentosSignificativosPacman(pacman);
    for(int i=0; i<ObtemNumeroMovimentosSignificativosPacman(pacman); i++){
        char string[50];
        strcpy(string, ObtemAcaoMovimento(historico[i]));
        char letra = ObtemLetraDirecao(ObtemComandoMovimento(historico[i]));
        fprintf(resumo, "Movimento %d (%c) ", ObtemNumeroMovimento(historico[i]), letra);
        fprintf(resumo, "%s", string);
        fprintf(resumo, "\n");
        DesalocaMovimento(historico[i]);
    }
    free(historico);

    if(!EstaVivoPacman(pacman)){
        fprintf(resumo, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", (ObtemNumeroAtualMovimentosPacman(pacman)), jogo->direcao);
    }

    ImprimeRanking(pacman, jogo, ranking);
    ImprimeEstatisticas(pacman, estatisticas);
    // FECHA OS ARQUIVOS
    fclose(resumo);
    fclose(ranking);
    fclose(estatisticas);

    SalvaTrilhaPacman(pacman);
    PrintaDerrota(pacman);

    DesalocaPacman(pacman);
    DesalocaJogo(jogo);
    DesalocaMapa(mapa);
    exit(0); // TERMINA O PROGRAMA
}

void VenceJogo(tJogo* jogo, tPacman* pacman, tMapa* mapa, FILE *resumo, FILE *ranking, FILE *estatisticas){

    //IMPRIME RESUMO A PARTIR DO HISTORICO DE MOVIMENTOS SIGNIFICATIVOS
    tMovimento** historico = ClonaHistoricoDeMovimentosSignificativosPacman(pacman);
    for(int i=0; i<ObtemNumeroMovimentosSignificativosPacman(pacman); i++){
        char string[50];
        strcpy(string, ObtemAcaoMovimento(historico[i]));
        char letra = ObtemLetraDirecao(ObtemComandoMovimento(historico[i]));
        fprintf(resumo, "Movimento %d (%c) ", ObtemNumeroMovimento(historico[i]), letra);
        fprintf(resumo, "%s", string);
        fprintf(resumo, "\n");
        DesalocaMovimento(historico[i]);
    }
    free(historico);

    ImprimeRanking(pacman, jogo, ranking);
    ImprimeEstatisticas(pacman, estatisticas);    
    PrintaPosJogada(pacman, jogo, mapa);
    // FECHA OS ARQUIVOS
    fclose(resumo);
    fclose(ranking);
    fclose(estatisticas);

    SalvaTrilhaPacman(pacman);
    PrintaVitoria(pacman); // TERMINA O JOGO CASO O PACMAN COMA TODAS AS COMIDAS    

    DesalocaPacman(pacman);
    DesalocaJogo(jogo);
    DesalocaMapa(mapa);
    exit(0); // TERMINA O PROGRAMA
}

COMANDO ObtemComandoDirecao(char direcao){
    if(direcao == 'a'){
        return MOV_ESQUERDA;
    } else if(direcao == 'w'){
        return MOV_CIMA;
    } else if(direcao == 's'){
        return MOV_BAIXO;
    } else if(direcao == 'd'){
        return MOV_DIREITA;
    }
}

tJogo* CriaJogo(char const* caminhoConfig){
    tJogo* jogo = (tJogo*) calloc (1, sizeof(tJogo));

    if(jogo != NULL){
        jogo->movimentos[0] = 'a';
        jogo->movimentos[1] = 'd';
        jogo->movimentos[2] = 'w';
        jogo->movimentos[3] = 's';
    }

    return jogo;
}

void InicializaMapa(tMapa* mapa, char const* caminhoConfig){

    FILE *file = fopen("./inicializacao.txt", "w");
    int linha, coluna;

    for(int i = 0; i < ObtemNumeroLinhasMapa(mapa); i++){
        for(int j = 0; j < ObtemNumeroColunasMapa(mapa); j++){
            tPosicao* posicao = CriaPosicao(i, j);
            char c = ObtemItemMapa(mapa, posicao);
            DesalocaPosicao(posicao);
            fprintf(file, "%c", c);
            if(c == '>'){
                linha = i;
                coluna = j;
            }
        }
        fprintf(file, "\n");
    }

    fprintf(file, "Pac-Man comecara o jogo na linha %d e coluna %d\n", linha + 1, coluna + 1);
    fclose(file);
}

void IniciaJogo(tJogo* jogo, tMapa* mapa, char const* caminhoConfig){

    FILE *ranking = fopen("./ranking.txt", "w");
    FILE *resumo = fopen("./resumo.txt", "a");
    FILE *estatisticas = fopen("./estatisticas.txt", "w");

    tPosicao* posicao_pacman = ObtemPosicaoItemMapa(mapa, '>');
    tPacman* pacman = CriaPacman(posicao_pacman);
    
    char tipos[MAX_FANTASMAS];
    int num_fantasmas=0;

    //INCREMENTA O NUMERO DE FANTASMAS QUE EXISTEM NO MAPA
    if(Existe(mapa, 'B')){
        tipos[num_fantasmas] = 'B';
        num_fantasmas++;
    } 
    if(Existe(mapa, 'P')){
        tipos[num_fantasmas] = 'P';
        num_fantasmas++;
    }
    if(Existe(mapa, 'I')){
        tipos[num_fantasmas] = 'I';
        num_fantasmas++;
    }
    if(Existe(mapa, 'C')){
        tipos[num_fantasmas] = 'C';
        num_fantasmas++;
    }

    tFantasma* fantasmas[num_fantasmas];
    tPosicao* posicao_fantasma[num_fantasmas];

    //CRIA CADA FANTASMA
    for(int i=0; i<num_fantasmas; i++){
        posicao_fantasma[i] = ObtemPosicaoItemMapa(mapa, tipos[i]);
        fantasmas[i] = CriaFantasma(posicao_fantasma[i], tipos[i]);
    }

    CriaTrilhaPacman(pacman, ObtemNumeroLinhasMapa(mapa), ObtemNumeroColunasMapa(mapa)); // GERA MAPA DA TRILHA

    while (ObtemNumeroAtualMovimentosPacman(pacman) < ObtemNumeroMaximoMovimentosMapa(mapa)){

        scanf("%c", &jogo->direcao);
        scanf("%*c");
    
        // GERA A MOVIMENTACAO NO MAPA
        int flag_fantasma=0, flag_cruzou=0;
        tPosicao *posicao_antiga_pacman = ObtemPosicaoItemMapa(mapa, '>');
        int linha_old_pac = ObtemLinhaPosicao(posicao_antiga_pacman);
        int coluna_old_pac = ObtemColunaPosicao(posicao_antiga_pacman);
        AtualizaTrilhaPacman(pacman);
        int num_atual_mov_sig = ObtemNumeroMovimentosSignificativosPacman(pacman);
        MovePacman(pacman, mapa, ObtemComandoDirecao(jogo->direcao));
        tPosicao *posicao_nova_pacman = ObtemPosicaoPacman(pacman);

        tPosicao* posicao_antiga_fantasma[num_fantasmas];
        tPosicao* posicao_nova_fantasma[num_fantasmas];
        for(int i=0; i<num_fantasmas; i++){
            posicao_antiga_fantasma[i] = ObtemPosicaoItemMapa(mapa, fantasmas[i]->tipo);
            MoveFantasma(jogo, fantasmas[i], pacman, mapa, linha_old_pac, coluna_old_pac, posicao_nova_pacman);
            posicao_nova_fantasma[i] = ObtemPosicaoFantasma(fantasmas[i]);
        }

        tPosicao* posicao_pac = ObtemPosicaoPacman(pacman);

        for(int i=0; i<num_fantasmas; i++){
            if(SaoIguaisPosicao(posicao_pac, posicao_fantasma[i])){
                flag_fantasma++;
            }
            if((SaoIguaisPosicao(posicao_antiga_fantasma[i], posicao_nova_pacman)) && (ObtemLinhaPosicao(posicao_nova_fantasma[i]) == linha_old_pac) && (ObtemColunaPosicao(posicao_nova_fantasma[i]) == coluna_old_pac)){
                flag_cruzou++;
            }
            DesalocaPosicao(posicao_antiga_fantasma[i]);
        }

        if((flag_fantasma == 0) && (flag_cruzou == 0)){
            AtualizaItemMapa(mapa, posicao_pac, '>');  
        } else if((flag_fantasma == 0) && (flag_cruzou != 0)){
            AtualizaItemMapa(mapa, posicao_pac, ' ');  
        }

        flag_fantasma = 0;
        if(PossuiTunelMapa(mapa) && EntrouTunel(ObtemTunelMapa(mapa), posicao_antiga_pacman)){
            if((SaoIguaisPosicao(posicao_pac, posicao_antiga_pacman)) && !(AcessouTunelMapa(mapa, posicao_antiga_pacman))){
                AtualizaItemMapa(mapa, posicao_antiga_pacman, '@');    
            }
            if(!(SaoIguaisPosicao(posicao_pac, posicao_antiga_pacman)) && AcessouTunelMapa(mapa, posicao_antiga_pacman)){
                AtualizaItemMapa(mapa, posicao_antiga_pacman, '@');
            }
        } else{
            for(int i=0; i<num_fantasmas; i++){
                if(SaoIguaisPosicao(posicao_antiga_pacman, posicao_fantasma[i])){
                    flag_fantasma++;
                }
            }
            if((flag_fantasma == 0) && !(SaoIguaisPosicao(posicao_antiga_pacman, posicao_pac))){
                AtualizaItemMapa(mapa, posicao_antiga_pacman, ' ');    
            }  
        }
        DesalocaPosicao(posicao_antiga_pacman);

        if (ObtemQuantidadeFrutasIniciaisMapa(mapa) == ObtemPontuacaoAtualPacman(pacman)){
            { // SE ELE COMER TODAS AS COMIDAS, O JOGO ACABA
                for(int i=0; i<num_fantasmas; i++){
                    DesalocaFantasma(fantasmas[i]);
                }
                VenceJogo(jogo, pacman, mapa, resumo, ranking, estatisticas);
            }    
        }

        printf("Estado do jogo apos o movimento '%c':\n", jogo->direcao);

        PrintaMapa(mapa); // PRINTA O MAPA COM AS MODIFICACOES DE CADA JOGADA

        printf("Pontuacao: %d\n\n", ObtemPontuacaoAtualPacman(pacman));

        if(!(EstaVivoPacman(pacman))){
            for(int i=0; i<num_fantasmas; i++){
                DesalocaFantasma(fantasmas[i]);
            }
            PerdeJogo(jogo, pacman, mapa, resumo, ranking, estatisticas);  
        }
    }

    for(int i=0; i<num_fantasmas; i++){
        DesalocaFantasma(fantasmas[i]);
    }
    PerdeJogo(jogo, pacman, mapa, resumo, ranking, estatisticas);
}


//////////////////////////////// AQUI COMECA tFantasma //////////////////////////////////////////////////////////////

/**
 * Atualiza flag relacionada a parede, verificando se o fantasma tocou nela,
 * caso afirmativo, o sentido da direcao eh invertido.
 * \param posicao Ponteiro para tFantasma
 */
void AtualizaFlag(tFantasma* fantasma){
    if(fantasma->flag_parede == 0){
        fantasma->flag_parede = 1;    
    } else{
        fantasma->flag_parede = 0;
    }
}

/**
 * Cria o fantasma dinamicamente. Caso dê erro na alocação da estrutura tFantasma, 
 * retorna NULL. 
 * Caso a posição passada como parâmetro seja NULL, retorna NULL.
 * Caso não dê erros, retorna o ponteiro para o tFantasma alocado.
 * \param posicao Ponteiro para tPosicao
 */
tFantasma* CriaFantasma(tPosicao* posicao, char tipo){
    tFantasma *fantasma = (tFantasma *)calloc(1, sizeof(tFantasma));
    
    if (fantasma == NULL || posicao == NULL)  return NULL;

    fantasma->posicaoAtual = posicao;
    fantasma->tipo = tipo;

    return fantasma;
}

/**
 * Retorna a posição do fantasma.
 * 
 * \param fantasma fantasma
 */ 
tPosicao* ObtemPosicaoFantasma(tFantasma* fantasma){
    return fantasma->posicaoAtual;
}

void IncrementaComida(tPacman *pacman, tJogo* jogo){
    if(ObtemComandoDirecao(jogo->direcao) == MOV_BAIXO){
        (pacman->nFrutasComidasBaixo)++;
    } else if(ObtemComandoDirecao(jogo->direcao) == MOV_CIMA){
        (pacman->nFrutasComidasCima)++;
    } else if(ObtemComandoDirecao(jogo->direcao) == MOV_DIREITA){
        (pacman->nFrutasComidasDireita)++;
    } else if(ObtemComandoDirecao(jogo->direcao) == MOV_ESQUERDA){
        (pacman->nFrutasComidasEsquerda)++;
    }
    InsereNovoMovimentoSignificativoPacman(pacman, ObtemComandoDirecao(jogo->direcao), "pegou comida");
}

/**
 * Função que irá mover o fantasma no mapa, atualizando sua posição.
 * Dado o fantasma, o mapa, e o tipo de fantasma, a posição do fantasma
 * é atualizada. Consultas ao mapa deverão ser feitas para ver se
 * a posição pode ser realmente atualizada ou não, como por exemplo,
 * se o fantasma encontrou uma parede ele inverte o sentido do seu movimento.
 * 
 * \param fantasma fantasma
 * \param mapa o mapa que contem o fantasma
 * \param tipo o tipo de fantasma
 */
void MoveFantasma(tJogo* jogo, tFantasma* fantasma, tPacman* pacman, tMapa* mapa, int linha_old_pac, int coluna_old_pac, tPosicao* posicao_nova_pacman){

    int linha_1, coluna_1, linha_2, coluna_2, flag_jogada_atual=0, segunda_comida=0;
    int linha_antiga = ObtemLinhaPosicao(fantasma->posicaoAtual);
    int coluna_antiga = ObtemColunaPosicao(fantasma->posicaoAtual);
    tPosicao* posicao_antiga = CriaPosicao(linha_antiga, coluna_antiga);
    tPosicao* posicao_pac = ObtemPosicaoPacman(pacman);

    if(fantasma->tipo == 'B'){
        linha_1 = ObtemLinhaPosicao(fantasma->posicaoAtual);
        coluna_1 = ObtemColunaPosicao(fantasma->posicaoAtual)-1;
        linha_2 = ObtemLinhaPosicao(fantasma->posicaoAtual);
        coluna_2 = ObtemColunaPosicao(fantasma->posicaoAtual)+1;

        if(fantasma->flag_parede == 0){
            tPosicao* proxima_posicao = CriaPosicao(linha_1, coluna_1);
            if(EncontrouParedeMapa(mapa, proxima_posicao)){  
                AtualizaFlag(fantasma);
            }
            DesalocaPosicao(proxima_posicao); 
        } else{            
            tPosicao* proxima_posicao = CriaPosicao(linha_2, coluna_2);
            if(EncontrouParedeMapa(mapa, proxima_posicao)){  
                AtualizaFlag(fantasma);
            }
            DesalocaPosicao(proxima_posicao);     
        }
        tPosicao* posicao;
        if(fantasma->flag_parede == 1){
            posicao = CriaPosicao(linha_2, coluna_2);    
        } else{
            posicao = CriaPosicao(linha_1, coluna_1);  
        }  
        if(EncontrouComidaMapa(mapa, posicao)){
            if(fantasma->flag_comida == 0){
                flag_jogada_atual = 1;
            } else if(fantasma->flag_comida == 1){
                segunda_comida = 1;
            }
            fantasma->flag_comida = 1;
        }
        if(SaoIguaisPosicao(posicao, posicao_pac) || (SaoIguaisPosicao(posicao_antiga, posicao_nova_pacman) && (ObtemLinhaPosicao(posicao) == linha_old_pac) && (ObtemColunaPosicao(posicao) == coluna_old_pac))){
            MataPacman(pacman);
        }
        if(AtualizaItemMapa(mapa, posicao, 'B')){
            AtualizaPosicao(fantasma->posicaoAtual, posicao);
        }
        DesalocaPosicao(posicao);
    } else if(fantasma->tipo == 'P'){
        linha_1 = ObtemLinhaPosicao(fantasma->posicaoAtual)-1;
        coluna_1 = ObtemColunaPosicao(fantasma->posicaoAtual);
        linha_2 = ObtemLinhaPosicao(fantasma->posicaoAtual)+1;
        coluna_2 = ObtemColunaPosicao(fantasma->posicaoAtual);

        if(fantasma->flag_parede == 0){
            tPosicao* proxima_posicao = CriaPosicao(linha_1, coluna_1);
            if(EncontrouParedeMapa(mapa, proxima_posicao)){  
                AtualizaFlag(fantasma);
            }
            DesalocaPosicao(proxima_posicao);
        } else{
            tPosicao* proxima_posicao = CriaPosicao(linha_2, coluna_2);
            if(EncontrouParedeMapa(mapa, proxima_posicao)){  
                AtualizaFlag(fantasma);
            }
            DesalocaPosicao(proxima_posicao);  
        }
        tPosicao* posicao;
        if(fantasma->flag_parede == 1){
            posicao = CriaPosicao(linha_2, coluna_2);    
        } else{
            posicao = CriaPosicao(linha_1, coluna_1);  
        }  
        if(EncontrouComidaMapa(mapa, posicao)){
            if(fantasma->flag_comida == 0){
                flag_jogada_atual = 1;
            } else if(fantasma->flag_comida == 1){
                segunda_comida = 1;
            }
            fantasma->flag_comida = 1;
        }
        if((SaoIguaisPosicao(posicao, posicao_pac)) || (SaoIguaisPosicao(posicao_antiga, posicao_nova_pacman) && (ObtemLinhaPosicao(posicao) == linha_old_pac) && (ObtemColunaPosicao(posicao) == coluna_old_pac))){
            MataPacman(pacman);
        }
        if(AtualizaItemMapa(mapa, posicao, 'P')){
            AtualizaPosicao(fantasma->posicaoAtual, posicao);
        }
        DesalocaPosicao(posicao);
    } else if(fantasma->tipo == 'I'){
        linha_1 = ObtemLinhaPosicao(fantasma->posicaoAtual)+1;
        coluna_1 = ObtemColunaPosicao(fantasma->posicaoAtual);
        linha_2 = ObtemLinhaPosicao(fantasma->posicaoAtual)-1;
        coluna_2 = ObtemColunaPosicao(fantasma->posicaoAtual);
        if(fantasma->flag_parede == 0){
            tPosicao* proxima_posicao = CriaPosicao(linha_1, coluna_1);
            if(EncontrouParedeMapa(mapa, proxima_posicao)){  
                AtualizaFlag(fantasma);
            }
            DesalocaPosicao(proxima_posicao);
        } else{
            tPosicao* proxima_posicao = CriaPosicao(linha_2, coluna_2);
            if(EncontrouParedeMapa(mapa, proxima_posicao)){  
                AtualizaFlag(fantasma);
            }
            DesalocaPosicao(proxima_posicao);     
        }
        tPosicao* posicao;
        if(fantasma->flag_parede == 1){
            posicao = CriaPosicao(linha_2, coluna_2);    
        } else{
            posicao = CriaPosicao(linha_1, coluna_1);  
        }   
        if(EncontrouComidaMapa(mapa, posicao)){
            if(fantasma->flag_comida == 0){
                flag_jogada_atual = 1;
            } else if(fantasma->flag_comida == 1){
                segunda_comida = 1;
            }
            fantasma->flag_comida = 1;
        }
        if(SaoIguaisPosicao(posicao, posicao_pac) || (SaoIguaisPosicao(posicao_antiga, posicao_nova_pacman) && (ObtemLinhaPosicao(posicao) == linha_old_pac) && (ObtemColunaPosicao(posicao) == coluna_old_pac))){
            MataPacman(pacman);
        }
        if(AtualizaItemMapa(mapa, posicao, 'I')){
            AtualizaPosicao(fantasma->posicaoAtual, posicao);
        }
        DesalocaPosicao(posicao);
    } else if(fantasma->tipo == 'C'){
        linha_1 = ObtemLinhaPosicao(fantasma->posicaoAtual);
        coluna_1 = ObtemColunaPosicao(fantasma->posicaoAtual)+1; 
        linha_2 = ObtemLinhaPosicao(fantasma->posicaoAtual);
        coluna_2 = ObtemColunaPosicao(fantasma->posicaoAtual)-1; 

        if(fantasma->flag_parede == 0){
            tPosicao* proxima_posicao = CriaPosicao(linha_1, coluna_1);
            if(EncontrouParedeMapa(mapa, proxima_posicao)){  
                AtualizaFlag(fantasma);
            }
            DesalocaPosicao(proxima_posicao);
        } else{
            tPosicao* proxima_posicao = CriaPosicao(linha_2, coluna_2);
            if(EncontrouParedeMapa(mapa, proxima_posicao)){  
                AtualizaFlag(fantasma);
            }
            DesalocaPosicao(proxima_posicao); 
        }
        tPosicao* posicao;
        if(fantasma->flag_parede == 1){
            posicao = CriaPosicao(linha_2, coluna_2);    
        } else{
            posicao = CriaPosicao(linha_1, coluna_1);  
        }
        if(EncontrouComidaMapa(mapa, posicao)){
            if(fantasma->flag_comida == 0){
                flag_jogada_atual = 1;
            } else if(fantasma->flag_comida == 1){
                segunda_comida = 1;
            }
            fantasma->flag_comida = 1;
        }
        if(SaoIguaisPosicao(posicao, posicao_pac) || (SaoIguaisPosicao(posicao_antiga, posicao_nova_pacman) && (ObtemLinhaPosicao(posicao) == linha_old_pac) && (ObtemColunaPosicao(posicao) == coluna_old_pac))){
            MataPacman(pacman);
        }
        if(AtualizaItemMapa(mapa, posicao, 'C')){
            AtualizaPosicao(fantasma->posicaoAtual, posicao);
        }
        DesalocaPosicao(posicao);
    }

    if(fantasma->flag_comida == 1 && flag_jogada_atual == 0){
        if(!(SaoIguaisPosicao(posicao_antiga, posicao_pac))){
            AtualizaItemMapa(mapa, posicao_antiga, '*');      
        } else{
            IncrementaComida(pacman, jogo);
        }
        if(segunda_comida == 0){
            fantasma->flag_comida = 0;    
        }
    } else{
        if(!SaoIguaisPosicao(posicao_antiga, posicao_pac)){
            AtualizaItemMapa(mapa, posicao_antiga, ' ');    
        }
    }
    DesalocaPosicao(posicao_antiga);

}

/**
 * Caso o fantasma seja diferente de NULL, libera o espaço 
 * alocado para a estrutura tFantasma.
 * Sempre verificar se é NULL antes de dar free.
 * 
 * \param fantasma fantasma
 */
void DesalocaFantasma(tFantasma* fantasma){
    if(fantasma != NULL){
        DesalocaPosicao(fantasma->posicaoAtual);
        free(fantasma);
    }
}


int main(int argc, char const* argv[]){

    const char* caminhoConfig = argv[1];

    tMapa* mapa = CriaMapa(caminhoConfig);
    InicializaMapa(mapa, caminhoConfig);

    tJogo* jogo = CriaJogo(caminhoConfig);
    IniciaJogo(jogo, mapa, caminhoConfig);

    return 0;
}