#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define F 440        // Fréquence du signal en Hz
#define A 20000      // Amplitude du signal
#define FE 44100     // Fréquence d'échantillonnage en Hz
#define DUREE 1      // Durée du signal en secondes

void write_wav_header(FILE *f, int sample_rate, int num_samples) {
    int byte_rate = sample_rate * sizeof(int16_t) * 1; // Mono, 16 bits
    int data_chunk_size = num_samples * sizeof(int16_t);

    // Écriture de l'en-tête du fichier WAV
    fwrite("RIFF", 1, 4, f);
    int32_t file_size = 36 + data_chunk_size;
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

    int16_t block_align = sizeof(int16_t) * num_channels;
    int16_t bits_per_sample = 16;
    fwrite(&block_align, 2, 1, f);
    fwrite(&bits_per_sample, 2, 1, f);

    fwrite("data", 1, 4, f);
    fwrite(&data_chunk_size, 4, 1, f);
}

int main() {
    int n = FE * DUREE;  // Nombre d'échantillons
    int16_t T[n];        // Tableau pour stocker les échantillons

    // Génération des échantillons
    for (int i = 0; i < n; i++) {
        double t = (double)i / FE;
        T[i] = (int16_t)(A * sin(2 * M_PI * F * t));
    }

    // Enregistrement dans un fichier WAV
    FILE *f = fopen("La.wav", "wb");
    if (f == NULL) {
        perror("Erreur à l'ouverture du fichier");
        return 1;
    }

    write_wav_header(f, FE, n);
    fwrite(T, sizeof(int16_t), n, f);

    fclose(f);
    return 0;
}
