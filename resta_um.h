// Gian Lucca Campanha Ribeiro (10438361)
// Gabriel Ferreira (10442043)

#ifndef RESTA_UM_H
#define RESTA_UM_H

#include <stdio.h>

#define N 7
#define MAX_PASSOS 100

typedef struct {
    int tabuleiro[N][N];
} Estado;

// Globais para controle de estado e busca
extern int tabuleiro_atual[N][N];
extern Estado caminho[MAX_PASSOS];
extern int tamanho_caminho;
extern long total_estados_testados;
extern int pecas_atuais;

// Assinaturas das funções principais
void carregarTabuleiro(const char* nomeArquivo);
void salvarSolucao(const char* nomeArquivo);
int Resolucao(int tabuleiro[N][N], int pecas_restantes);
void imprimirEstado(Estado estado);

#endif