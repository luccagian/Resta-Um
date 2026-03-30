// Gian Lucca Campanha Ribeiro (10438361)
// Gabriel Ferreira (10442043)

#include "resta_um.h"
#include <pthread.h>

int tabuleiro_atual[N][N];
Estado caminho[MAX_PASSOS];
int tamanho_caminho = 0;
long total_estados_testados = 0;
int pecas_atuais = 0;
extern pthread_mutex_t mutex_estado;

// Lê a configuração inicial do arquivo 'entrada.txt' 
void carregarTabuleiro(const char* nomeArquivo) {
    FILE *file = fopen(nomeArquivo, "r");
    if (!file) return;

    pecas_atuais = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fscanf(file, "%d", &tabuleiro_atual[i][j]);
            if (tabuleiro_atual[i][j] == 1) pecas_atuais++; // Conta pinos iniciais
        }
    }
    fclose(file);
}

// Salva a sequência de movimentos que leva à solução
void salvarSolucao(const char* nomeArquivo) {
    FILE *file = fopen(nomeArquivo, "w");
    if (!file) return;

    fprintf(file, "SOLUCAO ENCONTRADA\n\n");
    for (int k = 0; k < tamanho_caminho; k++) {
        fprintf(file, "Passo %d:\n", k);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (caminho[k].tabuleiro[i][j] == 1) fprintf(file, " @ ");
                else if (caminho[k].tabuleiro[i][j] == 0) fprintf(file, " . ");
                else fprintf(file, "   ");
            }
            fprintf(file, "\n");
        }
        fprintf(file, "\n-------------------\n");
    }
    fclose(file);
}

void imprimirEstado(Estado estado) {
    for (int i = 0; i < N; i++) {
        printf("        ");
        for (int j = 0; j < N; j++) {
            if (estado.tabuleiro[i][j] == 1) printf(" @ ");
            else if (estado.tabuleiro[i][j] == 0) printf(" . ");
            else printf("   ");
        }
        printf("\n");
    }
}

// Função recursiva de Backtracking
int Resolucao(int tab[N][N], int pecas_restantes) {
    // Objetivo: Apenas 1 pino restante na posição central
    if (pecas_restantes == 1 && tab[3][3] == 1) return 1;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (tab[i][j] == 1) { // Se encontrar um pino
                int dl[] = {-1, 1, 0, 0}, dc[] = {0, 0, -1, 1}; // Direções: N, S, W, E

                for (int d = 0; d < 4; d++) {
                    int l1 = i + dl[d], c1 = j + dc[d]; // Posição vizinha
                    int l2 = i + 2 * dl[d], c2 = j + 2 * dc[d]; // Posição de destino

                    // Valida movimento: destino dentro dos limites, vizinho é pino e destino é vazio
                    if (l2 >= 0 && l2 < N && c2 >= 0 && c2 < N &&
                        tab[l1][c1] == 1 && tab[l2][c2] == 0) {
                        
                        // Executa o movimento (captura)
                        tab[i][j] = 0; tab[l1][c1] = 0; tab[l2][c2] = 1;
                        
                        pthread_mutex_lock(&mutex_estado);
                        total_estados_testados++;
                        pecas_atuais = pecas_restantes - 1;
                        pthread_mutex_unlock(&mutex_estado);

                        // Armazena estado atual no caminho da solução
                        for(int r=0; r<N; r++) for(int s=0; s<N; s++) 
                            caminho[tamanho_caminho].tabuleiro[r][s] = tab[r][s];
                        tamanho_caminho++;

                        // Tenta resolver o próximo estado recursivamente
                        if (Resolucao(tab, pecas_restantes - 1)) return 1;

                        // Backtracking: desfaz a jogada para testar outros ramos
                        tamanho_caminho--;
                        tab[i][j] = 1; tab[l1][c1] = 1; tab[l2][c2] = 0;
                    }
                }
            }
        }
    }
    return 0; // Falha neste ramo da busca
}