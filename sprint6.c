#include <stdio.h>
#include <math.h>

#define MAX_LEITURAS 100
#define MAX_SENSORES 3
#define TAM_NOME 20


typedef struct {
    char nome[TAM_NOME];
    double ganho;
    double desvio;
} CalibracaoSensor;

// Tabela global com os dados de calibração dos sensores
const CalibracaoSensor TABELA_CALIBRACAO[MAX_SENSORES] = {
    {"TEMP1", 260.0, -20.0},
    {"TEMP2", 250.0, -15.0},
    {"TEMP3", 240.0, -10.0}
};


double converter_sensor(const int *valor, double ganho, double desvio) {
    return (ganho * (*valor)) / 1023.0 + desvio;
}


int verificar_valida(const double *temp) {
    if (*temp < -10.0 || *temp > 190.0) {
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
//sensor 1
    int instantes_1[MAX_LEITURAS];
    int valores_1[MAX_LEITURAS];
    double temps_1[MAX_LEITURAS];
    int total_1 = 0;
//sensor 2
    int instantes_2[MAX_LEITURAS];
    int valores_2[MAX_LEITURAS];
    double temps_2[MAX_LEITURAS];
    int total_2 = 0;
//sensor 3
    int instantes_3[MAX_LEITURAS];
    int valores_3[MAX_LEITURAS];
    double temps_3[MAX_LEITURAS];
    int total_3 = 0;

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

        int id_sensor = 0;
        double ganho = 0.0;
        double desvio = 0.0;

        if (nome_sensor[0] == TABELA_CALIBRACAO[0].nome[0] && 
            nome_sensor[4] == TABELA_CALIBRACAO[0].nome[4]) {
            id_sensor = 1;
            ganho = TABELA_CALIBRACAO[0].ganho;
            desvio = TABELA_CALIBRACAO[0].desvio;
        } else if (nome_sensor[0] == TABELA_CALIBRACAO[1].nome[0] && 
                   nome_sensor[4] == TABELA_CALIBRACAO[1].nome[4]) {
            id_sensor = 2;
            ganho = TABELA_CALIBRACAO[1].ganho;
            desvio = TABELA_CALIBRACAO[1].desvio;
        } else if (nome_sensor[0] == TABELA_CALIBRACAO[2].nome[0] && 
                   nome_sensor[4] == TABELA_CALIBRACAO[2].nome[4]) {
            id_sensor = 3;
            ganho = TABELA_CALIBRACAO[2].ganho;
            desvio = TABELA_CALIBRACAO[2].desvio;
        }

        if (id_sensor == 0) {
            printf("Sensor desconhecido\n");
            continue;
        }

        double temp_calculada = converter_sensor(&valor_bruto, ganho, desvio);

        if (!verificar_valida(&temp_calculada)) {
            printf("Valor fora da gama\n");
            continue;
        }
        printf("-> Temperatura calculada: %.2f C\n", temp_calculada);

        if (id_sensor == 1 && total_1 < MAX_LEITURAS) {
            *(instantes_1 + total_1) = instante;
            *(valores_1 + total_1) = valor_bruto;
            *(temps_1 + total_1) = temp_calculada;
            total_1++;
        } else if (id_sensor == 2 && total_2 < MAX_LEITURAS) {
            *(instantes_2 + total_2) = instante;
            *(valores_2 + total_2) = valor_bruto;
            *(temps_2 + total_2) = temp_calculada;
            total_2++;
        } else if (id_sensor == 3 && total_3 < MAX_LEITURAS) {
            *(instantes_3 + total_3) = instante;
            *(valores_3 + total_3) = valor_bruto;
            *(temps_3 + total_3) = temp_calculada;
            total_3++;
        }
    }


    printf("\n                RELATORIO FINAL                   \n");
    printf("Leituras registadas (sensor instante: valor -> temperatura):\n");
    
    for (int i = 0; i < total_1; i++) {
        printf("TEMP1 %d: %d -> %.2f\n", *(instantes_1 + i), *(valores_1 + i), *(temps_1 + i));
    }
    for (int i = 0; i < total_2; i++) {
        printf("TEMP2 %d: %d -> %.2f\n", *(instantes_2 + i), *(valores_2 + i), *(temps_2 + i));
    }
    for (int i = 0; i < total_3; i++) {
        printf("TEMP3 %d: %d -> %.2f\n", *(instantes_3 + i), *(valores_3 + i), *(temps_3 + i));
    }

    // Cálculo e apresentação de estatísticas por sensor
    if (total_1 > 0) {
        double min, max, media, desvio;
        calcular_estatisticas(temps_1, total_1, &min, &max, &media, &desvio);
        printf("\n--- Sensor TEMP1 ---\n");
        printf("Número de valores lidos: %d\n", total_1);
        printf("Temperatura mínima: %.2f\n", min);
        printf("Temperatura máxima: %.2f\n", max);
        printf("Temperatura média: %.2f\n", media);
        printf("Desvio-padrão: %.2f\n", desvio);
    }

    if (total_2 > 0) {
        double min, max, media, desvio;
        calcular_estatisticas(temps_2, total_2, &min, &max, &media, &desvio);
        printf("\n--- Sensor TEMP2 ---\n");
        printf("Número de valores lidos: %d\n", total_2);
        printf("Temperatura mínima: %.2f\n", min);
        printf("Temperatura máxima: %.2f\n", max);
        printf("Temperatura média: %.2f\n", media);
        printf("Desvio-padrão: %.2f\n", desvio);
    }

    if (total_3 > 0) {
        double min, max, media, desvio;
        calcular_estatisticas(temps_3, total_3, &min, &max, &media, &desvio);
        printf("\n--- Sensor TEMP3 ---\n");
        printf("Número de valores lidos: %d\n", total_3);
        printf("Temperatura mínima: %.2f\n", min);
        printf("Temperatura máxima: %.2f\n", max);
        printf("Temperatura média: %.2f\n", media);
        printf("Desvio-padrão: %.2f\n", desvio);
    }

    return 0;
}