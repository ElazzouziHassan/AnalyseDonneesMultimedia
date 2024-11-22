#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define FE 44100      // Fréquence d'échantillonnage en Hz
#define F 440         // Fréquence du signal carré en Hz
#define A 20000       // Amplitude maximale du signal
#define DURATION 2    // Durée du signal en secondes

// Fonction pour écrire l'en-tête du fichier WAV
void write_wav_header(FILE *f, int sample_rate, int num_samples) {
    int byte_rate = sample_rate * sizeof(int16_t);
    int data_chunk_size = num_samples * sizeof(int16_t);
    int file_size = 36 + data_chunk_size;

    fwrite("RIFF", 1, 4, f);
    fwrite(&file_size, 4, 1, f);
    fwrite("WAVE", 1, 4, f);
    fwrite("fmt ", 1, 4, f);

    int32_t fmt_chunk_size = 16;
    fwrite(&fmt_chunk_size, 4, 1, f);

    int16_t audio_format = 1; // PCM
    int16_t num_channels = 1; // Mono
    fwrite(&audio_format, 2, 1, f);
    fwrite(&num_channels, 2, 1, f);
    fwrite(&sample_rate, 4, 1, f);
    fwrite(&byte_rate, 4, 1, f);

    int16_t block_align = sizeof(int16_t);
    int16_t bits_per_sample = 16;
    fwrite(&block_align, 2, 1, f);
    fwrite(&bits_per_sample, 2, 1, f);

    fwrite("data", 1, 4, f);
    fwrite(&data_chunk_size, 4, 1, f);
}

int main() {
    int num_samples = FE * DURATION;
    int16_t *signal = malloc(num_samples * sizeof(int16_t));
    if (!signal) {
        perror("Erreur d'allocation mémoire");
        return 1;
    }

    // Génération du signal carré
    for (int i = 0; i < num_samples; i++) {
        double t = (double)i / FE;
        signal[i] = (int16_t)(A * (sin(2 * M_PI * F * t) > 0 ? 1 : -1));
    }

    // Création du fichier WAV
    FILE *f = fopen("signalcarre.wav", "wb");
    if (f == NULL) {
        perror("Erreur à l'ouverture du fichier");
        free(signal);
        return 1;
    }

    write_wav_header(f, FE, num_samples);
    fwrite(signal, sizeof(int16_t), num_samples, f);

    fclose(f);
    free(signal);

    printf("Signal carré enregistré dans 'signalcarre.wav'.\n");
    return 0;
}
