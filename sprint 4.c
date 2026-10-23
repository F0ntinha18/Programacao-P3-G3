#include <stdio.h>
#include <math.h> 

int main() {
    int temp;
    int max_leituras;

    printf("Introduza o número de amostras a ler: ");
    scanf("%d", &max_leituras);

    float temperaturas[max_leituras]; 
    int num_leituras = 0;

    while (num_leituras < max_leituras) {
        printf("Introduza o valor da temperatura (ou -1 para sair): ");
        
        if (scanf("%d", &temp) != 1) {
            while (getchar() != '\n');
            continue;
        }

        if (temp == -1) {
            break;
        }

        if (temp >= 40 && temp <= 820) {
            float tempf = 260.0f * temp / 1023.0f - 20.0f;
            printf("Registado: %.2f\n", tempf);
            
            temperaturas[num_leituras] = tempf;
            num_leituras++;
        } 
        else {
            printf("Valor fora da gama\n");
        }
    } 

    if (num_leituras > 0) {
        printf("\n--- Resumo dos Dados ---\n");
        
        printf("Valores registados: ");
        for (int i = 0; i < num_leituras; i++) {
            printf("%.2f ", temperaturas[i]);
        }
        printf("\n");

        float min = temperaturas[0];
        float max = temperaturas[0];
        float soma = 0.0f;

        for (int i = 0; i < num_leituras; i++) {
            if (temperaturas[i] < min) {
                min = temperaturas[i];
            }
            if (temperaturas[i] > max) {
                max = temperaturas[i];
            }
            soma += temperaturas[i];
        }

        float media = soma / num_leituras;

        float soma_diferencas_quadrado = 0.0f;
        for (int i = 0; i < num_leituras; i++) {
            soma_diferencas_quadrado += pow(temperaturas[i] - media, 2);
        }
        float desvio_padrao = sqrt(soma_diferencas_quadrado / num_leituras);

        printf("Número de valores lidos: %d\n", num_leituras);
        printf("Valor máximo: %.2f\n", max);
        printf("Valor mínimo: %.2f\n", min);
        printf("Média: %.2f\n", media);
        printf("Desvio-padrão: %.2f\n", desvio_padrao);

    } else {
        printf("\nNenhum valor válido foi registado.\n");
    }

    return 0;
}