/**
 * @file ai.c
 * @author Nicolae Boldisor
 * @brief Libreria contenente lo sviluppo delle funzioni per modalita' AI
 * @date 06/02/2024
 */

// Includes
#include "ai.h"

/**
 * Funzione principale che gestisce la modalità AI: in particolare inizializza la funzione ricorsiva
 * e alla fine stampa il suo risultato.
 * @param tilesOnHand array di tessere generate
 * @param numTilesOnHand numero di tessere generate
 */
void playAI(dominoTile tilesOnHand[], int numTilesOnHand) {
    // La modalità AI ha bisogno di avere le tessere speciali in un array separato
    // così da poter verificarle dopo ogni tessera normale inserita.

    /**
     * Numero di speciali che verrà aggiornato dalla funzione che le divide dalle normali,
     * visto che la funzione non può ritornare 2 valori
     */
    int numSpecials = 0;

    /** Array di tessere speciali */
    //dominoTile *specialsOnHand = get_specials_array(tilesOnHand, numTilesOnHand, &numSpecials);

    /** Situzione di gioco iniziale(vuota a questo punto) */
    gameSituation defaultGame = {
            .inGameDominoTiles = createInGameDominoTiles(1, 0),
            .rows = 1,
            .cols = 0,
            .score = 0
    };

    /** Risultato AI calcolato dalla funzione ricorsiva */
    gameSituation *bestGame = getBestGameAI(tilesOnHand, numTilesOnHand, &defaultGame);

    // Dato il risultato stampa il gioco finale
    printAI(bestGame);

    // Libera la matrice allocata dinamicamente per situazione di gioco iniziale
    freeInGameDominoTiles(defaultGame.inGameDominoTiles, defaultGame.rows);

    // Libera il risultato allocato in mem dinamica
    freeGameSituation(bestGame);
}

/**
 * Funzione ricorsiva che prova tutte le combinazioni per trovare la migliore situazione di gioco.
 * @param tilesOnHand tessere generate
 * @param numTilesOnHand numero di tessere generate
 * @param defaultGame situazione di gioco fino a questo momento
 * @returns miglior situazione di gioco con le tessere date inserite nella situazione di gioco data
 */
gameSituation *getBestGameAI(dominoTile *tilesOnHand,
                             int numTilesOnHand,
                             gameSituation *defaultGame){
    // CASO BASE 1: sono finite le tessere speciali e !endGame
    // => verifica le tessere normali se sono finite, se non ci sono più adiacenze con queste
    // oppure se si è a inizio gioco e si sta cercando di mettere una speciale
    if(endGame(numTilesOnHand, tilesOnHand, *defaultGame))
        return defaultGame;

    /** Miglior situazione di gioco */
    gameSituation *best = NULL;
    /**
     * Copia array speciali che verrà modificato spostandole verso sinistra per provarle tutte.
     * Lo crea statico e verrà passato direttamente a una funzione per la modifica, per evitare ulteriori malloc.
     */
    dominoTile copyTilesOnHand[numTilesOnHand];
    copyDominoTileAr(tilesOnHand, copyTilesOnHand, numTilesOnHand);
    for (int i = 0; i < numTilesOnHand; ++i) {
        // Controlla se la tessera non è già stata controllata precedentemente(=> non c'è prima nell'array originale)
        if(!hasThisTileIn(*copyTilesOnHand, tilesOnHand, i)){
            gameSituation *actualBestIn;
            /** Tessera da inserire speciale o no */
            bool isSpecial = copyTilesOnHand->left == 0 || copyTilesOnHand->left == 11 || copyTilesOnHand->left == 12;
            // Se sono all'inizio della partita metto una tessera normale in orizzontale come prima tessera in gioco
            if(defaultGame->score == 0 && !isSpecial){
                gameSituation *in = copyGame(*defaultGame, HORIZONTAL, 0);

                dominoTileSide left = {.value = copyTilesOnHand->left, .side = LEFT, .orientation = HORIZONTAL};
                in->inGameDominoTiles[0][0] = left;
                dominoTileSide right = {.value = copyTilesOnHand->right, .side = RIGHT, .orientation = HORIZONTAL};
                in->inGameDominoTiles[0][1] = right;
                in->score = copyTilesOnHand->left + copyTilesOnHand->right;

                // Continua la ricorsione sulle prossime tessere meno questa
                actualBestIn = getBestGameAI(copyTilesOnHand+1, numTilesOnHand-1, in);
                if (actualBestIn != in) freeGameSituation(in);
            }else{
                // Controlla la tessera attuale in tutte le posizioni e poi sfrutta la ricorsione successivamente
                actualBestIn = bestGameTileIn(copyTilesOnHand, numTilesOnHand, *defaultGame);
            }

            if(actualBestIn && (!best || best->score < actualBestIn->score)){
                if(best) freeGameSituation(best);
                best = actualBestIn;
            }else{
                freeGameSituation(actualBestIn);
            }
        }
        move_left(copyTilesOnHand, numTilesOnHand, *copyTilesOnHand);
    }

    return best;
}

/**
 * Ritorna la situazione di gioco migliore con la tessera attuale dentro nella sua posizione migliore controllandole
 * tutte e sfruttando la funzione ricorsiva.
 * @param tilesOnHand puntatore alla prima tessera generata, dopo la quale sequenzialmente ci sono le altre
 * @param numTilesOnHand numero di tessere generate
 * @param defaultGame situazione di gioco fino a questo momento
 * @returns situazione di gioco migliore con l'attuale tessera inserita nella sua posizione migliore nella situazione di gioco data
 */
gameSituation *bestGameTileIn(dominoTile *tilesOnHand,
                              int numTilesOnHand,
                              gameSituation defaultGame) {

    /** Puntatore alla migliore situazione di gioco */
    gameSituation *bestIn = NULL;

    // Per ogni lato delle tessere in gioco(=> ogni lato è in una cella della matrice)
    // devo controllare l'adiacenza con la tesera passata
    for (int row = 0; row < defaultGame.rows; ++row) {
        for (int cell = 0; cell < defaultGame.cols; ++cell) {
            // Caso 1: la tessera in gioco adiacente è in orizzontale, l'attuale tessera può essere
            // attacata o a destra(H e V) o a sinistra(H e V) e questo dipende che lato è la tessera in gioco
            // Controlla che la mossa sia valida anche in caso di reversed

            // TESSERA COLLEGATA A SINISTRA
            gameSituation *in_H_L =  bestTileInEachPosition(tilesOnHand, numTilesOnHand, defaultGame, HORIZONTAL, LEFT, row, cell);
            if (in_H_L){
                if (!bestIn || bestIn->score < in_H_L->score) {
                    if(bestIn) freeGameSituation(bestIn);
                    bestIn = in_H_L;
                } else {
                    freeGameSituation(in_H_L);
                }
            }

            // TESSERA COLLEGATA A SINISTRA VERTICALE
            gameSituation *in_V_L =  bestTileInEachPosition(tilesOnHand, numTilesOnHand, defaultGame, VERTICAL, LEFT, row, cell);
            if (in_V_L){
                if (!bestIn || bestIn->score < in_V_L->score) {
                    if(bestIn) freeGameSituation(bestIn);
                    bestIn = in_V_L;
                } else {
                    freeGameSituation(in_V_L);
                }
            }

            // TESSERA COLLEGATA A DESTRA
            gameSituation *in_H_R =  bestTileInEachPosition(tilesOnHand, numTilesOnHand, defaultGame, HORIZONTAL, RIGHT, row, cell);
            if (in_H_R){
                if (!bestIn || bestIn->score < in_H_R->score) {
                    if(bestIn) freeGameSituation(bestIn);
                    bestIn = in_H_R;
                } else {
                    freeGameSituation(in_H_R);
                }
            }

            // TESSERA COLLEGATA A DESTRA VERTICALE
            gameSituation *in_V_R =  bestTileInEachPosition(tilesOnHand, numTilesOnHand, defaultGame, VERTICAL, RIGHT, row, cell);
            if (in_V_R){
                if (!bestIn || bestIn->score < in_V_R->score) {
                    if(bestIn) freeGameSituation(bestIn);
                    bestIn = in_V_R;
                } else {
                    freeGameSituation(in_V_R);
                }
            }
        }
    }
    return bestIn;
}

/**
 * Restituisce la migliore situazione di gioco(sfruttando la funzione ricorsiva) con la tessera passata dopo
 * averla aggiunta nella posizione specificata dai parametri
 * @param tilesOnHand puntatore alla prima tessera generata, dopo la quale sequenzialmente ci sono le altre
 * @param numTilesOnHand numero di tessere generate
 * @param defaultGame situazione di gioco fino a questo momento
 * @param tileToPutOrientation orintamento della tessera da mettere
 * @param isLinkedTo da che lato dovrebbe essere collegata la tessera passata a quella in gioco
 * @param rowAdjacent riga tessera in gioco alla quale quella da aggiugere dovrebbe essere adiacente
 * @param cellAdjacent colonna tessera in gioco alla quale quella da aggiugere dovrebbe essere adiacente
 * @returns NULL se mossa non valida oppure puntatore alla situazione di gioco migliore allocata in memoria dinamica
 */
gameSituation *bestTileInEachPosition(dominoTile *tilesOnHand,
                                      int numTilesOnHand,
                                      gameSituation defaultGame,
                                      char tileToPutOrientation,
                                      char isLinkedTo,
                                      int rowAdjacent,
                                      int cellAdjacent){
    /** Puntatore alla migliore situazione di gioco con la tessera attuale dentro nella posizione passata */
    gameSituation *bestIn = NULL;
    /** Tessera da inserire nel gioco */
    dominoTile tileToAdd = *tilesOnHand;

    /** Tessera da inserire speciale o no */
   bool isSpecial = tilesOnHand->left == 0 || tilesOnHand->left == 11 || tilesOnHand->left == 12;

    /** Salva se la mossa con la tessera attuale è valida */
    bool validAdjacency = isValidMove(tileToAdd, defaultGame, rowAdjacent, cellAdjacent, tileToPutOrientation, isLinkedTo);
    // Se non è adiacente la tessera di default verifica anche reversed
    if(!validAdjacency){
        rotateDominoTile(&tileToAdd);
        validAdjacency = isValidMove(tileToAdd, defaultGame, rowAdjacent, cellAdjacent, tileToPutOrientation, isLinkedTo);
    }
    // Se tessera normale o girata ha adiacenze
    if (validAdjacency){
        /** Copia della situazione di gioco in cui inserire la tessera orizzontale */
        gameSituation *tileInGame = copyGame(defaultGame, tileToPutOrientation, cellAdjacent);
        if (isSpecial)
            fixSpecialTiles(&tileToAdd, tileInGame, rowAdjacent, cellAdjacent, tileToPutOrientation, isLinkedTo);
        addDominoTile(tileInGame, tileToAdd, rowAdjacent, cellAdjacent, tileToPutOrientation, isLinkedTo);
        bestIn = getBestGameAI(tilesOnHand+1, numTilesOnHand-1, tileInGame);
        // Se non è la stessa situazione di gioco precedente perchè la ricorsione è finita
        if (bestIn != tileInGame) freeGameSituation(tileInGame);
    }
    return bestIn;
}

/**
 * Esegue la free di una situazione di gioco allocata in memoria dinamica
 * @param game situazione di gioco allocata dinamicamente
 */
void freeGameSituation(gameSituation *game){
    if(game){
        freeInGameDominoTiles(game->inGameDominoTiles, game->rows);
        free(game);
    }
}

/**
 * Aggiunge al gioco la tessera in base alla posizione in cui aggiungere e il suo orientamento
 * facendo lo split tra i due lati.
 * @param game situazione di gioco da modificare aggiungendo la tessera
 * @param tileToPut tessera da aggiungere
 * @param rowTileInGame riga della tessera alla quale collegare quella in mano
 * @param colTileInGame colonna della tessera alla quale collegare quella in mano
 * @param orientationTileToPut orientamento della tessera da mettere
 * @param isLinkedTo da che parte si collega la tessera a quella in gioco, LEFT or RIGHT
 */
void addDominoTile(gameSituation *game, dominoTile tileToPut, int rowTileInGame, int colTileInGame, char orientationTileToPut, char isLinkedTo){
    // Se la tessera adiacente in gioco è sulla colonna 0, bisogna spostare tutto a destra di 1 oppure 2 colonne in base
    // ad orientamento della tessera in gioco.
    if(colTileInGame == 0 && orientationTileToPut == HORIZONTAL){
       moveRight(game, 2);
       // Aggiunta della tessera
       game->inGameDominoTiles[rowTileInGame][0].value = tileToPut.left;
       game->inGameDominoTiles[rowTileInGame][0].orientation = HORIZONTAL;
       game->inGameDominoTiles[rowTileInGame][0].side = LEFT;

       game->inGameDominoTiles[rowTileInGame][1].value = tileToPut.right;
       game->inGameDominoTiles[rowTileInGame][1].orientation = HORIZONTAL;
       game->inGameDominoTiles[rowTileInGame][1].side = RIGHT;

    }else if(isLinkedTo == LEFT && ((colTileInGame == 0 && orientationTileToPut == VERTICAL) || (colTileInGame == 1 && orientationTileToPut == HORIZONTAL))){
        // Sposta solo di 1
        moveRight(game, 1);
        if(colTileInGame == 0){ // Caso in cui la tessera è verticale e si è sul bordo
            game->inGameDominoTiles[rowTileInGame][0].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][0].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame][0].side = TOP;

            game->inGameDominoTiles[rowTileInGame+1][0].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame+1][0].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame+1][0].side = BOTTOM;
        }else{ // Caso in cui la tessera è in orizzontale e prima del bordo
            game->inGameDominoTiles[rowTileInGame][0].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][0].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][0].side = LEFT;

            game->inGameDominoTiles[rowTileInGame][1].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame][1].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][1].side = RIGHT;
        }
    }else{
        // Aggiunta della tessera in base al lato al quale è collegata
        if(isLinkedTo == LEFT && orientationTileToPut == HORIZONTAL){
            game->inGameDominoTiles[rowTileInGame][colTileInGame-2].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][colTileInGame-2].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame-2].side = LEFT;

            game->inGameDominoTiles[rowTileInGame][colTileInGame-1].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame][colTileInGame-1].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame-1].side = RIGHT;
        } else if(isLinkedTo == RIGHT && orientationTileToPut == HORIZONTAL){
            game->inGameDominoTiles[rowTileInGame][colTileInGame+1].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][colTileInGame+1].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame+1].side = LEFT;

            game->inGameDominoTiles[rowTileInGame][colTileInGame+2].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame][colTileInGame+2].orientation = HORIZONTAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame+2].side = RIGHT;
        } else if(isLinkedTo == LEFT && orientationTileToPut == VERTICAL){
            game->inGameDominoTiles[rowTileInGame][colTileInGame-1].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][colTileInGame-1].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame-1].side = TOP;

            game->inGameDominoTiles[rowTileInGame+1][colTileInGame-1].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame+1][colTileInGame-1].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame+1][colTileInGame-1].side = BOTTOM;
        } else if(isLinkedTo == RIGHT && orientationTileToPut == VERTICAL){
            game->inGameDominoTiles[rowTileInGame][colTileInGame+1].value = tileToPut.left;
            game->inGameDominoTiles[rowTileInGame][colTileInGame+1].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame][colTileInGame+1].side = TOP;

            game->inGameDominoTiles[rowTileInGame+1][colTileInGame+1].value = tileToPut.right;
            game->inGameDominoTiles[rowTileInGame+1][colTileInGame+1].orientation = VERTICAL;
            game->inGameDominoTiles[rowTileInGame+1][colTileInGame+1].side = BOTTOM;
        }
    }

    // Aggiorna punteggio
    game->score += tileToPut.left + tileToPut.right;
}

/**
 * Copia un array di dominoTile in un altro passato alla funzione, evitando di fare malloc per la copia.
 * @param from array da cui copiare
 * @param to array in cui copiare
 * @param size lunghezza array
 */
void copyDominoTileAr(dominoTile from[], dominoTile *to, int size){
    for (int i = 0; i < size; ++i) {
       to[i] = from[i];
    }
}


/**
 * Copia una situazione di gioco in un altra situazione di gioco già data.
 * E aggiorna la copia in base alla tessera da inserire
 * @param defaultGame situazione di gioco originale
 * @param toAddOrientation orientamento della tessera da inserire in base al quale aggiornare righe o colonne
 * @param toAddCell colonna in cui aggiungere la nuova tessera per capire se aumentare o no righe e colonne
 */
gameSituation *copyGame(gameSituation defaultGame, char toAddOrientation, int toAddCell) {
    /** Numero di righe per la copia della situazione di gioco, aggiornato sulla posizione della tessera da inserire */
    int newRows = defaultGame.rows;
    /** Numero di colonne per la copia della situazione di gioco, aggiornato sulla posizione della tessera da inserire */
    int newCols = defaultGame.cols;
    /** Se tessera deve essere inserita ai bordi */
    bool borders = toAddCell == 0 || toAddCell == defaultGame.cols - 1;
    /** Se tessera deve essere inserita una cella prima dei bordi */
    bool beforeBorders = toAddCell == 1 || toAddCell == defaultGame.cols - 2;
    //  Se tessera deve essere inserita ai bordi aggiorna il numero di colonne e righe in base alla posizione
    if (toAddOrientation == HORIZONTAL && borders) {
        newCols += 2;
    }else if (toAddOrientation == HORIZONTAL && beforeBorders) {
        newCols += 1;
    }else if (toAddOrientation == VERTICAL && borders) {
        newRows += 1;
        newCols += 1;
    }

    /** Copia dell'array di array dinamico contenente i lati delle tessere in gioco */
    dominoTileSide **copyInDominoTileSides = copyMatrixSides(defaultGame, newRows,newCols);
    /** Copia della situazione di gioco in memoria dinamica */
    gameSituation *copy = (gameSituation*) malloc(sizeof(gameSituation));
    copy->inGameDominoTiles = copyInDominoTileSides;
    copy->rows = newRows;
    copy->cols = newCols;
    copy->score = defaultGame.score;

    return copy;
}

/**
 * Copia la matrice contenente i lati delle tessere in gioco.
 * @param originalGame situazione di gioco da cui copiare la matrice
 * @param rowsNewMat righe della nuova matrice
 * @param colsNewMat colonne della nuova matrice
 */
dominoTileSide **copyMatrixSides(gameSituation originalGame, int rowsNewMat, int colsNewMat) {
    /** Array di array dinamico in cui viene copiato quello originale */
    dominoTileSide **copy = (dominoTileSide **) malloc((rowsNewMat) * sizeof(dominoTileSide*));
    for (int i = 0; i < rowsNewMat; ++i) {
        copy[i] = (dominoTileSide *) malloc((colsNewMat) * sizeof(dominoTileSide));
        for (int j = 0; j < colsNewMat; ++j) {
            // Se la matrice originale è più piccola, nei nuovi posti creati si mette NOT_VALID_INT
            if (i >= originalGame.rows || j >= originalGame.cols) {
                copy[i][j].value = NOT_VALID_INT;
                copy[i][j].orientation = NOT_VALID_CHAR;
                copy[i][j].side = NOT_VALID_CHAR;
            } else {
                copy[i][j] = originalGame.inGameDominoTiles[i][j];
            }
        }
    }
    return copy;
}

/**
 * Verifica che su un dato array non ci sia la tessera inserita
 * @param toFound tessera da cercare
 * @param ar array in cui cercare
 * @param toCheckUntil fino a dove cercare
 * @returns se la tessera è presente
 */
bool hasThisTileIn(dominoTile toFound, dominoTile *ar, int toCheckUntil){
    for (int i = 0; i < toCheckUntil; ++i) {
        if(ar[i].left == toFound.left && ar[i].right == toFound.right)
            return true;
    }
    return false;
}

/**
 * Controlla se la tessera data(non speciale) ha adiacenze con le tessere della situazione di gioco data
 * @param tile tessera da verificare
 * @param game situazione di gioco
 * @returns se la tessera ha adiacenze
 */
bool hasTileAdjacency(dominoTile tile, gameSituation game){
    // Controllo se ci sono combinazioni disponibili
    for (int row = 0; row < game.rows; row++) {
        for (int cell = 0; cell < game.cols; cell++) {
            if (game.inGameDominoTiles[row][cell].value != NOT_VALID_INT) { // Se c'è una tessera in gioco valida
                if (isValidMove(tile, game, row, cell, HORIZONTAL, LEFT)
                    || isValidMove(tile, game, row, cell, HORIZONTAL, RIGHT)
                    || isValidMove(tile, game, row, cell, VERTICAL, LEFT)) {
                        return true;
                }
                dominoTile tileReversed;
                tileReversed.left=tile.right;
                tileReversed.right=tile.left;
                if (isValidMove(tileReversed, game, row, cell, HORIZONTAL, LEFT)
                    || isValidMove(tileReversed, game, row, cell, HORIZONTAL, RIGHT)
                    || isValidMove(tileReversed, game, row, cell, VERTICAL, LEFT)) {
                        return true;
                }
            }
        }
    }
    return false;
}

/**
 * Sposta l'array a sinistra e aggiunge alla fine la tessera data
 * (la prima tessera viene sovrascritta)
 * @param ar array di tessere da spostare
 * @param numTiles numero di tessere dell'array
 * @param to_add tessera da aggiungere
 * */
void move_left(dominoTile *ar, int numTiles, dominoTile to_add){
    for (int i = 0; i < numTiles-1; ++i) {
        ar[i] = ar[i+1];
    }
    ar[numTiles-1] = to_add;
}


/**
 * Stampa il risultato della AI, ossia la migliore partita finita date le tessere generate.
 * @param game miglior situazione di gioco trovata dall'AI
 */
void printAI(gameSituation *game) {
    for (int row = 0; row < game->rows; ++row) {
        for (int cell = 0; cell < game->cols; ++cell) {
            if (game->inGameDominoTiles[row][cell].value == NOT_VALID_INT) {
                printf("   ");
            } else {
                if (game->inGameDominoTiles[row][cell].orientation == HORIZONTAL && game->inGameDominoTiles[row][cell].side == LEFT) {
                    printf("[%d|", game->inGameDominoTiles[row][cell].value);
                } else if (game->inGameDominoTiles[row][cell].orientation == HORIZONTAL &&
                           game->inGameDominoTiles[row][cell].side == RIGHT) {
                    printf("|%d]", game->inGameDominoTiles[row][cell].value);
                } else if (game->inGameDominoTiles[row][cell].orientation == VERTICAL &&
                           game->inGameDominoTiles[row][cell].side == TOP) {
                    printf("{%d:", game->inGameDominoTiles[row][cell].value);
                } else if (game->inGameDominoTiles[row][cell].orientation == VERTICAL &&
                           game->inGameDominoTiles[row][cell].side == BOTTOM) {
                    printf(":%d}", game->inGameDominoTiles[row][cell].value);
                }
            }
        }
        if (row != game->rows-1) (printf("\n"));
    }
    printf("\n");
}