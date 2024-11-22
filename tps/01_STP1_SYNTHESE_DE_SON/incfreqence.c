#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>  // Pour memcpy

#define FREQ_ECHANTILLON 44100  // Fréquence d'échantillonnage (44.1 kHz)
#define DUREE 1                 // Durée du signal en secondes
#define FREQUENCE_BASE 261.625      // Fréquence de base de la sinusoïde
#define AMPLITUDE 20000         // Amplitude du signal

// Structure pour l'en-tête WAV
typedef struct {
    char riffTag[4];            // "RIFF"
    uint32_t fileSize;          // Taille totale du fichier - 8 octets
    char waveTag[4];            // "WAVE"
    char fmtTag[4];             // "fmt "
    uint32_t fmtChunkSize;      // Taille du sous-bloc "fmt" (16 pour PCM)
    uint16_t audioFormat;       // Format audio (1 pour PCM)
    uint16_t numChannels;       // Nombre de canaux (1 pour mono, 2 pour stéréo)
    uint32_t sampleRate;        // Fréquence d'échantillonnage
    uint32_t byteRate;          // Taux de transfert en octets (sampleRate * numChannels * bitsPerSample / 8)
    uint16_t blockAlign;        // Nombre d'octets par échantillon (numChannels * bitsPerSample / 8)
    uint16_t bitsPerSample;     // Nombre de bits par échantillon (8, 16, etc.)
    char dataTag[4];            // "data"
    uint32_t dataChunkSize;     // Taille des données audio (numChannels * numSamples * bitsPerSample / 8)
} WavHeader;

void initialiser_entete_wav(WavHeader *entete, int taille_donnees) {
    // Initialisation des champs de l'en-tête
    memcpy(entete->riffTag, "RIFF", 4);
    entete->fileSize = 36 + taille_donnees;
    memcpy(entete->waveTag, "WAVE", 4);
    memcpy(entete->fmtTag, "fmt ", 4);
    entete->fmtChunkSize = 16;
    entete->audioFormat = 1;            // PCM
    entete->numChannels = 1;            // Mono
    entete->sampleRate = FREQ_ECHANTILLON;
    entete->byteRate = FREQ_ECHANTILLON * entete->numChannels * (entete->bitsPerSample / 8);
    entete->blockAlign = entete->numChannels * (entete->bitsPerSample / 8);
    entete->bitsPerSample = 16;
    memcpy(entete->dataTag, "data", 4);
    entete->dataChunkSize = taille_donnees;
}

int main() {
    int taille = FREQ_ECHANTILLON * DUREE;  // Nombre d'échantillons pour la durée spécifiée
    int16_t *T = (int16_t *)malloc(taille * sizeof(int16_t));  // Allocation dynamique pour les échantillons

    if (T == NULL) {
        printf("Erreur d'allocation mémoire\n");
        return 1;
    }

    // Génération du signal combiné (superposition des signaux)
    for (int i = 0; i < taille; i++) {
        double t = (double)i / FREQ_ECHANTILLON;
        T[i] = (int16_t)(
            AMPLITUDE * 
			(sin(2.0 * M_PI * FREQUENCE_BASE * t) +
                0.5*sin(2.0 * M_PI * FREQUENCE_BASE * 2 * t) +   // Signal de fréquence 880 Hz
                0.5*sin(2.0 * M_PI * FREQUENCE_BASE * 3 * t) +   // Signal de fréquence 1320 Hz
                0.5*sin(2.0 * M_PI * FREQUENCE_BASE * 4 * t)     // Signal de fréquence 1760 Hz
            ) / 1.5                                         // Normalisation pour éviter la saturation
        );
    }

    // Création et initialisation de l'en-tête WAV
    WavHeader entete;
    initialiser_entete_wav(&entete, taille * sizeof(int16_t));

    // Ouverture du fichier WAV en mode binaire pour l'écriture
    FILE *file = fopen("Signalinc_frequence.wav", "wb");
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier\n");
        free(T);
        return 1;
    }

    // Écriture de l'en-tête WAV
    fwrite(&entete, sizeof(WavHeader), 1, file);

    // Écriture des données audio dans le fichier
    fwrite(T, sizeof(int16_t), taille, file);

    // Fermeture du fichier et libération de la mémoire
    fclose(file);
    free(T);

    printf("Fichier Signal_440Hz_Multiples.wav enregistré avec succès\n");
    return 0;
}
