// Gian Lucca Campanha Ribeiro (10438361)
// Gabriel Ferreira (10442043)
// Matheus Regatieri Olivieri (10409590)

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#define N 7
#define TAM_BARRA 20
#define MAX_PASSOS 100

int rodando = 1;
long estados_testados = 0;

int pecas_iniciais = (N * N - 17);
int pecas_atuais = (N * N - 17);

time_t inicio;

pthread_mutex_t lock;

int tabuleiro[N][N] = {
    {-1,-1, 1, 1, 1,-1,-1},
    {-1,-1, 1, 1, 1,-1,-1},
    { 1, 1, 1, 1, 1, 1, 1},
    { 1, 1, 1, 0, 1, 1, 1},
    { 1, 1, 1, 1, 1, 1, 1},
    {-1,-1, 1, 1, 1,-1,-1},
    {-1,-1, 1, 1, 1,-1,-1}
};

typedef struct{
    int tabuleiro[N][N];
} Estado;

Estado caminho[MAX_PASSOS];
Estado solucao[MAX_PASSOS];

int tamanho_caminho = 0;
int tamanho_solucao = 0;

// função que calcula o progresso da resolução
int calcularProgresso() {
    return ((pecas_iniciais - pecas_iniciais) * 100) / (pecas_iniciais - 1);
}

// função que cria a barra de progresso
void barraProgresso(char *barra, int progresso) {
    int pos = (progresso * TAM_BARRA) / 100;

    for (int i = 0; i < TAM_BARRA; i++) {
        barra[i] = (i < pos) ? '#' : '-';
    }
    barra[TAM_BARRA] = '\0';
}

// função que carrega o loading e o tempo percorrido
void* loading(void* arg) {
    char barra[TAM_BARRA + 1];

    while (rodando) {

        // leitura segura
        pthread_mutex_lock(&lock);
        long estados = estados_testados;
        int pecas = pecas_atuais;
        pthread_mutex_unlock(&lock);

        int progresso = calcularProgresso(pecas);
        double tempo = difftime(time(NULL), inicio);

        barraProgresso(barra, progresso);

        printf("\r[%s] Estados: %ld | Tempo: %.1fs",
               barra,
               estados,
               tempo);

        fflush(stdout);

        usleep(100000);
    }

    // final
    pthread_mutex_lock(&lock);
    long estados = estados_testados;
    pthread_mutex_unlock(&lock);

    char barra_final[TAM_BARRA + 1];
    barraProgresso(barra_final, 100);

    double tempo_total = difftime(time(NULL), inicio);

    printf("\r[%s] | Estados: %ld | Tempo: %.1fs\n",
           barra_final,
           estados,
           tempo_total);

    return NULL;
}

void imprimirEstado(Estado e) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {

            if (e.tabuleiro[i][j] == 1)
                printf("o");
            else if (e.tabuleiro[i][j] == 0)
                printf(" ");
            else
                printf("#"); 

        }
        printf("\n");
    }
    printf("\n");
}

void imprimirSolucao() {
    for (int i = 0; i < tamanho_solucao; i++) {
        imprimirEstado(solucao[i]);
    }
}

Estado copiarTabuleiro(int tab[N][N]) {
    Estado e;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            e.tabuleiro[i][j] = tab[i][j];
        }
    }

    return e;
}

int Resolucao(int tabuleiro[N][N], int pecas_atuais) {
    // caso base
    if (pecas_atuais == 1 && tabuleiro[3][3] == 1) {

        tamanho_solucao = tamanho_caminho;
        for (int i = 0; i < tamanho_caminho; i++) {
            solucao[i] = caminho[i];
        }

        return 1;
    }

    // percorre tabuleiro
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (tabuleiro[i][j] == 1) {
                int di[4] = {-1, 1, 0, 0};
                int dj[4] = {0, 0, -1, 1};

                for (int d = 0; d < 4; d++) {
                    int ni = i + di[d];
                    int nj = j + dj[d];

                    int fi = i + 2 * di[d];
                    int fj = j + 2 * dj[d];

                    // verifica limites e movimento válido
                    if (fi >= 0 && fi < N && fj >= 0 && fj < N &&
                        ni >= 0 && ni < N && nj >= 0 && nj < N &&
                        tabuleiro[ni][nj] == 1 && tabuleiro[fi][fj] == 0) {

                        // fazer movimento
                        tabuleiro[i][j] = 0;
                        tabuleiro[ni][nj] = 0;
                        tabuleiro[fi][fj] = 1;

                        pthread_mutex_lock(&lock);
                        estados_testados++;
                        pecas_atuais--;
                        pthread_mutex_unlock(&lock);

                        // salva estado
                        caminho[tamanho_caminho++] = copiarTabuleiro(tabuleiro);

                        // recursao
                        if (Resolucao(tabuleiro, pecas_atuais)) {
                            return 1;
                        }

                        // backtrack
                        tamanho_caminho--;

                        tabuleiro[i][j] = 1;
                        tabuleiro[ni][nj] = 1;
                        tabuleiro[fi][fj] = 0;

                        pthread_mutex_lock(&lock);
                        pecas_atuais++;
                        pthread_mutex_unlock(&lock);
                    }
                }
            }
        }
    }

    return 0;
}

int main() {
    pthread_t t;
    pthread_mutex_init(&lock, NULL);
    inicio = time(NULL);
    pthread_create(&t, NULL, loading, NULL);

    caminho[tamanho_caminho++] = copiarTabuleiro(tabuleiro);
    Resolucao(tabuleiro, pecas_atuais);

    rodando = 0;

    pthread_join(t, NULL);
    pthread_mutex_destroy(&lock);

    printf("Resultado final exibido abaixo:\n");
    imprimirSolucao();

    return 0;
}