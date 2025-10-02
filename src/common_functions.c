/**
 * @file common_functions.c
 * @author Nicolae/Daniele
 * @brief Libreria contenente funzioni comuni tra la modalità interattiva del domino e la
 * modalità AI.
 * @date 06/02/2024
 */

#include "common_functions.h"

/**
 * Genera casualmente le tessere normali e speciali.
 * @param numTotalTiles numero di tessere totali da generare
 * @param numSpecials numero di tessere speciali da generare
 * @param tilesOnHand array da riempire con le tessere generate
*/

void generateTiles(int numTotalTiles, int numSpecials, dominoTile tilesOnHand[]) {
    // Tessere normali
    for (int i = 0; i < numTotalTiles - numSpecials; i++) {
        tilesOnHand[i].left = rand() % 6 + 1;
        // Il numero sul lato destro deve essere più grande o uguale a quello sul lato sinistro
        tilesOnHand[i].right = rand() % (7 - tilesOnHand[i].left) + tilesOnHand[i].left;
    }

    // Tessere speciali
    for (int i = numTotalTiles - numSpecials; i < numTotalTiles; i++) {
        switch (rand() % 3) {
            case 0:
                tilesOnHand[i].left = tilesOnHand[i].right = 0;
                break;
            case 1:
                tilesOnHand[i].left = tilesOnHand[i].right = 11;
                break;
            case 2:
                tilesOnHand[i].left = 12;
                tilesOnHand[i].right = 21;
                break;
        }
    }
}

/**
 * Stampa le tessere generate
 * @param numTiles numero di tessere generate
 * @param tilesOnHand array con le tessere da stampare
*/
void printTiles(int numTiles, dominoTile tilesOnHand[]) {
    for (int i = 0; i < numTiles; i++) {
        printf("%d) [%d|%d]\n", i + 1, tilesOnHand[i].left, tilesOnHand[i].right);
    }
}

/**
 * Sposta di [posMoved] colonne le tessere e mette le celle delle prime [posMoved] colonne a -1 (NOT_VALID_INT)
 * @param game situazione di gioco le cui posizioni vanno spostate e le cui colonne sono state aggiornate
 * (a + posMoved) precedentemente, ma le ultime colonne sono vuote
 * @param posMoved numero di colonne da shiftare
*/
void moveRight(gameSituation *game, int posMoved) {
    for (int row = 0; row < game->rows; ++row) {
        /** Il nuovo indice, coincide con l'ultima colonna della matrice vuota */
        int newIndex = game->cols - 1;
        // La prima cella da spostare è l'ultima con un valore valido ossia l'ultima colonna prima
        // di aver aggiornato le colonne
        for (int cell = game->cols - posMoved - 1; cell >= 0; --cell) {
            game->inGameDominoTiles[row][newIndex] = game->inGameDominoTiles[row][cell];
            if (cell < posMoved) game->inGameDominoTiles[row][cell].value = NOT_VALID_INT;
            newIndex--;
        }
    }
}

/**
 * Crea un campo da gioco mettendo valori di default
 * @param rows righe del campo da creare
 * @param cols colonne del campo da creare
 * @return newGame campo da gioco creato
*/
dominoTileSide **createInGameDominoTiles(int rows, int cols) {
    dominoTileSide **newGame = (dominoTileSide **) malloc(sizeof(dominoTileSide*) * rows);
    for (int row = 0; row < rows; row++) {
        newGame[row] = (dominoTileSide *) malloc(sizeof(dominoTileSide) * cols);
        for (int cell = 0; cell < cols; cell++) {
            newGame[row][cell].value = NOT_VALID_INT;
            newGame[row][cell].side = NOT_VALID_CHAR;
            newGame[row][cell].orientation = NOT_VALID_CHAR;
        }
    }
    return newGame;
}

/**
 * Libera la memoria allocata nell'heap per la matrice
 * @param dominoTilesInGame matrice del campo da gioco
 * @param rows righe della matrice
*/
void freeInGameDominoTiles(dominoTileSide **dominoTilesInGame, int rows) {
    for (int row = 0; row < rows; row++) {
        free(dominoTilesInGame[row]);
    }
    free(dominoTilesInGame);
}

/**
* Scambia i due valori della tessera presa in considerazione
* @param tile tessera da ruotare
*/

void rotateDominoTile(dominoTile *tile) {
    int temp = tile->left;
    tile->left = tile->right;
    tile->right = temp;
}

/**
* Sistema il campo da gioco e la tessera presa in considerazione in base alle regole delle tessere speciali
* @param dominoTile tessera in mano
* @param game situazione di gioco da aggiornare
* @param row riga dove inserire tessera
* @param col colonna dove inserire tessera
* @param orientation orientamento della tessera presa dalla mano
* @param isLinkedTo da che parte si collega la tessera, LEFT or RIGHT
*/

void fixSpecialTiles(dominoTile *dominoTile, gameSituation *game, int row, int col, char orientation, char isLinkedTo) {
    switch (dominoTile->left) {
        case 11: // Caso tessera [11|11]
            for (int i = 0; i < game->rows; i++) {
                for (int j = 0; j < game->cols; j++) {
                    if (game->inGameDominoTiles[i][j].value != NOT_VALID_INT) { // Se la posizione del campo non è vuota
                        if (game->inGameDominoTiles[i][j].value == 6) {
                            game->inGameDominoTiles[i][j].value = 1; // Se la tessera contiene un 6, lo faccio diventare 1
                            game->score -= 5; // Diminuisco score di 5
                        } else if(game->inGameDominoTiles[i][j].value != 0) { // Lo zero non viene incrementato se no perde la sua abilità da speciale
                            game->inGameDominoTiles[i][j].value++;
                            game->score++;
                        }
                    }
                }
            }
            // Copio il valore della cella adiacente
            dominoTile->left = dominoTile->right = game->inGameDominoTiles[row][col].value;
            break;

        case 12: // Caso tessera [12|21]
        if (game->inGameDominoTiles[row][col].orientation==HORIZONTAL) {
            if (isLinkedTo==RIGHT) { // Se tessera in mano si collega a dx di quella sul tavolo
                dominoTile->left=game->inGameDominoTiles[row][col].value;
                dominoTile->right=game->inGameDominoTiles[row][col-1].value;
            }
            else if (orientation==HORIZONTAL) { // Se tessera in mano collegata a sx e orizzontale
                dominoTile->left=game->inGameDominoTiles[row][col+1].value;
                dominoTile->right=game->inGameDominoTiles[row][col].value;
            } else { // Se tessera in mano collegata a sx e verticale
                dominoTile->left=game->inGameDominoTiles[row][col].value;
                dominoTile->right=game->inGameDominoTiles[row][col+1].value;
            }
        } else { // Se cella in campo proviene da una tessera verticale
            if (orientation==HORIZONTAL) { // Se tessera in mano orizzontale
                if (isLinkedTo==LEFT) { // Se tessera in mano collegata a sx di quella in campo
                    dominoTile->left=(game->inGameDominoTiles[row][col].side==TOP) ? game->inGameDominoTiles[row+1][col].value : game->inGameDominoTiles[row-1][col].value;
                    dominoTile->right=game->inGameDominoTiles[row][col].value;
                } else { // Se tessera in mano collegata a dx di quella in campo
                    dominoTile->left=game->inGameDominoTiles[row][col].value;
                    dominoTile->right=(game->inGameDominoTiles[row][col].side==TOP) ? game->inGameDominoTiles[row+1][col].value : game->inGameDominoTiles[row-1][col].value;
                }
            } else { // Se tessera in mano verticale
                dominoTile->left=game->inGameDominoTiles[row][col].value;
                dominoTile->right=game->inGameDominoTiles[row-1][col].value;
            }
        }
        break;
    }
}

/**
 * Controlla in base alla tessera adiacente se mettendo una tessera dalla mano,
 * la mossa è valida.
 * @param tileToPut tessera in mano da mettere
 * @param game situazione di gioco attuale nel quale inserire la tessera
 * @param rowTileInGame riga della tessera alla quale collegare quella in mano
 * @param colTileInGame colonna della tessera alla quale collegare quella in mano
 * @param orientationTileToPut orientamento della tessera da mettere
 * @param isLinkedTo specifica da che parte è collegata la tessera in gioco a quella da mettere
 * @return valid se la mossa è valida
 */
bool isValidMove(dominoTile tileToPut, gameSituation game, int rowTileInGame, int colTileInGame, char orientationTileToPut, char isLinkedTo) {
    /** Lato della tessera in gioco al quale sto cercando di collegare */
    dominoTileSide adjacent = game.inGameDominoTiles[rowTileInGame][colTileInGame];
    // Controllo che la mossa implichi tessere speciali perchè queste possono essere collegate a prescindere
    bool isSpecial = adjacent.value == 0 || tileToPut.left == 0 || tileToPut.left == 11 || tileToPut.left == 12;
    if (orientationTileToPut == HORIZONTAL) { // Se sto mettendo in orizzontale
        if (isLinkedTo == LEFT) { // Se la sto mettendo alla sinistra dell'adiacente
            if ((isSpecial && game.inGameDominoTiles[rowTileInGame][colTileInGame].value != NOT_VALID_INT) || tileToPut.right == adjacent.value) { // Se c'è adiacenza
                if (colTileInGame == 0 // Bordo
                    || (colTileInGame == 1 && game.inGameDominoTiles[rowTileInGame][colTileInGame - 1].value == NOT_VALID_INT) // Se sono una cella prima del bordo
                    || (game.inGameDominoTiles[rowTileInGame][colTileInGame - 1].value == NOT_VALID_INT &&
                        game.inGameDominoTiles[rowTileInGame][colTileInGame - 2].value == NOT_VALID_INT)) {
                    return true;
                }
            }
        } else if (isLinkedTo == RIGHT) { // Se la sto mettendo alla destra dell'adiacente
            if ((isSpecial && game.inGameDominoTiles[rowTileInGame][colTileInGame].value != NOT_VALID_INT) || tileToPut.left == adjacent.value) { // Se c'è adiacenza
                if (colTileInGame == game.cols - 1 // Bordo
                    || (colTileInGame == game.cols - 2 &&
                        game.inGameDominoTiles[rowTileInGame][colTileInGame + 1].value == NOT_VALID_INT) // Se sono una cella prima del bordo
                    || (game.inGameDominoTiles[rowTileInGame][colTileInGame + 1].value == NOT_VALID_INT &&
                        game.inGameDominoTiles[rowTileInGame][colTileInGame + 2].value == NOT_VALID_INT)) {
                    return true;
                }
            }
        }
    } else if (orientationTileToPut == VERTICAL) { // Se la sto mettendo in verticale
        if ((isLinkedTo == LEFT && colTileInGame == 0) || (colTileInGame == game.cols - 1 && isLinkedTo == RIGHT)) { // Le verticali possono essere attaccate solo ai bordi
            if ((isSpecial && game.inGameDominoTiles[rowTileInGame][colTileInGame].value != NOT_VALID_INT) || tileToPut.left == adjacent.value) { // Se c'è adiacenza oppure speciale
                // Le verticali possono essere attaccate alle orizzontali oppure ad altre verticali sul lato sotto
                if (adjacent.orientation == HORIZONTAL ||
                    (adjacent.orientation == VERTICAL && adjacent.side == BOTTOM)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Controlla se si è arrivati alla fine della partita, ossia non ci sono più adiacenze
 * tra le tessere in gioco(no specials) e quelle in mano.
 * @param numTotalTiles numero di tessere in mano
 * @param tilesOnHand tessere in mano
 * @param game situazione di gioco
 * @return Se la partita è finita
 */
bool endGame(int numTotalTiles, dominoTile tilesOnHand[], gameSituation game) {
    // Non ci sono più tessere in mano
    if (numTotalTiles == 0) return true;

    // Se non ho ancora messo tessere quindi punteggio della partita è 0
    if (game.score == 0) return false;

    // Controllo se ci sono tessere speciali e se si allora sicuramente la partita non sarà finita
    // perchè possono essere attaccate ovunque
    for (int i = 0; i < numTotalTiles; i++) {
        if (tilesOnHand[i].left == 0 || tilesOnHand[i].left == 12 || tilesOnHand[i].left == 11) return false;
    }

    // Controllo se ci sono combinazioni disponibili per continuare il gioco
    for (int row = 0; row < game.rows; row++) {
        for (int cell = 0; cell < game.cols; cell++) {
            if (game.inGameDominoTiles[row][cell].value != NOT_VALID_INT) { // Se c'è una tessera in gioco valida
                for (int onHandIndex = 0; onHandIndex < numTotalTiles; onHandIndex++) { //controllo se le tessere in mano hanno un numero da mettere sul tavolo
                    if (isValidMove(tilesOnHand[onHandIndex], game, row, cell, HORIZONTAL, LEFT)
                        || isValidMove(tilesOnHand[onHandIndex], game, row, cell, HORIZONTAL, RIGHT)
                        || isValidMove(tilesOnHand[onHandIndex], game, row, cell, VERTICAL, LEFT)) {
                        return false;
                    }
                    dominoTile tileReversed;
                    tileReversed.left=tilesOnHand[onHandIndex].right;
                    tileReversed.right=tilesOnHand[onHandIndex].left;
                    if (isValidMove(tileReversed, game, row, cell, HORIZONTAL, LEFT)
                        || isValidMove(tileReversed, game, row, cell, HORIZONTAL, RIGHT)
                        || isValidMove(tileReversed, game, row, cell, VERTICAL, LEFT)) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

