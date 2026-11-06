#include <stdio.h>
#include <math.h>

#define MAX_LEITURAS 5

double converter_sensor_para_temperatura(int valor_sensor);
int verificar_valor_sensor(int valor_sensor);
int ler_sequencia_valores(int sensores[], double temperaturas[], int limite_maximo);
void calcular_estatisticas(double temperaturas[], int total_lidos, double *min, double *max, double *media, double *desvio_padrao);
void apresentar_resultados(int sensores[], double temperaturas[], int total_lidos, double min, double max, double media, double desvio_padrao);
void executar_testes_unitarios();

int main(void) {
    int valores_sensores[MAX_LEITURAS];
    double temperaturas[MAX_LEITURAS];
    int total_lidos;

    double min = 0.0, max = 0.0, media = 0.0, desvio_padrao = 0.0;

    total_lidos = ler_sequencia_valores(valores_sensores, temperaturas, MAX_LEITURAS);

    if (total_lidos == 0) {
        printf("\nNenhum valor foi registado.\n");
        return 0;
    }

    calcular_estatisticas(temperaturas, total_lidos, &min, &max, &media, &desvio_padrao);
    apresentar_resultados(valores_sensores, temperaturas, total_lidos, min, max, media, desvio_padrao);

    return 0;
}

double converter_sensor_para_temperatura(int valor_sensor) {
    return (valor_sensor * 260.0) / 1023.0 - 20;
}

int verificar_temperatura_valida(double temperatura) {
    if (temperatura >= -10.0 && temperatura <= 190.0) {
        return 1; 
    }
    return 0;
}

int ler_sequencia_valores(int sensores[], double temperaturas[], int limite_maximo) {
    int total_lidos = 0;
    int valor;

    while (total_lidos < limite_maximo) {
        printf("\nIntroduza o valor do sensor (0 a 1023) ou -1 para sair: ");

        if (scanf("%d", &valor) != 1) {
            printf("Valor inválido. Por favor, introduza um número inteiro entre 0 e 1023.\n");
            while (getchar() != '\n');
        }

        if (valor == -1) {
            break;
        }

        if (valor >= 0 && valor <= 1023) {
            double temp_convertida = converter_sensor_para_temperatura(valor);
            
            if (verificar_temperatura_valida(temp_convertida)) {
                sensores[total_lidos] = valor;
                temperaturas[total_lidos] = temp_convertida;
                total_lidos++;
            } else {
                printf("Valor fora da gama!\n");
            }
        } else {
            printf("Valor fora da gama!\n");
        }
    }
    return total_lidos;

}

void calcular_estatisticas(double *temperaturas, int total_lidos, double *min, double *max, double *media, double *desvio_padrao) {
    if (total_lidos == 0) return;

    *min = *temperaturas;
    *max = *temperaturas;
    double soma = 0.0;

    for (int i = 0; i < total_lidos; i++) {
        double val = *(temperaturas + i);
        if (val < *min) *min = val;
        if (val > *max) *max = val;
        soma += val;
    }

    *media = soma / total_lidos;

    double soma_desvios = 0.0;
    for (int i = 0; i < total_lidos; i++) {
        soma_desvios += pow(*(temperaturas + i) - *media, 2);
    }

    if (total_lidos > 0) {
        *desvio_padrao = sqrt(soma_desvios / (total_lidos));
    } else {
        *desvio_padrao = 0.0;
    }
}

void apresentar_resultados(int *sensores, double *temperaturas, int total_lidos, double min, double max, double media, double desvio_padrao) {
    printf("\nValores lidos (sensor -> temperatura):\n");
    for (int i = 0; i < total_lidos; i++) {
        printf("%d -> %.2f\n", *(sensores + i), *(temperaturas + i));
    }

    printf("\nNumero de valores lidos: %d\n", total_lidos);
    printf("Mínimo: %.2f\n", min);
    printf("Máximo: %.2f\n", max);
    printf("Média: %.2f\n", media);
    printf("Desvio Padrão: %.2f\n", desvio_padrao);
}
