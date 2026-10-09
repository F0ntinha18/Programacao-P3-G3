#include <stdio.h>
int main(void) {
    int valor;
    double temp;

    printf("Insira um valor inteiro entre 0 e 1023:  ");
    while(scanf("%d", &valor)!= 1){
        printf("Entrada invalida! Insira um valor inteiro: ");
        while(getchar() != '\n');
    }

    
    temp = (260 * valor) / 1023.0 - 20.0;
if (temp < -10 || temp > 190) {
    printf("Valor fora da gama.\n");
    return 1;
}
    printf("Valor inserido: %d\n", valor);
    printf("Temperatura correspondente: %.2f\n", temp);
    
    return 0;
}