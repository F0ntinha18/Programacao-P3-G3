#include <stdio.h>

int main() {
    int temp;

    while (1) {
        printf("Introduza o valor da temperatura: ");
        
        if (scanf("%d", &temp) != 1) {
           
            while (getchar() != '\n');
            continue;
        }

        if (temp == -1) {
            break;
        }

        if (temp >= 40 && temp <= 820) {
            float tempf = 260.0f * temp / 1023.0f - 20.0f;
            printf("%.2f\n", tempf);
        } 
        else {
        
            printf("Valor fora da gama\n");
        }
    }

    return 0;
}
