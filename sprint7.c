#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_LEITURAS 100
#define TAM_NOME 20

typedef struct {
    char nome[TAM_NOME];
    double ganho;
    double desvio;
    double temp_min;
    double temp_max;
} FichaSensor;

typedef struct {
    int instante;
    int valor_bruto;
    double temperatura;
} Leitura;

typedef struct {
    const FichaSensor *ficha;
    Leitura leituras[MAX_LEITURAS];
    int total;
} RegistoSensor;

const FichaSensor SENSORES_CONHECIDOS[] = {
    {"TEMP1", 260.0, -20.0, -10.0, 190.0},
    {"TEMP2", 250.0, -15.0,   0.0, 150.0},
    {"TEMP3", 240.0, -10.0, -10.0, 100.0}
};

const int NUM_SENSORES_CONHECIDOS = sizeof(SENSORES_CONHECIDOS) / sizeof(SENSORES_CONHECIDOS[0]);

const FichaSensor *procurar_sensor(const char *id, const FichaSensor *sensores, int num_sensores) {
    for (int i = 0; i < num_sensores; i++) {
        if (strcmp((sensores + i)->nome, id) == 0) {
            return sensores + i;
        }
    }
    return NULL;
}

double converter_sensor(const FichaSensor *sensor, const int *valor) {
    return (sensor->ganho * (*valor)) / 1023.0 + sensor->desvio;
}

int verificar_valida(const double *temp, const FichaSensor *sensor) {
    if (*temp < sensor->temp_min || *temp > sensor->temp_max) {
        return 0;
    }
    return 1;
}

void calcular_estatisticas(const double *temps, int tamanho, double *p_min, double *p_max, double *p_media, double *p_desvio) {
    if (tamanho <= 0) return;

    *p_min = *temps;
    *p_max = *temps;
    double soma = 0.0;
    double soma_diferencas_quadrado = 0.0;

    for (int i = 0; i < tamanho; i++) {
        if (*(temps + i) < *p_min) *p_min = *(temps + i);
        if (*(temps + i) > *p_max) *p_max = *(temps + i);
        soma += *(temps + i);
    }
    *p_media = soma / tamanho;

    for (int i = 0; i < tamanho; i++) {
        soma_diferencas_quadrado += pow(*(temps + i) - (*p_media), 2);
    }
    *p_desvio = sqrt(soma_diferencas_quadrado / tamanho);
}

int processar_linha(const char *linha, char *nome_sensor, int *instante, int *valor_bruto) {
    int campos = sscanf(linha, "%s %d %d", nome_sensor, instante, valor_bruto);
    return campos;
}

int main(void) {
    RegistoSensor registos[NUM_SENSORES_CONHECIDOS];
    for (int i = 0; i < NUM_SENSORES_CONHECIDOS; i++) {
        registos[i].ficha = &SENSORES_CONHECIDOS[i];
        registos[i].total = 0;
    }

    char linha[150];

    printf("       SISTEMA DE MONITORIZACAO DE SENSORES       \n");
    printf("Formato esperado: [SENSOR] [INSTANTE] [VALOR]\n");
    printf("Exemplo: TEMP1 1 234\n");
    printf("Insira 'FIM' para terminar o programa.\n\n");

    while (1) {
        printf("Insira a leitura: ");
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            break;
        }
        if (linha[0] == 'F' && linha[1] == 'I' && linha[2] == 'M') {
            break;
        }
        if (linha[0] == 'f' && linha[1] == 'i' && linha[2] == 'm') {
            break;
        }
        if (linha[0] == '\n' || linha[0] == '\r') {
            continue;
        }

        char nome_sensor[TAM_NOME];
        int instante;
        int valor_bruto;

        int campos_lidos = processar_linha(linha, nome_sensor, &instante, &valor_bruto);

        if (campos_lidos < 3) {
            printf("Linha inválida\n");
            continue;
        }

        const FichaSensor *sensor = procurar_sensor(nome_sensor, SENSORES_CONHECIDOS, NUM_SENSORES_CONHECIDOS);

        if (sensor == NULL) {
            printf("Sensor desconhecido\n");
            continue;
        }

        double temp_calculada = converter_sensor(sensor, &valor_bruto);

        if (!verificar_valida(&temp_calculada, sensor)) {
            printf("Valor fora da gama\n");
            continue;
        }
        printf("-> Temperatura calculada: %.2f C\n", temp_calculada);

        int idx = (int)(sensor - SENSORES_CONHECIDOS);
        RegistoSensor *reg = &registos[idx];
        if (reg->total < MAX_LEITURAS) {
            reg->leituras[reg->total].instante = instante;
            reg->leituras[reg->total].valor_bruto = valor_bruto;
            reg->leituras[reg->total].temperatura = temp_calculada;
            reg->total++;
        }
    }

    printf("\n                RELATORIO FINAL                   \n");
    printf("Leituras registadas (sensor instante: valor -> temperatura):\n");

    for (int s = 0; s < NUM_SENSORES_CONHECIDOS; s++) {
        for (int i = 0; i < registos[s].total; i++) {
            const Leitura *l = &registos[s].leituras[i];
            printf("%s %d: %d -> %.2f\n", registos[s].ficha->nome, l->instante, l->valor_bruto, l->temperatura);
        }
    }

    for (int s = 0; s < NUM_SENSORES_CONHECIDOS; s++) {
        if (registos[s].total > 0) {
            double temps_aux[MAX_LEITURAS];
            for (int i = 0; i < registos[s].total; i++) {
                temps_aux[i] = registos[s].leituras[i].temperatura;
            }

            double min, max, media, desvio;
            calcular_estatisticas(temps_aux, registos[s].total, &min, &max, &media, &desvio);
            printf("\n--- Sensor %s ---\n", registos[s].ficha->nome);
            printf("Número de valores lidos: %d\n", registos[s].total);
            printf("Temperatura mínima: %.2f\n", min);
            printf("Temperatura máxima: %.2f\n", max);
            printf("Temperatura média: %.2f\n", media);
            printf("Desvio-padrão: %.2f\n", desvio);
        }
    }

    return 0;
}