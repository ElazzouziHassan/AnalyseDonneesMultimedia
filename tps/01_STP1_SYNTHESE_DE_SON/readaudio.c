#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Définition de l'en-tête WAV
typedef struct {
    char chunkID[4];
    uint32_t chunkSize;
    char format[4];
    char subchunk1ID[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char subchunk2ID[4];
    uint32_t subchunk2Size;
} WAVHeader;

int main() {
    // Ouvrir le fichier WAV en lecture
    FILE *inputFile = fopen("bruit_.wav", "rb");
    if (!inputFile) {
        printf("Erreur : impossible d'ouvrir le fichier d'entrée.\n");
        return 1;
    }

    // Lire l'en-tête WAV
    WAVHeader header;
    if (fread(&header, sizeof(WAVHeader), 1, inputFile) != 1) {
        printf("Erreur lors de la lecture de l'en-tête WAV.\n");
        fclose(inputFile);
        return 1;
    }

    // Vérifier si le fichier est bien au format PCM
    if (header.audioFormat != 1 || header.bitsPerSample != 16) {
        printf("Seuls les fichiers WAV PCM 16 bits sont pris en charge.\n");
        fclose(inputFile);
        return 1;
    }

    // Allocation de mémoire pour les données audio
    int numSamples = header.subchunk2Size / (header.bitsPerSample / 8);
    short *audioData = (short *)malloc(numSamples * sizeof(short));
    if (!audioData) {
        printf("Erreur d'allocation mémoire pour les données audio.\n");
        fclose(inputFile);
        return 1;
    }

    // Lire les données audio
    if (fread(audioData, sizeof(short), numSamples, inputFile) != numSamples) {
        printf("Erreur lors de la lecture des données audio.\n");
        free(audioData);
        fclose(inputFile);
        return 1;
    }

    fclose(inputFile);

    // Écrire les données audio dans un fichier texte
    FILE *outputFile = fopen("output.txt", "w");
    if (!outputFile) {
        printf("Erreur : impossible de créer le fichier de sortie.\n");
        free(audioData);
        return 1;
    }

    for (int i = 0; i < numSamples; i++) {
        fprintf(outputFile, "%d\n", audioData[i]);
    }

    fclose(outputFile);
    free(audioData);

    printf("Les données audio ont été écrites dans 'output.txt'.\n");
    return 0;
}
