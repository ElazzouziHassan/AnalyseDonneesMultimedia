#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define F 440
#define A 20000
#define FE 44100
#define DUREE 1 

int main() {
    int n = FE * DUREE;  
    int16_t T[n];   

    for (int i = 0; i < n; i++) {
        double t = (double)i / FE;
        T[i] = (int16_t)(A * sin(2 * M_PI * F * t));
    }

    for (int i = 0; i < n; i++) {
        printf("%d\n", T[i]);
    }

    return 0;
}

