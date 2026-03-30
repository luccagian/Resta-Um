// Gian Lucca Campanha Ribeiro (10438361)
// Gabriel Ferreira (10442043)

#include "resta_um.h"
#include <pthread.h>
#include <unistd.h>
#include <time.h>

int rodando = 1;
pthread_mutex_t mutex_estado;
time_t inicio;

// Thread secundária para feedback visual
void* loading(void* arg) {
    while (rodando) {
        pthread_mutex_lock(&mutex_estado);
        printf("\r[PROCESSANDO] Estados: %ld | Pecas: %d | Tempo: %.1fs", 
               total_estados_testados, pecas_atuais, difftime(time(NULL), inicio));
        pthread_mutex_unlock(&mutex_estado);
        fflush(stdout);
        usleep(150000);
    }
    return NULL;
}

// Reproduz a solução encontrada com limpeza de tela ANSI 
void animarSolucao() {
    for (int i = 0; i < tamanho_caminho; i++) {
        printf("\033[H\033[J"); // Limpa tela
        printf("PASSO %d/%d\n\n", i + 1, tamanho_caminho);
        imprimirEstado(caminho[i]);
        usleep(300000); // Pausa para visualização humana
    }
}

int main() {
    pthread_t th;
    pthread_mutex_init(&mutex_estado, NULL);
    
    carregarTabuleiro("entrada.txt"); // 
    inicio = time(NULL);
    pthread_create(&th, NULL, loading, NULL);

    // Inicia algoritmo de busca por retrocesso
    if (Resolucao(tabuleiro_atual, pecas_atuais)) {
        rodando = 0;
        pthread_join(th, NULL);
        
        salvarSolucao("restaum.out"); // 
        printf("\n\nSolucao encontrada e salva em 'restaum.out'.\n");
        sleep(2);
        animarSolucao(); // Exibe sequência final na tela 
    } else {
        rodando = 0;
        pthread_join(th, NULL);
        printf("\nNao foi possivel encontrar uma solucao.\n");
    }

    pthread_mutex_destroy(&mutex_estado);
    return 0;
}