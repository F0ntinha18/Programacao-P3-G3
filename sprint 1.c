#include <stdio.h>
int main() {
    int temp;
    printf("Introduza o valor da temperatura: ");
    scanf("%d", &temp);
    float tempf = 260.0 * temp / 1023.0 - 20.0;
    printf("A temperatura final é: %.2f\n", tempf);

    return 0;
}