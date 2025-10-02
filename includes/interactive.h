#ifndef INTERACTIVE_H
#define INTERACTIVE_H

/**
 * @file interactive.h
 * @author Daniele Agnoletto
 * @brief Libreria contenente funzioni della modalita' interattiva
 * @date 06/02/2024
 */

#include "common_functions.h"
#include <ctype.h>

void playInteractive(int, dominoTile[]);
int chooseTile(int);
void insertTile(gameSituation*, dominoTile*, dominoTile, int, int, char, int*);
void printTilesInGame(gameSituation);
bool hasSpecials(dominoTile[], int);

#endif // INTERACTIVE_H
