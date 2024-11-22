#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14159265359

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

// Filtrage passe-bas simple (moyenne glissante)
void lowPassFilter(short *signal, int length, int windowSize) {
    short *filtered = (short *)malloc(length * sizeof(short));
    if (!filtered) {
        printf("Erreur d'allocation mémoire pour le filtre.\n");
        return;
    }

    for (int i = 0; i < length; i++) {
        int sum = 0;
        int count = 0;

        // Moyenne des échantillons voisins
        for (int j = -windowSize; j <= windowSize; j++) {
            if (i + j >= 0 && i + j < length) {
                sum += signal[i + j];
                count++;
            }
        }

        filtered[i] = sum / count;
    }

    // Copier le signal filtré dans l'original
    for (int i = 0; i < length; i++) {
        signal[i] = filtered[i];
    }

    free(filtered);
}

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

    // Appliquer le filtre passe-bas
    int windowSize = 5; // Taille de la fenêtre du filtre (modifiable selon le besoin)
    lowPassFilter(audioData, numSamples, windowSize);

    // Écrire le fichier WAV filtré
    FILE *outputFile = fopen("output_filtered.wav", "wb");
    if (!outputFile) {
        printf("Erreur : impossible de créer le fichier de sortie.\n");
        free(audioData);
        return 1;
    }

    // Écrire l'en-tête
    if (fwrite(&header, sizeof(WAVHeader), 1, outputFile) != 1) {
        printf("Erreur lors de l'écriture de l'en-tête WAV.\n");
        fclose(outputFile);
        free(audioData);
        return 1;
    }

    // Écrire les données audio filtrées
    if (fwrite(audioData, sizeof(short), numSamples, outputFile) != numSamples) {
        printf("Erreur lors de l'écriture des données audio.\n");
        fclose(outputFile);
        free(audioData);
        return 1;
    }

    fclose(outputFile);
    free(audioData);

    printf("Le fichier audio filtré a été enregistré sous 'clean_audio.wav'.\n");
    return 0;
}
