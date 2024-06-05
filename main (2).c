#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definições de constantes
#define MAX_HEALTH 1000
#define MAX_STAMINA 1000
#define MAX_JOGADA 4

// Estrutura para armazenar o estado do jogador
typedef struct  {
    int vida;
    int estamina;
    struct Estado *prox;
} Estado;

typedef struct {
    int vida;
    int estamina;
    Estado *historico;
} Jogador;

// Tabela de resultados dos ataques
int tabela_resultados[11][11] = {
    {0, -25, 20, -15, 30, -10, 15, -5, 10, -20, 0},  // Zarabatana
    {25, 0, -20, 30, -15, 10, -5, 15, -10, 20, 0},  // Pontapé
    {-20, 20, 0, 25, -10, 15, -5, 30, -15, 10, 0},  // Agarrar
    {15, -30, -25, 0, 20, -20, 10, -10, 5, -15, 0},  // Estalada
    {-30, 15, 10, -20, 0, 25, -5, 20, -10, 30, 0},  // Tombeta
    {10, -10, -15, 20, -25, 0, 5, -30, 15, -20, 0},  // Rasteira
    {-15, 5, 5, -10, 5, -5, 0, 15, -20, 25, 0},  // Cotovelada
    {5, -15, -30, 10, -20, 30, -15, 0, 20, -10, 0},  // Bicada
    {-10, 10, 15, -5, 10, -15, 20, -20, 0, 5, 0},  // Onda de Choque
    {20, -20, -10, 15, -30, 20, -25, 10, -5, 0, 0},  // Murro
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  // Defender
};

// Funções de utilidade
void adicionar_ao_historico(Jogador *jogador) {
    Estado *novo_estado = (Estado *)malloc(sizeof(Estado));
    novo_estado->vida = jogador->vida;
    novo_estado->estamina = jogador->estamina;
    novo_estado->prox = jogador->historico;
    jogador->historico = novo_estado;
}

void imprimir_historico(Jogador *jogador) {
    printf("Histórico dos últimos 20 ataques:\n");
    Estado *atual = jogador->historico;
    for (int i = 0; i < 20 && atual != NULL; i++) {
        printf("Vida: %d, Estamina: %d\n", atual->vida, atual->estamina);
        atual = atual->prox;
    }
}

void imprimir_estado_jogador(Jogador *jogador, int numero) {
    printf("P#%d: %d de vida, %d de estamina\n", numero, jogador->vida, jogador->estamina);
}

// Funções de ataque e combos
int calcular_dano(char ataque_jogador1, char ataque_jogador2, int estamina) {
    int dano_base = tabela_resultados[ataque_jogador1 - 'A'][ataque_jogador2 - 'A'];
    if (estamina > 750) return dano_base;
    if (estamina > 500) return 2 * dano_base;
    if (estamina > 250) return 3 * dano_base;
    return 4 * dano_base;
}

void aplicar_ataque(Jogador *atacante, Jogador *defensor, char ataque) {
    int dano = calcular_dano(ataque, ' ', defensor->estamina); // ' ' representa nenhum ataque em resposta
    defensor->vida -= dano;
    atacante->estamina -= (ataque == 'D') ? 10 : 25;
    if (ataque == 'D') {
        int vida_recuperada = (defensor->estamina > 750) ? 10 : (defensor->estamina > 500) ? 20 : (defensor->estamina > 250) ? 30 : 40;
        atacante->vida += vida_recuperada;
        if (atacante->vida > 1000) atacante->vida = 1000;
    }
    if (atacante->estamina < 0) atacante->estamina = 0;
}

void aplicar_combo(Jogador *atacante, Jogador *defensor, char *combo) {
    if (atacante->estamina < 750) {
        printf("Estamina Insuficiente\n");
        return;
    }
    int dano, estamina_perdida;
    if (strcmp(combo, "ARROZAO") == 0) {
        dano = 500;
        estamina_perdida = 500;
    } else if (strcmp(combo, "DADBAD") == 0) {
        dano = 400;
        estamina_perdida = 400;
    } else if (strcmp(combo, "STTEACC") == 0) {
        dano = 300;
        estamina_perdida = 300;
    } else if (strcmp(combo, "TATAPAAA") == 0) {
        dano = 200;
        estamina_perdida = 200;
    } else {
        printf("Entrada inválida\n");
        exit(EXIT_FAILURE);
    }
    defensor->vida -= dano;
    atacante->estamina -= estamina_perdida;
    if (atacante->estamina < 0) atacante->estamina = 0;
}

// Função para jogada especial "Lucio Tarzan Reversal"
void aplicar_lucio_tarzan_reversal(Jogador *jogador, Jogador *oponente, int retroceder) {
    if (retroceder <= 0) {
        printf("Entrada inválida\n");
        exit(EXIT_FAILURE);
    }

    // Reverter o histórico do jogador
    Estado *atual = jogador->historico;
    Estado *anterior = NULL;
    for (int i = 0; i < retroceder && atual != NULL; i++) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual != NULL) {
        jogador->vida = atual->vida;
        jogador->estamina = atual->estamina;
        jogador->historico = atual;
        if (anterior != NULL) {
            anterior->prox = NULL;
        }
    } else {
        // Se retroceder for maior que o número de jogadas, reinicia o jogo
        jogador->vida = MAX_HEALTH;
        jogador->estamina = MAX_STAMINA;
        jogador->historico = NULL;
    }

    // Reverter o histórico do oponente
    atual = oponente->historico;
    anterior = NULL;
    for (int i = 0; i < retroceder && atual != NULL; i++) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual != NULL) {
        oponente->vida = atual->vida;
        oponente->estamina = atual->estamina;
        oponente->historico = atual;
        if (anterior != NULL) {
            anterior->prox = NULL;
        }
    } else {
        // Se retroceder for maior que o número de jogadas, reinicia o jogo
        oponente->vida = MAX_HEALTH;
        oponente->estamina = MAX_STAMINA;
        oponente->historico = NULL;
    }
}

// Função principal do jogo
void realizar_jogada(Jogador *jogador1, Jogador *jogador2, char *jogada1, char *jogada2) {
    // Primeiro, processa a jogada do jogador 1
    for (int i = 0; i < MAX_JOGADA && jogada1[i] != '\0'; i++) {
        aplicar_ataque(jogador1, jogador2, jogada1[i]);
    }
    adicionar_ao_historico(jogador1);

    // Em seguida, processa a jogada do jogador 2
    for (int i = 0; i < MAX_JOGADA && jogada2[i] != '\0'; i++) {
        aplicar_ataque(jogador2, jogador1, jogada2[i]);
    }
    adicionar_ao_historico(jogador2);

    // Verifica se alguém ganhou
    if (jogador1->vida <= 0 && jogador2->vida <= 0) {
        printf("Empate!\n");
        exit(EXIT_SUCCESS);
    } else if (jogador1->vida <= 0) {
        printf("Jogador 2 venceu!\n");
        exit(EXIT_SUCCESS);
    } else if (jogador2->vida <= 0) {
        printf("Jogador 1 venceu!\n");
        exit(EXIT_SUCCESS);
    }
}

// Função main
int main(int argc, char *argv[]) {
    Jogador jogador1 = {MAX_HEALTH, MAX_STAMINA, NULL};
    Jogador jogador2 = {MAX_HEALTH, MAX_STAMINA, NULL};

    char jogada1[MAX_JOGADA + 1];
    char jogada2[MAX_JOGADA + 1];

    while (1) {
        imprimir_estado_jogador(&jogador1, 1);
        imprimir_historico(&jogador1);

        imprimir_estado_jogador(&jogador2, 2);
        imprimir_historico(&jogador2);

        printf("Jogador 1, insira a sua jogada: ");
        scanf("%s", jogada1);
        printf("Jogador 2, insira a sua jogada: ");
        scanf("%s", jogada2);

        realizar_jogada(&jogador1, &jogador2, jogada1, jogada2);
    }

    return 0;
}


