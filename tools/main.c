/**
* @file main.c
* @author Nicolae/Daniele
* @mainpage Gioca al Domino
* @section intro_sec Introduzione
*
* Benvenuti sul nostro Domino. Potrete giocare cercanbdo di ottenere la miglior sequenza.
* Potreste provarci da soli, ma abbiamo anche una modalita' AI in vostro aiuto. Il domino
* potrete svilupparlo sia in orizzontale che in verticale.
*
*
* @section rules_sec Regole
* Regola 1: Partire da una tessera orizzontale.\n
* Regola 2: Le tessere verticali possono essere attaccate solo agli estremi del piano di gioco.\n
* Regola 3: Le tessere verticali possono essere attaccate solo sul lato basso di una adicente sempre verticale.\n
* Regola 4: Si puo' far crescere il domino solo in basso e non in alto.\n
* Regola 5: Sarete voi a decidere il numero di tessere speciali con cui giocare pero' almeno una deve essere normale.\n
* Regola 6: Tessera speciale 0|0 puo' essere attaccata ovunque.\n
* Regola 7: Tessera speciale 11|11 puo' essere attaccata ovunque e incrementa di uno tutte le tessere tranne i 6 che diventano 1.\n
* Regola 8: Tessera speciale 12|21 puo' essere attaccata ovunque e copia a specchio la tessera adiacente.\n
* Regola 9: Le tessere inserite in mezzo ad altre due devono essere adiacenti sia da destra che da sinistra\n
*
*
* @subsection modalita_1 Modalita' di gioco
* \li Modalita' 1: Interattiva
* \li Modalita' 2: AI
* @subsection modalita_2 Modalita' interattiva:
* Vi verra' generata una sequenza di tessere classiche + speciali e dovrete cercare
* di ottenere la miglior sequenza, ossia massimo punteggio. Se non ci saranno piu' adiacenze
* tra le tessere in mano e quelle sul campo da gioco, la partita finira'.
* @subsection modalita_3 Modalita' AI:
* Verra' generata una sequenza di tessere classiche + speciali e sara' il computer a trovare
* la miglior sequenza con esse.
*/

#include "ai.h"
#include "interactive.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int main() {
    srand(time(0)); // Random
    int numTotalTiles, numSpecials, choice = 0;
    do {
        printf("Con quante tessere vuoi giocare? (Almeno 2):");
        scanf("%d", &numTotalTiles);
        printf("Quante di queste vuoi che siano tessere speciali?");
        scanf("%d", &numSpecials);
        if (numTotalTiles < 2 || numSpecials < 0 || numSpecials >= numTotalTiles)
            printf("Numero di tessere non concesso! Inserisci numeri validi e ricorda che devi giocare con almeno 1 tessera normale e 2 totali\n");
    } while (numTotalTiles < 2 || numSpecials < 0 || numSpecials >= numTotalTiles); // Continua a chiedere fino a quando il numero di tessere normali non e' almeno 1

    // Inizio gioco, genera le tessere normali e quelle speciali
    dominoTile dominoTiles[numTotalTiles];
    generateTiles(numTotalTiles, numSpecials, dominoTiles);

    printTiles(numTotalTiles, dominoTiles);

    // Chiedi modalita' di gioco
    printf("\n--> Scegli la modalita' in cui giocare: \n");
    printf("1) Modalita' interattiva -> sei tu a scegliere come posizionare le tessere date \n");
    printf("2) Modalita' AI -> la nostra AI ti suggerisce la miglior strategia con le tessere date");
    do {
        printf("\n--> Scelta: ");
        scanf("%d", &choice);
    } while (choice != 1 && choice != 2);

    switch (choice) {
        case 1:
            printf("--- Hai scelto la modalita' interattiva! ---\n");
            playInteractive(numTotalTiles, dominoTiles);
            break;
        case 2:
            printf("--- Hai scelto la modalita' AI! ---\n");
            // Chiama la funzione AI che genera e stampa la miglior sequenza
            playAI(dominoTiles, numTotalTiles);
            break;
    }
    return 0;
}
