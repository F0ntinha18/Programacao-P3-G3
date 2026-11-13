#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LEITURAS 50
#define TAM_LINHA 128
#define TAM_ID 20

#define TEMP_MIN_VALIDA -10.0
#define TEMP_MAX_VALIDA 190.0

typedef struct {
    char id[TAM_ID];
    double ganho;
    double desvio;
} SensorCalibracao;

typedef struct {
    char id_sensor[TAM_ID];
    int instante;
    int valor_bruto;
    double temperatura;
} Leitura;

const SensorCalibracao SENSORES_CONHECIDOS[] = {
    {"TEMP1", 260.0, -20.0},
    {"TEMP2", 250.0, -15.0},
    {"TEMP3", 240.0, -10.0}
};
const int NUM_SENSORES = sizeof(SENSORES_CONHECIDOS) / sizeof(SENSORES_CONHECIDOS[0]);

int procurar_sensor(const char *id);
double converter_temperatura(int valor, double ganho, double desvio);
int temperatura_valida(double temp);
int is_duplicada(const Leitura leituras[], int total, const char *id, int instante);
int ler_todas_leituras(Leitura leituras[], int max_leituras);
void calcular_estatisticas_sensor(const Leitura leituras[], int total, const char *id_sensor,
                                  int *count, double *min, int *inst_min,
                                  double *max, int *inst_max, double *media, double *desvio);
void apresentar_relatorio(const Leitura leituras[], int total);

int main(void) {
    Leitura leituras[MAX_LEITURAS];

    int total_lidos = ler_todas_leituras(leituras, MAX_LEITURAS);
    apresentar_relatorio(leituras, total_lidos);

    return 0;
}

int procurar_sensor(const char *id) {
    for (int i = 0; i < NUM_SENSORES; i++) {
        if (strcmp(SENSORES_CONHECIDOS[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

double converter_temperatura(int valor, double ganho, double desvio) {
    return (ganho * valor) / 1023.0 + desvio;
}

int temperatura_valida(double temp) {
    return (temp >= TEMP_MIN_VALIDA && temp <= TEMP_MAX_VALIDA);
}

int is_duplicada(const Leitura leituras[], int total, const char *id, int instante) {
    for (int i = 0; i < total; i++) {
        if (leituras[i].instante == instante && strcmp(leituras[i].id_sensor, id) == 0) {
            return 1;
        }
    }
    return 0;
}

int ler_todas_leituras(Leitura leituras[], int max_leituras) {
    char linha[TAM_LINHA];
    int total = 0;

    printf("Formato esperado: SENSOR INSTANTE VALOR\n");
    printf(" - SENSOR: TEMP1, TEMP2 ou TEMP3\n");
    printf(" - INSTANTE: numero inteiro (ex: 1, 2, 3...)\n");
    printf(" - VALOR: numero inteiro entre 0 e 1023\n");
    printf("Exemplo: TEMP1 1 234 (ou escreva FIM para terminar)\n\n");

    while (total < max_leituras) {
        printf("Leitura: ");

        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            break;
        }

        char id[TAM_ID];
        int instante, valor;
        char extra;

        int itens_palavra = sscanf(linha, " %19s", id);
        if (itens_palavra <= 0) {
            continue; // Linha em branco -> ignora
        }

        if (strcmp(id, "FIM") == 0) {
            if (sscanf(linha, " %19s %c", id, &extra) == 1) {
                break;
            }
        }

        int idx_sensor = procurar_sensor(id);
        if (idx_sensor == -1) {
            printf("Sensor desconhecido\n");
            continue;
        }

        int campos = sscanf(linha, " %19s %d %d %c", id, &instante, &valor, &extra);
        if (campos != 3 || valor < 0 || valor > 1023) {
            printf("Linha inválida\n");
            continue;
        }

        if (is_duplicada(leituras, total, id, instante)) {
            printf("Linha duplicada\n");
            continue;
        }

        double temp = converter_temperatura(valor, SENSORES_CONHECIDOS[idx_sensor].ganho, SENSORES_CONHECIDOS[idx_sensor].desvio);

        if (!temperatura_valida(temp)) {
            printf("Valor fora da gama\n");
            continue;
        }

        printf("%.2f\n", temp);
        strcpy(leituras[total].id_sensor, id);
        leituras[total].instante = instante;
        leituras[total].valor_bruto = valor;
        leituras[total].temperatura = temp;
        total++;
    }

    return total;
}

void calcular_estatisticas_sensor(const Leitura leituras[], int total, const char *id_sensor,
                                  int *count, double *min, int *inst_min,
                                  double *max, int *inst_max, double *media, double *desvio) {
    *count = 0;
    double soma = 0.0;

    for (int i = 0; i < total; i++) {
        if (strcmp(leituras[i].id_sensor, id_sensor) == 0) {
            double t = leituras[i].temperatura;
            int inst = leituras[i].instante;

            if (*count == 0) {
                *min = *max = t;
                *inst_min = *inst_max = inst;
            } else {
                if (t < *min) {
                    *min = t;
                    *inst_min = inst;
                }
                if (t > *max) {
                    *max = t;
                    *inst_max = inst;
                }
            }
            soma += t;
            (*count)++;
        }
    }

    if (*count == 0) return;

    *media = soma / (*count);

    double soma_desvios = 0.0;
    for (int i = 0; i < total; i++) {
        if (strcmp(leituras[i].id_sensor, id_sensor) == 0) {
            soma_desvios += pow(leituras[i].temperatura - (*media), 2);
        }
    }
    *desvio = sqrt(soma_desvios / (*count));
}

void apresentar_relatorio(const Leitura leituras[], int total) {
    if (total == 0) {
        printf("\nNenhuma leitura registada.\n");
        return;
    }

    printf("\nLeituras registadas (sensor instante: valor -> temperatura):\n");
    for (int i = 0; i < total; i++) {
        printf("%s %d: %3d -> %6.2f\n",
               leituras[i].id_sensor,
               leituras[i].instante,
               leituras[i].valor_bruto,
               leituras[i].temperatura);
    }

    for (int s = 0; s < NUM_SENSORES; s++) {
        int count, inst_min, inst_max;
        double min, max, media, desvio;

        calcular_estatisticas_sensor(leituras, total, SENSORES_CONHECIDOS[s].id,
                                     &count, &min, &inst_min, &max, &inst_max, &media, &desvio);

        if (count > 0) {
            printf("\nSensor %s\n", SENSORES_CONHECIDOS[s].id);
            printf("Número de valores lidos: %d\n", count);
            printf("Temperatura mínima:    %6.2f (instante %d)\n", min, inst_min);
            printf("Temperatura máxima:    %6.2f (instante %d)\n", max, inst_max);
            printf("Temperatura média:     %6.2f\n", media);
            printf("Desvio-padrão:         %6.2f\n", desvio);
        }
    }
}