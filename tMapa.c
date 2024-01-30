#include "tMapa.h"

// Retorna 1 caso a posicao indicada esteja fora do mapa e 0 caso contrario
int ConfereValidadePosicao(tMapa *mapa, tPosicao *posicao)
{
    if (ObtemColunaPosicao(posicao) >= mapa->nColunas || ObtemLinhaPosicao(posicao) >= mapa->nLinhas || ObtemColunaPosicao(posicao) < 0 || ObtemLinhaPosicao(posicao) < 0)
    {
        return 1;
    }
    return 0;
}

tMapa *CriaMapa(const char *caminhoConfig)
{
    tMapa *mapa = (tMapa *) calloc (1, sizeof(tMapa));
    mapa->grid = (char**)calloc(1, sizeof(char*));

    if(mapa == NULL || mapa->grid == NULL){
        printf("Falha na alocacao de memoria.\n");
        return NULL;
    }

    char imprimir_mapa[1001];
    sprintf(imprimir_mapa, "%s/mapa.txt", caminhoConfig); // RECEBE CAMINHO DO DIRETORIO E O MAPA

    FILE *file = fopen(imprimir_mapa, "r");
    if (file == NULL)
    {
        printf("ERRO: O arquivo %s nao pode ser lido. O arquivo esperado e o 'mapa.txt'\n ", caminhoConfig);
        exit(1);
    }

    fscanf(file, "%d", &mapa->nMaximoMovimentos);
    fscanf(file, "%*[^\n]");
    fscanf(file, "%*c");

    while(1) {
        char *line = NULL;
        int size = 0;
        int c;

        while((c = fgetc(file)) != '\n' && c != EOF){
            size++;
            line = realloc(line, size);

            if (line == NULL) {
                printf("Falha na alocacao de memoria.\n");
                exit(1);
            }

            line[size - 1] = c;
        }

        line = realloc(line, size + 1);
        line[size] = '\0';

        if(line[0] == '\0'){
            free(line);
            break;
        }

        mapa->nLinhas++;
        mapa->nColunas = size;

        mapa->grid = (char**)realloc(mapa->grid, mapa->nLinhas * sizeof(char*));
        mapa->grid[mapa->nLinhas - 1] = (char*)calloc(mapa->nColunas, sizeof(char));

        if (mapa == NULL || mapa->grid == NULL) {
            printf("Falha na alocacao de memoria.\n");
            exit(1);
        }

        strncpy(mapa->grid[mapa->nLinhas - 1], line, size);
        free(line);
    }

    int flag=0, um_linha=0, um_coluna=0, dois_linha=0, dois_coluna=0;
    for (int i = 0; i < mapa->nLinhas; i++)
    {
        for (int j = 0; j < mapa->nColunas; j++)
        {
            if (mapa->grid[i][j]  == '*'){
               (mapa->nFrutasAtual)++; 
            } 

            if(mapa->grid[i][j] == '@' && flag == 0){
                um_linha = i;
                um_coluna = j;
                flag++;
            } else if(mapa->grid[i][j] == '@' && flag == 1){
                dois_linha = i;
                dois_coluna = j;
            }
        }
    }

    if(flag != 0){
        mapa->tunel = CriaTunel(um_linha, um_coluna, dois_linha, dois_coluna);    
    }
    
    fclose(file);
    return mapa;
}

tPosicao *ObtemPosicaoItemMapa(tMapa *mapa, char item)
{
    if (mapa == NULL || mapa->grid == NULL){
        return NULL; 
    } 

    for (int i = 0; i < mapa->nLinhas; i++)
    {
        for (int j = 0; j < mapa->nColunas; j++)
        {
            if (mapa->grid[i][j] == item)
            {
                return CriaPosicao(i, j);
            }
        }
    }

    return NULL;
}

tTunel *ObtemTunelMapa(tMapa *mapa)
{
    return mapa->tunel;
}

char ObtemItemMapa(tMapa *mapa, tPosicao *posicao)
{
    if (mapa == NULL || mapa->grid == NULL || ConfereValidadePosicao(mapa, posicao))
    {
        return '\0';
    }
    return mapa->grid[ObtemLinhaPosicao(posicao)][ObtemColunaPosicao(posicao)];
}

int ObtemNumeroLinhasMapa(tMapa *mapa)
{
    return mapa->nLinhas;
}

int ObtemNumeroColunasMapa(tMapa *mapa)
{
    return mapa->nColunas;
}

int ObtemQuantidadeFrutasIniciaisMapa(tMapa *mapa)
{
    return mapa->nFrutasAtual;
}

int ObtemNumeroMaximoMovimentosMapa(tMapa *mapa)
{
    return mapa->nMaximoMovimentos;
}

bool EncontrouComidaMapa(tMapa *mapa, tPosicao *posicao)
{
    if (mapa->grid == NULL || ConfereValidadePosicao(mapa, posicao))
    {
        return false;
    }
    if(mapa->grid[ObtemLinhaPosicao(posicao)][ObtemColunaPosicao(posicao)] != '*')
    {
        return false;
    }
    return true;
}

bool EncontrouParedeMapa(tMapa *mapa, tPosicao *posicao)
{
    if (mapa->grid == NULL || ConfereValidadePosicao(mapa, posicao))
    {
        return false;
    }
    if(mapa->grid[ObtemLinhaPosicao(posicao)][ObtemColunaPosicao(posicao)] != '#'){
        return false;
    } 
    return true;
}

bool AtualizaItemMapa(tMapa *mapa, tPosicao *posicao, char item)
{
    if (mapa == NULL || mapa->grid == NULL || ConfereValidadePosicao(mapa, posicao))
    {
        return false;
    }

    mapa->grid[ObtemLinhaPosicao(posicao)][ObtemColunaPosicao(posicao)] = item;
    return true;
}

bool PossuiTunelMapa(tMapa *mapa)
{
    if (mapa->tunel == NULL)
    {
        return false;
    }
    return true;
}

bool AcessouTunelMapa(tMapa *mapa, tPosicao *posicao)
{
    if (EntrouTunel(mapa->tunel, posicao))
    {
        return true;
    }
    return false;
}

void EntraTunelMapa(tMapa *mapa, tPosicao *posicao)
{
    LevaFinalTunel(mapa->tunel, posicao);
}

void DesalocaMapa(tMapa *mapa)
{
    if(mapa != NULL){
        for (int i = 0; i < mapa->nLinhas; i++)
        {
            free(mapa->grid[i]);
        }
        free(mapa->grid);
        if(mapa->tunel != NULL){
            DesalocaTunel(mapa->tunel);    
        }
        free(mapa);
    }
}
