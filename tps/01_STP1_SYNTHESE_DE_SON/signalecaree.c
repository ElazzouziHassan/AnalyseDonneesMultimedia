#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14159265359

// WAV header structure
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
    // Signal parameters
    double F = 440.0;      // Frequency in Hz
    int A = 20000;         // Amplitude
    int fe = 44100;        // Sampling frequency in Hz
    int duration = 3;      // Duration in seconds
    int N = fe * duration; // Total number of samples
    int NUM_HARMONICS = 9; // Number of odd harmonics to include

    // Memory allocation for the signal
    short *T = (short *)malloc(N * sizeof(short));
    if (T == NULL) {
        printf("Memory allocation error.\n");
        return 1;
    }

    // Generate the square wave using Fourier series
    for (int i = 0; i < N; i++) {
        double t = (double)i / fe;
        double value = 0.0;

        for (int n = 1; n <= NUM_HARMONICS; n += 2) {
            value += (4.0 / PI) * (1.0 / n) * sin(2 * PI * n * F * t);
        }

        T[i] = (short)(value * A);
    }

    // Build the WAV header
    WAVHeader header = {0};
    header.chunkID[0] = 'R'; header.chunkID[1] = 'I'; header.chunkID[2] = 'F'; header.chunkID[3] = 'F';
    header.format[0] = 'W'; header.format[1] = 'A'; header.format[2] = 'V'; header.format[3] = 'E';
    header.subchunk1ID[0] = 'f'; header.subchunk1ID[1] = 'm'; header.subchunk1ID[2] = 't'; header.subchunk1ID[3] = ' ';
    header.subchunk2ID[0] = 'd'; header.subchunk2ID[1] = 'a'; header.subchunk2ID[2] = 't'; header.subchunk2ID[3] = 'a';

    header.chunkSize = 36 + N * sizeof(short);
    header.subchunk1Size = 16;
    header.audioFormat = 1; // PCM
    header.numChannels = 1; // Mono
    header.sampleRate = fe;
    header.bitsPerSample = 16;
    header.byteRate = fe * header.numChannels * header.bitsPerSample / 8;
    header.blockAlign = header.numChannels * header.bitsPerSample / 8;
    header.subchunk2Size = N * sizeof(short);

    // Create and write to the WAV file
    FILE *file = fopen("SquareWave_Fourier.wav", "wb");
    if (file == NULL) {
        printf("Error creating file.\n");
        free(T);
        return 1;
    }

    if (fwrite(&header, sizeof(WAVHeader), 1, file) != 1) {
        printf("Error writing WAV header.\n");
        fclose(file);
        free(T);
        return 1;
    }

    if (fwrite(T, sizeof(short), N, file) != N) {
        printf("Error writing audio data.\n");
        fclose(file);
        free(T);
        return 1;
    }

    fclose(file);
    free(T);

    printf("The square wave (using Fourier series) has been saved in SquareWave_Fourier.wav\n");
    return 0;
}
