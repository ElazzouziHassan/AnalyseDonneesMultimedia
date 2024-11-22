#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

int main() {
    // Paramètres de la gamme musicale (Do à Si)
    double frequences[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88}; // Fréquences des notes
    int nombre_notes = 7;                     // Nombre total de notes
    double amplitude = 20000.0;               // Amplitude des notes
    double duree_par_note = 1.0;              // Durée de chaque note en secondes
    double duree_totale = 7.0;                // Durée totale du signal (7 secondes)
    int frequence_echantillonnage = 44100;    // Fréquence d'échantillonnage en Hz
    int echantillons_par_note = (int)(frequence_echantillonnage * duree_par_note); // Échantillons pour chaque note
    int echantillons_totaux = (int)(frequence_echantillonnage * duree_totale);     // Nombre total d'échantillons

    // Allocation dynamique pour le tableau contenant les échantillons audio
    short *T = (short *)malloc(echantillons_totaux * sizeof(short));
    if (T == NULL) {
        printf("Erreur d'allocation de mémoire\n");
        return 1;
    }

    // Génération du signal carré approximé pour chaque note de la gamme
    for (int note = 0; note < nombre_notes; note++) {
        double frequence = frequences[note];
        for (int n = 0; n < echantillons_par_note; n++) {
            int indice = note * echantillons_par_note + n; // Indice global dans le tableau T

            // Application directe de la formule de Fourier pour générer un signal carré
            double signal_carre = 0.0;
            for (int k = 1; k <= 99; k += 2) { // Limité aux 5 premières harmoniques impaires (1, 3, 5, 7, 9)
                signal_carre += (1.0 / k) * sin(2 * M_PI * k * frequence * n / frequence_echantillonnage);
            }
 
            // Normalisation et multiplication par l'amplitude
            T[indice] = (short)(amplitude * signal_carre ); // Normalisation par M_PI pour garder la même amplitude
        }
    }

    // Création du fichier audio au format WAVE
    FILE *fichier = fopen("DoToSi_Carre_Fourier_Direct.wav", "wb");
    if (fichier == NULL) {
        printf("Erreur lors de la création du fichier\n");
        free(T);
        return 1;
    }

    // Écriture de l'entête WAVE
    uint32_t taille_chunk = 36 + echantillons_totaux * sizeof(short); // Taille totale du fichier moins 8 octets
    uint16_t format_audio = 1;            // PCM (non compressé)
    uint16_t nombre_canaux = 1;           // Mono
    uint16_t bits_par_echantillon = 16;   // 16 bits par échantillon
    uint32_t octets_par_seconde = frequence_echantillonnage * nombre_canaux * (bits_par_echantillon / 8);
    uint16_t alignement_bloc = nombre_canaux * (bits_par_echantillon / 8); // Octets par bloc d'échantillon
    uint32_t taille_donnees = echantillons_totaux * sizeof(short);       // Taille des données audio

    fwrite("RIFF", 1, 4, fichier);                     // Identifiant ChunkID : "RIFF"
    fwrite(&taille_chunk, 4, 1, fichier);              // ChunkSize
    fwrite("WAVE", 1, 4, fichier);                     // Format : "WAVE"
    fwrite("fmt ", 1, 4, fichier);                     // Identifiant Subchunk1ID : "fmt "
    uint32_t taille_subchunk1 = 16;                    // Taille du Subchunk1 (16 pour PCM)
    fwrite(&taille_subchunk1, 4, 1, fichier);
    fwrite(&format_audio, 2, 1, fichier);              // Format audio : 1 pour PCM
    fwrite(&nombre_canaux, 2, 1, fichier);             // Nombre de canaux : 1 (mono)
    fwrite(&frequence_echantillonnage, 4, 1, fichier); // Fréquence d'échantillonnage
    fwrite(&octets_par_seconde, 4, 1, fichier);        // ByteRate
    fwrite(&alignement_bloc, 2, 1, fichier);           // BlockAlign
    fwrite(&bits_par_echantillon, 2, 1, fichier);      // Bits par échantillon
    fwrite("data", 1, 4, fichier);                     // Identifiant Subchunk2ID : "data"
    fwrite(&taille_donnees, 4, 1, fichier);            // Taille des données audio (Subchunk2Size)
    fwrite(T, sizeof(short), echantillons_totaux, fichier); // Écriture des échantillons audio

    fclose(fichier);  // Fermeture du fichier
    free(T);          // Libération de la mémoire

    printf("Le fichier DoToSi_Carre_Fourier_Direct.wav a été généré avec succès\n");

    return 0;
}