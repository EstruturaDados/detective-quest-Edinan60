#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_SIZE 10

// --- Estruturas de Dados ---

// Nó para o mapa da mansão
typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

// Nó para a árvore de busca de pistas (BST)
typedef struct PistaNode {
    char pista[100];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

// Nó para a lista encadeada na tabela hash (tratamento de colisões)
typedef struct HashNode {
    char pista[100];
    char suspeito[50];
    struct HashNode* proximo;
} HashNode;


// --- Protótipos das Funções ---
Sala* criarSala(const char* nome, const char* pista);
void explorarSalas(Sala* salaAtual, PistaNode** pistasRaiz);

PistaNode* inserirPista(PistaNode* raiz, const char* pista);
void exibirPistas(PistaNode* raiz, int* contador);

unsigned int funcaoHash(const char* str);
void inserirNaHash(HashNode* tabela[], const char* pista, const char* suspeito);
const char* encontrarSuspeito(HashNode* tabela[], const char* pista);

void verificarSuspeitoFinal(PistaNode* raizPistas, HashNode* tabelaSuspeitos[]);
void limparBufferEntrada();
void liberarRecursos(Sala* mapa, PistaNode* pistas, HashNode* tabela[]);


int main() {
    // 1. Monta o mapa da mansão
    Sala* hall = criarSala("Hall de Entrada", "Um bilhete amassado diz: 'O segredo comeca onde o conhecimento repousa.'");
    hall->esquerda = criarSala("Sala de Jantar", "");
    hall->direita = criarSala("Biblioteca", "Um livro fora do lugar tem a marca de uma 'Adaga_Antiga'.");
    hall->esquerda->esquerda = criarSala("Cozinha", "Ha um 'Frasco_de_Veneno' quase vazio sob a pia.");
    hall->direita->esquerda = criarSala("Escritorio", "Uma carta na mesa menciona uma 'Heranca_Disputada'.");
    hall->direita->direita = criarSala("Jardim_Secreto", "Pegadas de 'Botas_enlameadas' levam para longe da casa.");

    // 2. Inicializa as estruturas de dados
    PistaNode* pistasColetadas = NULL;
    HashNode* tabelaSuspeitos[HASH_TABLE_SIZE] = {NULL};

    // 3. Popula a tabela hash com as associações pista -> suspeito
    inserirNaHash(tabelaSuspeitos, "Adaga_Antiga", "Mordomo");
    inserirNaHash(tabelaSuspeitos, "Frasco_de_Veneno", "Cozinheira");
    inserirNaHash(tabelaSuspeitos, "Heranca_Disputada", "Sobrinho");
    inserirNaHash(tabelaSuspeitos, "Botas_enlameadas", "Jardineiro");
    inserirNaHash(tabelaSuspeitos, "Bilhete_amassado", "Mordomo");

    // 4. Inicia o jogo
    printf("--- Bem-vindo ao Detective Quest: O Desafio Final ---\n");
    explorarSalas(hall, &pistasColetadas);

    // 5. Fase final de acusação
    verificarSuspeitoFinal(pistasColetadas, tabelaSuspeitos);

    // 6. Libera toda a memória alocada
    liberarRecursos(hall, pistasColetadas, tabelaSuspeitos);

    return 0;
}

// Aloca e inicializa uma nova sala para o mapa
Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    strcpy(novaSala->nome, nome);
    strcpy(novaSala->pista, pista);
    novaSala->esquerda = novaSala->direita = NULL;
    return novaSala;
}

// Controla a navegação do jogador pela mansão
void explorarSalas(Sala* salaAtual, PistaNode** pistasRaiz) {
    char escolha;

    while (salaAtual != NULL) {
        printf("\n------------------------------------");
        printf("\nVoce esta em: %s\n", salaAtual->nome);

        // Coleta a pista, se houver
        if (strlen(salaAtual->pista) > 0) {
            printf("Pista encontrada: %s\n", salaAtual->pista);
            *pistasRaiz = inserirPista(*pistasRaiz, salaAtual->pista);
            strcpy(salaAtual->pista, ""); // Remove a pista para não coletar de novo
        }

        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Este eh um beco sem saida.\n");
            break;
        }
        if (salaAtual->esquerda != NULL) printf("Caminho a esquerda (e): %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita != NULL) printf("Caminho a direita (d): %s\n", salaAtual->direita->nome);
        
        printf("Para onde deseja ir? (ou 's' para sair e acusar): ");
        scanf(" %c", &escolha);
        limparBufferEntrada();

        if (escolha == 'e' || escolha == 'E') salaAtual = salaAtual->esquerda;
        else if (escolha == 'd' || escolha == 'D') salaAtual = salaAtual->direita;
        else if (escolha == 's' || escolha == 'S') break;
        else printf("Comando invalido.\n");
    }
}

// Insere uma nova pista na BST de forma ordenada
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (raiz == NULL) {
        PistaNode* novoNode = (PistaNode*)malloc(sizeof(PistaNode));
        strcpy(novoNode->pista, pista);
        novoNode->esquerda = novoNode->direita = NULL;
        return novoNode;
    }
    if (strcmp(pista, raiz->pista) < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (strcmp(pista, raiz->pista) > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    return raiz;
}

// Exibe as pistas coletadas em ordem (percurso em ordem na BST)
void exibirPistas(PistaNode* raiz, int* contador) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda, contador);
        printf("- %s\n", raiz->pista);
        (*contador)++;
        exibirPistas(raiz->direita, contador);
    }
}

// Função de hash djb2 para strings
unsigned int funcaoHash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % HASH_TABLE_SIZE;
}

// Insere uma associação pista/suspeito na tabela hash
void inserirNaHash(HashNode* tabela[], const char* pista, const char* suspeito) {
    unsigned int indice = funcaoHash(pista);
    HashNode* novoNode = (HashNode*)malloc(sizeof(HashNode));
    strcpy(novoNode->pista, pista);
    strcpy(novoNode->suspeito, suspeito);
    novoNode->proximo = tabela[indice];
    tabela[indice] = novoNode;
}

// Busca um suspeito na tabela hash com base na pista
const char* encontrarSuspeito(HashNode* tabela[], const char* pista) {
    unsigned int indice = funcaoHash(pista);
    HashNode* atual = tabela[indice];
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    return "Ninguem"; // Caso não encontre
}

// Função auxiliar recursiva para contar pistas de um suspeito
void contarPistasParaSuspeito(PistaNode* raizPistas, HashNode* tabela[], const char* suspeito, int* contagem) {
    if (raizPistas == NULL) return;
    
    const char* suspeitoDaPista = encontrarSuspeito(tabela, raizPistas->pista);
    if (strcmp(suspeitoDaPista, suspeito) == 0) {
        (*contagem)++;
    }
    
    contarPistasParaSuspeito(raizPistas->esquerda, tabela, suspeito, contagem);
    contarPistasParaSuspeito(raizPistas->direita, tabela, suspeito, contagem);
}

// Conduz a fase de julgamento final
void verificarSuspeitoFinal(PistaNode* raizPistas, HashNode* tabelaSuspeitos[]) {
    int totalPistas = 0;
    printf("\n\n--- HORA DA ACUSACAO ---\n");
    printf("Pistas coletadas em ordem:\n");
    exibirPistas(raizPistas, &totalPistas);

    if (totalPistas == 0) {
        printf("\nVoce nao coletou pistas suficientes para fazer uma acusacao.\n");
        return;
    }

    char acusado[50];
    printf("\nSuspeitos: Mordomo, Cozinheira, Sobrinho, Jardineiro\n");
    printf("Quem voce acusa? ");
    scanf("%49s", acusado);
    limparBufferEntrada();

    int contagemEvidencias = 0;
    contarPistasParaSuspeito(raizPistas, tabelaSuspeitos, acusado, &contagemEvidencias);

    printf("\n--- VEREDITO ---\n");
    if (contagemEvidencias >= 2) {
        printf("Evidencias contundentes (%d pistas) apontam para %s. Caso encerrado! Parabens, detetive!\n", contagemEvidencias, acusado);
    } else {
        printf("Apesar de sua suspeita, voce so tem %d pista(s) contra %s. O verdadeiro culpado escapou...\n", contagemEvidencias, acusado);
    }
}

// Limpa o buffer de entrada do teclado
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Libera toda a memória alocada dinamicamente
void liberarRecursos(Sala* mapa, PistaNode* pistas, HashNode* tabela[]) {
    // Liberar mapa (recursivo)
    if (mapa != NULL) {
        liberarRecursos(mapa->esquerda, NULL, NULL);
        liberarRecursos(mapa->direita, NULL, NULL);
        free(mapa);
    }
    // Liberar BST de pistas (recursivo)
    if (pistas != NULL) {
        liberarRecursos(NULL, pistas->esquerda, NULL);
        liberarRecursos(NULL, pistas->direita, NULL);
        free(pistas);
    }
    // Liberar tabela hash
    if (tabela != NULL) {
        for (int i = 0; i < HASH_TABLE_SIZE; i++) {
            HashNode* atual = tabela[i];
            while (atual != NULL) {
                HashNode* temp = atual;
                atual = atual->proximo;
                free(temp);
            }
        }
    }
}