#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define FE 44100       // Sampling frequency in Hz
#define DURATION 1     // Duration in seconds
#define A 20000        // Amplitude

// Formant frequencies for "break" (approximation)
typedef struct {
    double f1;
    double f2;
    double f3;
    double duration; // Duration of the phoneme in seconds
} Phoneme;

Phoneme phonemes[] = {
    {300, 1000, 2500, 0.2},  // /b/ (plosive)
    {350, 1200, 2700, 0.2},  // /r/
    {500, 2000, 2800, 0.4},  // /eɪ/
    {300, 1800, 2600, 0.2}   // /k/ (plosive)
};

// Function to generate a formant waveform
void generate_formant(double *buffer, int sample_rate, double duration, double amplitude, double f1, double f2, double f3) {
    int samples = (int)(duration * sample_rate);
    for (int i = 0; i < samples; i++) {
        double t = (double)i / sample_rate;
        buffer[i] = amplitude * (
            0.3 * sin(2 * M_PI * f1 * t) + // Weight each formant
            0.3 * sin(2 * M_PI * f2 * t) +
            0.4 * sin(2 * M_PI * f3 * t)
        );
    }
}

// Function to write the WAV header
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
    int total_samples = FE * DURATION;
    double *buffer = calloc(total_samples, sizeof(double));
    int16_t *final_buffer = calloc(total_samples, sizeof(int16_t));
    if (!buffer || !final_buffer) {
        perror("Memory allocation failed");
        free(buffer);
        free(final_buffer);
        return 1;
    }

    // Synthesize each phoneme
    int sample_offset = 0;
    for (int p = 0; p < sizeof(phonemes) / sizeof(phonemes[0]); p++) {
        Phoneme ph = phonemes[p];
        int samples = (int)(ph.duration * FE);
        generate_formant(&buffer[sample_offset], FE, ph.duration, A, ph.f1, ph.f2, ph.f3);
        sample_offset += samples;
    }

    // Normalize and convert to int16_t
    for (int i = 0; i < total_samples; i++) {
        if (i < sample_offset) {
            final_buffer[i] = (int16_t)(buffer[i] > A ? A : buffer[i] < -A ? -A : buffer[i]);
        } else {
            final_buffer[i] = 0;
        }
    }

    // Write to WAV file
    FILE *f = fopen("break.wav", "wb");
    if (f == NULL) {
        perror("File opening failed");
        free(buffer);
        free(final_buffer);
        return 1;
    }

    write_wav_header(f, FE, total_samples);
    fwrite(final_buffer, sizeof(int16_t), total_samples, f);

    fclose(f);
    free(buffer);
    free(final_buffer);

    printf("Audio file 'break.wav' generated successfully.\n");
    return 0;
}

