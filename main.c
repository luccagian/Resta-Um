// Gian Lucca Campanha Ribeiro (10438361)
// Gabriel Ferreira (10442043)

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define N 7
#define TAM_BARRA 20
#define MAX_PASSOS 100

// controle da thread de loading
int rodando = 1;
// quantidade de estados explorados pela busca
long total_estados_testados = 0;

// total de pecas no inicio e durante a busca
int pecas_iniciais = (N * N - 17); // 32 peças
int pecas_atuais = (N * N - 17);

// marca o inicio para medir tempo de execucao
time_t inicio_execucao;

// protege variaveis compartilhadas entre as threads
pthread_mutex_t mutex_estado;

// -1 = fora do tabuleiro, 1 = peca, 0 = vazio
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
    // snapshot completo do tabuleiro em um passo da busca
    int tabuleiro[N][N];
} Estado;

Estado caminho[MAX_PASSOS];
Estado solucao[MAX_PASSOS];

int tamanho_caminho = 0;
int tamanho_solucao = 0;

// função que calcula o progresso da resolução
int calcularProgresso(int atuais) {
    int removidas = pecas_iniciais - atuais;
    return (removidas * 100) / (pecas_iniciais - 1);
}

// função que cria a barra de progresso
void barraProgresso(char *barra, int progresso) {
    if (progresso > 100) progresso = 100;
    int posicao_preenchida = (progresso * TAM_BARRA) / 100;

    for (int i = 0; i < TAM_BARRA; i++) {
        barra[i] = (i < posicao_preenchida) ? '#' : '-';
    }
    barra[TAM_BARRA] = '\0';
}

// função que carrega o loading e o tempo percorrido
void* loading(void* arg) {
    char barra[TAM_BARRA + 1];

    // atualiza periodicamente a barra enquanto a busca estiver rodando
    while (rodando) {
        // leitura segura dos dados compartilhados entre as threads
        pthread_mutex_lock(&mutex_estado);
        long estados = total_estados_testados;
        int pecas = pecas_atuais;
        pthread_mutex_unlock(&mutex_estado);

        int progresso = calcularProgresso(pecas);
        double tempo = difftime(time(NULL), inicio_execucao);

        barraProgresso(barra, progresso);

        printf("\r[%s] %d%% | Estados: %ld | Tempo: %.1fs",
               barra, 
               progresso, 
               estados, 
               tempo);

        fflush(stdout);
        usleep(100000);
    }

    // impressao final ao encerrar a busca
    pthread_mutex_lock(&mutex_estado);
    long estados = total_estados_testados;
    pthread_mutex_unlock(&mutex_estado);

    char barra_final[TAM_BARRA + 1];
    barraProgresso(barra_final, 100);
    double tempo_total = difftime(time(NULL), inicio_execucao);

    printf("\r[%s] 100%% | Estados: %ld | Tempo: %.1fs\n",
           barra_final, 
           estados, 
           tempo_total);

    return NULL;
}

void imprimirEstado(Estado estado) {
    for (int i = 0; i < N; i++) {
        printf("        "); // recuo central
        for (int j = 0; j < N; j++) {
            if (estado.tabuleiro[i][j] == 1)
                printf(" @ "); // Peça
            else if (estado.tabuleiro[i][j] == 0)
                printf(" . "); // Espaço vazio
            else
                printf("   "); // Fora do tabuleiro
        }
        printf("\n");
    }
    printf("\n");
}

void imprimirSolucao() {
    // imprime todos os estados desde o inicio ate a solucao com efeito de animação
    for (int i = 0; i < tamanho_solucao; i++) {
        printf("\033[H\033[J"); // Limpa a tela (ANSI escape)
        printf("=== SOLUCAO ENCONTRADA - PASSO %d/%d ===\n\n", i + 1, tamanho_solucao);
        imprimirEstado(solucao[i]);
        usleep(300000);
    }
}

Estado copiarTabuleiro(int matriz_origem[N][N]) {
    Estado estado_copiado;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            estado_copiado.tabuleiro[i][j] = matriz_origem[i][j];
        }
    }
    return estado_copiado;
}

int Resolucao(int tabuleiro[N][N], int pecas_restantes) {
    // caso base: encerra apenas com uma peca no centro
    if (pecas_restantes == 1 && tabuleiro[3][3] == 1) {
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
                // deslocamentos para cima, baixo, esquerda e direita
                int passo_linha[4] = {-1, 1, 0, 0};
                int passo_coluna[4] = {0, 0, -1, 1};

                for (int d = 0; d < 4; d++) {
                    int linha = i + passo_linha[d];
                    int coluna = j + passo_coluna[d];

                    int linha_destino = i + 2 * passo_linha[d];
                    int coluna_destino = j + 2 * passo_coluna[d];

                    if (linha_destino >= 0 && linha_destino < N && coluna_destino >= 0 && coluna_destino < N &&
                        linha >= 0 && linha < N && coluna >= 0 && coluna < N &&
                        tabuleiro[linha][coluna] == 1 && tabuleiro[linha_destino][coluna_destino] == 0) {

                        // fazer movimento
                        tabuleiro[i][j] = 0;
                        tabuleiro[linha][coluna] = 0;
                        tabuleiro[linha_destino][coluna_destino] = 1;

                        // atualiza contadores compartilhados
                        total_estados_testados++;
                        pecas_atuais = pecas_restantes - 1;

                        // salva estado
                        caminho[tamanho_caminho++] = copiarTabuleiro(tabuleiro);

                        // recursao
                        if (Resolucao(tabuleiro, pecas_restantes - 1)) {
                            return 1;
                        }

                        // backtrack: desfaz o movimento para testar outro ramo
                        tamanho_caminho--;
                        tabuleiro[i][j] = 1;
                        tabuleiro[linha][coluna] = 1;
                        tabuleiro[linha_destino][coluna_destino] = 0;
                        pecas_atuais = pecas_restantes;
                    }
                }
            }
        }
    }
    return 0;
}

int main() {
    pthread_t thread_loading;

    // inicializa recursos de concorrencia
    pthread_mutex_init(&mutex_estado, NULL);
    inicio_execucao = time(NULL);
    pthread_create(&thread_loading, NULL, loading, NULL);

    // primeiro estado do caminho e inicio da busca recursiva
    caminho[tamanho_caminho++] = copiarTabuleiro(tabuleiro);
    Resolucao(tabuleiro, pecas_atuais);

    // encerra a thread de loading e libera recursos
    rodando = 0;
    pthread_join(thread_loading, NULL);
    pthread_mutex_destroy(&mutex_estado);

    printf("\nResultado final exibido abaixo:\n\n");
    sleep(1);
    imprimirSolucao();

    return 0;
}