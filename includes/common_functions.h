#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

/**
 * @file common_functions.h
 * @author Nicolae/Daniele
 * @brief Libreria contenente funzioni comuni tra la modalità interattiva del domino e la
 * modalità AI.
 * @date 06/02/2024
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define LEFT 'L'
#define RIGHT 'R'
#define TOP 'T'
#define BOTTOM 'B'
#define HORIZONTAL 'H'
#define VERTICAL 'V'
#define NOT_VALID_INT (-1)
#define NOT_VALID_CHAR '*'

/**
 * Tipo di dato per le tessere generate del domino
 * @var left parte sinistra della tessera
 * @var right parte destra della tessera
 */
typedef struct{
    int left;
    int right;
}dominoTile;

/**
 * Tipo di dato per il singolo lato della tessera inserito nel campo da gioco
 * @var value valore
 * @var side specifica quale lato della tessera è: R(Right) or L(Left) || T(Top) or B(Bottom)
 * @var orientation specifica se la tessera è in verticale oppure orizzontale: H(Horizontal) || V(Vertical)
 */
typedef struct{
    int value;
    char side;
    char orientation;
}dominoTileSide;

/**
 * Tipo di dato che contiene tutte le info sulla situazione di gioco
 * @var **inGameDominoTiles puntatore all'array di array le cui celle contengono i
 * singoli lati delle tessere attualmente in gioco
 * @var score punteggio
 * @var rows righe del campo da gioco
 * @var cols colonne del campo da gioco
 */
typedef struct {
    dominoTileSide **inGameDominoTiles;
    int score;
    int rows;
    int cols;
}gameSituation;


// Firme funzioni
void generateTiles(int numTotalTiles, int numSpecials, dominoTile[]);
void printTiles(int numTiles, dominoTile[]);
void moveRight(gameSituation*, int);
dominoTileSide **createInGameDominoTiles(int, int);
void initializeMatrix(dominoTileSide**, int, int);
void freeInGameDominoTiles (dominoTileSide **dominoTilesInGame, int rows);
void rotateDominoTile(dominoTile*);
void fixSpecialTiles(dominoTile*, gameSituation*, int, int, char, char);
bool isValidMove(dominoTile, gameSituation, int, int, char, char);
bool endGame(int numTotalTiles, dominoTile  [], gameSituation game);

#endif // COMMON_FUNCTIONS_H