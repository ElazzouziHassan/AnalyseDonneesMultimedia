#include <stdio.h>

#define SIZE 64
#define WHITE 255
#define BLACK 0

int main() {
    unsigned char vertical[SIZE][SIZE];
    
    // Étape 1 : Créer des bandes verticales
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            vertical[y][x] = ((x/8) % 2 == 0) ? WHITE : BLACK;
        }
    }

      

    // Étape 4 : Sauvegarder le damier dans un fichier PGM
    FILE *file = fopen("vertical.raw", "wb");
    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", SIZE, SIZE); // Dimensions de l'image
    fprintf(file, "255\n");              // Niveau de gris maximal
   // Dimensions de l'image
                 // Niveau de gris maximal
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            fprintf(file, "%d ", vertical[x][y]);
        }
        fprintf(file, "\n");
    }
    fclose(file);

    printf("Image 'checkerboard.raw' générée avec succès !\n");
    return 0;
}
