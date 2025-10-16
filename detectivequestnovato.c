#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para representar uma sala (nó da árvore)
typedef struct Sala {
    char nome[50];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

// --- Protótipos das Funções ---
Sala* criarSala(const char* nome);
void explorarSalas(Sala* salaAtual);
void liberarMapa(Sala* salaAtual);
void limparBufferEntrada();

int main() {
    // Monta o mapa da mansão de forma estática
    Sala* hall = criarSala("Hall de Entrada");
    hall->esquerda = criarSala("Sala de Jantar");
    hall->direita = criarSala("Biblioteca");
    
    hall->esquerda->esquerda = criarSala("Cozinha");
    hall->esquerda->direita = criarSala("Despensa");
    
    hall->direita->esquerda = criarSala("Escritorio");
    hall->direita->direita = criarSala("Jardim Secreto");

    // Inicia a exploração a partir do Hall de Entrada
    printf("--- Bem-vindo ao Detective Quest! ---\n");
    printf("Explore a mansao para desvendar o misterio.\n");
    explorarSalas(hall);

    // Libera a memória alocada para o mapa
    liberarMapa(hall);

    printf("\nObrigado por jogar!\n");
    return 0;
}

// Aloca memória para uma nova sala e define seu nome
Sala* criarSala(const char* nome) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro ao alocar memoria para uma sala.\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

// Permite a navegação interativa do jogador pela mansão
void explorarSalas(Sala* salaAtual) {
    char escolha;

    while (salaAtual != NULL) {
        printf("\nVoce esta em: %s\n", salaAtual->nome);

        // Verifica se é um beco sem saída (nó-folha)
        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Este eh um beco sem saida. Fim da exploracao neste caminho.\n");
            break;
        }

        // Mostra as opções de caminho
        if (salaAtual->esquerda != NULL) {
            printf("Caminho a esquerda (e): %s\n", salaAtual->esquerda->nome);
        }
        if (salaAtual->direita != NULL) {
            printf("Caminho a direita (d): %s\n", salaAtual->direita->nome);
        }
        printf("Para onde deseja ir? (ou 's' para sair): ");
        scanf(" %c", &escolha);
        limparBufferEntrada();

        // Navega para a próxima sala com base na escolha
        if (escolha == 'e' || escolha == 'E') {
            if (salaAtual->esquerda != NULL) {
                salaAtual = salaAtual->esquerda;
            } else {
                printf("Nao ha caminho a esquerda.\n");
            }
        } else if (escolha == 'd' || escolha == 'D') {
            if (salaAtual->direita != NULL) {
                salaAtual = salaAtual->direita;
            } else {
                printf("Nao ha caminho a direita.\n");
            }
        } else if (escolha == 's' || escolha == 'S') {
            printf("Voce decidiu parar a exploracao por enquanto.\n");
            break;
        } else {
            printf("Escolha invalida. Tente novamente.\n");
        }
    }
}

// Libera recursivamente a memória alocada para a árvore
void liberarMapa(Sala* salaAtual) {
    if (salaAtual == NULL) {
        return;
    }
    liberarMapa(salaAtual->esquerda);
    liberarMapa(salaAtual->direita);
    free(salaAtual);
}

// Limpa o buffer de entrada do teclado
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}