#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Estruturas de Dados ---

// Nó para a árvore do mapa da mansão
typedef struct Sala {
    char nome[50];
    char pista[100]; // Pode ser uma string vazia se não houver pista
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

// Nó para a árvore de busca de pistas (BST)
typedef struct PistaNode {
    char pista[100];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;


// --- Protótipos das Funções ---
Sala* criarSala(const char* nome, const char* pista);
PistaNode* inserirPista(PistaNode* raiz, const char* pista);
void explorarSalasComPistas(Sala* salaAtual, PistaNode** pistasRaiz);
void exibirPistas(PistaNode* raiz);
void liberarMapa(Sala* sala);
void liberarPistas(PistaNode* raiz);
void limparBufferEntrada();


int main() {
    // Monta o mapa da mansão com pistas pré-definidas
    Sala* hall = criarSala("Hall de Entrada", "Um bilhete amassado diz: 'O segredo comeca onde o conhecimento repousa.'");
    hall->esquerda = criarSala("Sala de Jantar", "");
    hall->direita = criarSala("Biblioteca", "Um livro fora do lugar tem a marca de uma 'Adaga Antiga'.");
    
    hall->esquerda->esquerda = criarSala("Cozinha", "Ha um 'Frasco de Veneno' quase vazio sob a pia.");
    hall->esquerda->direita = criarSala("Despensa", "");
    
    hall->direita->esquerda = criarSala("Escritorio", "Uma carta na mesa menciona uma 'Heranca Disputada'.");
    hall->direita->direita = criarSala("Jardim Secreto", "Pegadas de 'Botas enlameadas' levam para longe da casa.");

    // A raiz da árvore de pistas começa como nula
    PistaNode* pistasColetadas = NULL;

    printf("--- Bem-vindo ao Detective Quest! ---\n");
    printf("Explore a mansao e colete as pistas para desvendar o misterio.\n");
    
    // Inicia a exploração
    explorarSalasComPistas(hall, &pistasColetadas);

    // Exibe o resultado final
    printf("\n\n--- Relatorio Final ---");
    exibirPistas(pistasColetadas);

    // Libera toda a memória alocada
    liberarMapa(hall);
    liberarPistas(pistasColetadas);

    return 0;
}

// Aloca e inicializa uma nova sala
Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    strcpy(novaSala->nome, nome);
    strcpy(novaSala->pista, pista);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

// Insere uma nova pista na BST de forma recursiva e ordenada
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    // Se a árvore estiver vazia, cria um novo nó
    if (raiz == NULL) {
        PistaNode* novoNode = (PistaNode*)malloc(sizeof(PistaNode));
        strcpy(novoNode->pista, pista);
        novoNode->esquerda = novoNode->direita = NULL;
        return novoNode;
    }

    // Compara as strings para decidir o caminho
    int comparacao = strcmp(pista, raiz->pista);
    
    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    // Se for igual, não faz nada para evitar duplicatas
    
    return raiz;
}

// Controla a navegação do jogador e a coleta de pistas
void explorarSalasComPistas(Sala* salaAtual, PistaNode** pistasRaiz) {
    char escolha;

    while (salaAtual != NULL) {
        printf("\n------------------------------------");
        printf("\nVoce esta em: %s\n", salaAtual->nome);

        // Verifica e coleta a pista, se houver
        if (strlen(salaAtual->pista) > 0) {
            printf("Voce encontrou uma pista: %s\n", salaAtual->pista);
            *pistasRaiz = inserirPista(*pistasRaiz, salaAtual->pista);
            // "Apaga" a pista para não ser coletada novamente
            strcpy(salaAtual->pista, ""); 
        }

        // Mostra os caminhos disponíveis
        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Este eh um beco sem saida.\n");
            break;
        }
        if (salaAtual->esquerda != NULL) printf("Caminho a esquerda (e): %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita != NULL) printf("Caminho a direita (d): %s\n", salaAtual->direita->nome);
        
        printf("Para onde deseja ir? (ou 's' para sair): ");
        scanf(" %c", &escolha);
        limparBufferEntrada();

        // Move o jogador
        if (escolha == 'e' || escolha == 'E') {
            if (salaAtual->esquerda != NULL) salaAtual = salaAtual->esquerda;
            else printf("Nao ha caminho a esquerda.\n");
        } else if (escolha == 'd' || escolha == 'D') {
            if (salaAtual->direita != NULL) salaAtual = salaAtual->direita;
            else printf("Nao ha caminho a direita.\n");
        } else if (escolha == 's' || escolha == 'S') {
            printf("\nVoce encerrou a exploracao.\n");
            break;
        } else {
            printf("Comando invalido.\n");
        }
    }
}

// Exibe as pistas em ordem alfabética (percurso em ordem)
void exibirPistas(PistaNode* raiz) {
    if (raiz == NULL) {
        printf("\nNenhuma pista foi coletada.\n");
        return;
    }
    
    // Imprime o cabeçalho apenas uma vez
    if (raiz != NULL && (raiz->esquerda == NULL || raiz->direita == NULL)) {
         printf("\nPistas coletadas em ordem alfabetica:\n");
    }

    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        exibirPistas(raiz->direita);
    }
}

// Libera a memória da árvore do mapa
void liberarMapa(Sala* sala) {
    if (sala == NULL) return;
    liberarMapa(sala->esquerda);
    liberarMapa(sala->direita);
    free(sala);
}

// Libera a memória da árvore de pistas
void liberarPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

// Limpa o buffer de entrada do teclado
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}