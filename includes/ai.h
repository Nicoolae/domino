#ifndef AI_H
#define AI_H

/**
 * @file ai.h
 * @author Nicolae Boldisor
 * @brief Libreria contenente le funzioni per la modalità AI del domino.
 * @date 25/02/2024
 */

// Includes
#include "common_functions.h"
#include <string.h>

// Functions prototypes
void playAI(dominoTile[], int);
void printAI(gameSituation*);
gameSituation *getBestGameAI(dominoTile*, int, gameSituation*);
bool hasTileAdjacency(dominoTile, gameSituation);
void move_left(dominoTile*, int, dominoTile);
bool hasThisTileIn(dominoTile toFound, dominoTile *ar, int toCheckUntil);
gameSituation *copyGame(gameSituation, char, int);
dominoTileSide **copyMatrixSides(gameSituation, int, int);
void addDominoTile(gameSituation*, dominoTile, int, int, char, char);
void freeGameSituation(gameSituation*);
void copyDominoTileAr(dominoTile[], dominoTile*, int);
gameSituation *bestTileInEachPosition(dominoTile*, int, gameSituation, char, char, int, int);
gameSituation *bestGameTileIn(dominoTile*, int, gameSituation);

#endif // AI_H