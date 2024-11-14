#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

int main() {
    int F = 440;           
    int A = 20000;     
    int Fe = 44100;       
    int D = 1;            

    int N = Fe * D;

    int16_t *T = (int16_t*)malloc(N * sizeof(int16_t));

    if (T == NULL) {
        printf("Erreur d'allocation mémoire\n");
        return 1;
    }

    // Génération du signal sinusoïdal
    for (int n = 0; n < N; n++) {
        T[n] = (int16_t)(A * sin(2 * PI * F * n / Fe));
    }

    // Libération de la mémoire
    free(T);

    printf("Signal généré avec succès\n");

    return 0;
}
