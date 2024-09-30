// Este programa em C monitora o arquivo de log do sistema para identificar eventos de rede,
// como conexões e desconexões do NetworkManager.
// Ele armazena esses eventos em uma lista duplamente encadeada,
// e oferece um menu interativo que permite listar, filtrar, remover e contar os eventos registrados.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024
#define LOG_FILE "/var/log/syslog"

// Estrutura para armazenar um evento de rede
typedef struct Evento {
    int codigo;                  // Código único do evento
    char dataHora[30];           // Data e hora do evento
    char tipo[20];               // Tipo do evento: CONECTADO ou DESCONECTADO
    struct Evento *anterior;     // Ponteiro para o nó anterior
    struct Evento *proximo;      // Ponteiro para o próximo nó
} Evento;

// Ponteiros globais para a cabeça e a cauda da lista
Evento *cabeca = NULL;
Evento *cauda = NULL;
int codigoAtual = 1;  // Código incremental para cada evento

// Função para adicionar um evento à lista
void adicionar_evento(const char *dataHora, const char *tipo) {
    // Aloca memória para um novo evento
    Evento *novoEvento = (Evento *)malloc(sizeof(Evento));
    novoEvento->codigo = codigoAtual++;
    strcpy(novoEvento->dataHora, dataHora);
    strcpy(novoEvento->tipo, tipo);
    novoEvento->proximo = NULL;
    novoEvento->anterior = cauda;

    // Se a lista estiver vazia, o novo evento será a cabeça e a cauda
    if (cauda == NULL) {
        cabeca = novoEvento;
    } else {
        cauda->proximo = novoEvento;
    }
    cauda = novoEvento;  // Atualiza a cauda para o novo evento
}

// Função para remover um evento pelo código
void remover_evento(int codigo) {
    Evento *atual = cabeca;

    // Percorre a lista em busca do evento com o código correspondente
    while (atual != NULL) {
        if (atual->codigo == codigo) {
            // Ajusta os ponteiros da lista para remover o evento
            if (atual->anterior != NULL) {
                atual->anterior->proximo = atual->proximo;
            } else {
                cabeca = atual->proximo;  // Atualiza a cabeça se o primeiro evento foi removido
            }

            if (atual->proximo != NULL) {
                atual->proximo->anterior = atual->anterior;
            } else {
                cauda = atual->anterior;  // Atualiza a cauda se o último evento foi removido
            }

            free(atual);  // Libera a memória alocada para o evento
            printf("Evento %d removido com sucesso.\n", codigo);
            return;
        }
        atual = atual->proximo;
    }
    printf("Evento com o código %d não encontrado.\n", codigo);
}

// Função para listar todos os eventos
void listar_eventos() {
    Evento *atual = cabeca;
    if (atual == NULL) {
        printf("Nenhum evento registrado.\n");
        return;
    }

    printf("Lista de eventos:\n");
    while (atual != NULL) {
        printf("Código: %d | Data e Hora: %s | Tipo: %s\n", atual->codigo, atual->dataHora, atual->tipo);
        atual = atual->proximo;
    }
}

// Função para listar os eventos por tipo
void listar_tipo(const char *tipo) {
    Evento *atual = cabeca;
    if (atual == NULL) {
        printf("Nenhum evento registrado.\n");
        return;
    }

    printf("Lista de eventos do tipo '%s':\n", tipo);
    int encontrou = 0;  // Variável para rastrear se algum evento foi encontrado

    while (atual != NULL) {
        // Verifica se o tipo atual é "CONECTADO" ou "DESCONECTADO" usando strcasecmp
        if (strcasecmp(atual->tipo, "CONECTADO") == 0 && strcasecmp(tipo, "conexão") == 0) {
            printf("Código: %d | Data e Hora: %s | Tipo: %s\n", atual->codigo, atual->dataHora, atual->tipo);
            encontrou = 1;  // Marca que encontrou um evento
        } else if (strcasecmp(atual->tipo, "DESCONECTADO") == 0 && strcasecmp(tipo, "desconexão") == 0) {
            printf("Código: %d | Data e Hora: %s | Tipo: %s\n", atual->codigo, atual->dataHora, atual->tipo);
            encontrou = 1;  // Marca que encontrou um evento
        }
        atual = atual->proximo; // Avança para o próximo evento
    }

    // Se nenhum evento foi encontrado, exibe uma mensagem
    if (!encontrou) {
        printf("Nenhum evento encontrado do tipo '%s'.\n", tipo);
    }
}

// Função para contar o número total de eventos
int contar_eventos() {
    int contador = 0;
    Evento *atual = cabeca;

    while (atual != NULL) {
        contador++;
        atual = atual->proximo;
    }
    return contador;
}

void filtrar_eventos(const char *linha) {
    // Captura os primeiros 15 caracteres da linha que contém a data e hora
    char dataHora[30];
    strncpy(dataHora, linha, 15);
    dataHora[15] = '\0';  // Finaliza a string corretamente

    // Verifica se a linha contém "NetworkManager"
    if (strcasestr(linha, "NetworkManager")) {
        char tipo[20];

        // Evento de Conexão
        if (strcasestr(linha, "link connected")) { // Indica que a conexão física do dispositivo à rede foi estabelecida.
            strcpy(tipo, "CONECTADO");
            adicionar_evento(dataHora, tipo);
        }

        // Evento de Desconexão
        if (strcasestr(linha, "NetworkManager state is now DISCONNECTED")) { // Indica que o gerenciador de rede detectou que o dispositivo foi desconectado da rede.
            strcpy(tipo, "DESCONECTADO");
            adicionar_evento(dataHora, tipo);
        }
    }
}

// Função principal
int main() {
    // Abre o arquivo de log para leitura
    FILE *file = fopen(LOG_FILE, "r");
    if (file == NULL) {
        perror("Erro ao abrir o syslog");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    printf("Monitorando o arquivo %s...\n", LOG_FILE);

    // Lê o arquivo linha por linha e filtra os eventos
    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        filtrar_eventos(buffer);
    }

    fclose(file);  // Fecha o arquivo após a leitura

    // Exibe as opções do menu
    int opcao, codigo;
    char tipo[20]; // Variável para armazenar o tipo de evento
    do {
        printf("\nMenu de Operações:\n");
        printf("1. Listar todos os eventos\n");
        printf("2. Filtrar e listar eventos por tipo\n");
        printf("3. Remover um evento\n");
        printf("4. Contar eventos\n");
        printf("5. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                listar_eventos();
                break;
            case 2:
                printf("Digite o tipo de evento que deseja listar (Conexão ou Desconexão): ");
                scanf("%s", tipo);
                listar_tipo(tipo);
                break;
            case 3:
                printf("Digite o código do evento para remover: ");
                scanf("%d", &codigo);
                remover_evento(codigo);
                break;
            case 4:
                printf("Total de eventos: %d\n", contar_eventos());
                break;
            case 5:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 5);

    return 0;
}
