// Gian Lucca Campanha Ribeiro (10438361)
// Gabriel Ferreira (10442043)
// Matheus Regatieri Olivieri (10409590)

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#define N 7
#define TAM_BARRA 20


int rodando = 1;
long estados_testados = 0;

int pecas_iniciais = (N * N - 17);
int pecas_atuais = (N * N - 17);

time_t inicio;

pthread_mutex_t lock;

int tabuleiro [N][N] = 
{#,#,1,1,1,#,#},
{#,#,1,1,1,#,#},
{1,1,1,1,1,1,1},
{1,1,1,0,1,1,1},
{1,1,1,1,1,1,1},
{#,#,1,1,1,#,#},
{#,#,1,1,1,#,#}

// função que calcula o progresso da resolução
int calcularProgresso() {
    return ((pecas_iniciais - pecas_atuais_local) * 100) / (pecas_iniciais - 1);
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

    char barra_final[LARGURA_BARRA + 1];
    barraProgresso(barra_final, 100);

    double tempo_total = difftime(time(NULL), inicio);

    printf("\r[%s] Estados: %ld | Tempo: %.1fs\n",
           barra_final,
           estados,
           tempo_total);

    return NULL;
}

// função da resolução do jogo 
// inserir no backtracking
/*
ao fazer o movimento
-----
pthread_mutex_lock(&lock);

estados_testados++;
pecas_atuais--;

pthread_mutex_unlock(&lock);

ao fazer movimento
-----
pthread_mutex_lock(&lock);

estados_testados++;
pecas_atuais--;

pthread_mutex_unlock(&lock);
*/

Resolucao(int tab [N][N]){
        se só resta 1 peça:
        retornar verdadeiro

    para cada movimento possível:

        se movimento é válido:

            fazer movimento

            lock(mutex)
                estados_testados++
                pecas_atuais--
            unlock(mutex)

            se resolver(tabuleiro):
                retornar verdadeiro

            desfazer movimento

            lock(mutex)
                pecas_atuais++
            unlock(mutex)

    return false;

}


int main() {
    pthread_t t;

    pthread_mutex_init(&lock, NULL);

    inicio = time(NULL);

    pthread_create(&t, NULL, loading, NULL);

    Resolucao(tabuleiro [N][N]);

    rodando = 0;

    pthread_join(t, NULL);

    pthread_mutex_destroy(&lock);

    printf("Resultado final exibido acima.\n");

    return 0;
}