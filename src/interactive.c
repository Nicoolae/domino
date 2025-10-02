/**
 * @file interactive.c
 * @author Daniele Agnoletto
 * @brief Libreria contenente funzioni della modalita' interattiva
 * @date 06/02/2024
 */

#include "interactive.h"

/**
* Funzione per la modalita' interattiva
* @param numTiles numero di tessere in mano
* @param dominoTiles tessere del domino in mano
*/
void playInteractive(int numTiles, dominoTile dominoTiles[]) {
    gameSituation game;
    int numTotalTiles = numTiles;
    game.inGameDominoTiles= createInGameDominoTiles(numTiles * 2, numTiles * 2); // Inizializzo il campo con il massimo di tessere che può contenere
    game.rows=1;
    game.cols=2;
    game.score=0;
    do { // Fino a che non ho piu' carte da giocare
        printf("Il tuo punteggio e' di: %d\n", game.score);
        printf("Inserisci la posizione della tessera da mettere sul tavolo\n");
        int onHandIndex=chooseTile(numTiles);
        bool isSpecial=(dominoTiles[onHandIndex].left==0 || dominoTiles[onHandIndex].left==11 || dominoTiles[onHandIndex].left==12);
        if (game.score==0) {  // Se primo turno
            if (!isSpecial)  // Se tessera non speciale
                insertTile(&game, &dominoTiles[onHandIndex], dominoTiles[numTiles-1],0, 0, HORIZONTAL, &numTiles);
            else printf("Non puoi iniziare con una tessera speciale\n");
        }
        else     // Se non e' il primo turno
        {
            int row,col;
            char orientation=NOT_VALID_CHAR, isLinkedTo=NOT_VALID_CHAR;
            do {
            printf("Inserisci le coordinate della tessera a cui vuoi collegare la tessera in mano (inserisci valori validi)\n");
            printf("Riga: ");
            scanf("%d",&row);
            printf("Colonna: ");
            scanf("%d",&col);
            } while ((row<0 || row>=game.rows) || (col<0 || col>=game.cols));
            // Gira tessera
            if (!isSpecial) { // Se non e' una tessera speciale chiedi se vuole girare
                char rotate=NOT_VALID_CHAR;
                printf("Vuoi girare la tessera? y/n: ");
                while (rotate!='y' && rotate!='n')
                scanf("%c",&rotate);
                if (rotate=='y') rotateDominoTile(&dominoTiles[onHandIndex]);
            }
            //Chiedi orientamento tessera
        printf("Scegliere orientamento tessera (h=orizzontale v=verticale b=back): ");
        while (orientation!='h' && orientation!='v' && orientation!='b')
        scanf("%c", &orientation);
        orientation=toupper(orientation);

        if (game.inGameDominoTiles[row][col].orientation==VERTICAL) { // Se l'orientamento della tessera adiacente e' verticale
                if (orientation==VERTICAL)  // Se l'orientamento della tessera che sto mettendo e' verticale
                    isLinkedTo = (col==0) ? LEFT : RIGHT; // Se sono sulla prima colonna si linka a sx, se e' sull'ultima si linka a dx
                 else { // Se l'orientamento della tessera che sto mettendo e' orizzontale
                    if (col+1<game.cols && game.inGameDominoTiles[row][col+1].value!=NOT_VALID_INT) isLinkedTo=LEFT; // Se esiste col+1 e il posto successivo e' occupato, la tessera da mettere e' linkata a sx
                    else if (col-1>=0 && game.inGameDominoTiles[row][col-1].value!=NOT_VALID_INT) isLinkedTo=RIGHT; //Se esiste col-1 e il posto successivo e' occupato, la tessera da mettere e' linkata a dx
                    else { //Se posso scegliere da che parte linkarla
            printf("Vuoi collegare la tessera a sinistra o destra? (l=sinistra r=destra): ");
            while (isLinkedTo!='l' && isLinkedTo!='r')
            scanf("%c", &isLinkedTo);
            isLinkedTo=toupper(isLinkedTo);
                }
            }
        } else // Se l'orientamento della tessera adiacente e' orizzontale
            isLinkedTo = (game.inGameDominoTiles[row][col].side==LEFT) ? LEFT : RIGHT; // Se la parte presa nel campo e' quella sx allora si colleghera' a sx, senno' viceversa

        if (isValidMove(dominoTiles[onHandIndex],game,row,col,orientation,isLinkedTo)) { // Se la mossa e' valida
            if (dominoTiles[onHandIndex].left==12 || dominoTiles[onHandIndex].left==11) {
                fixSpecialTiles(dominoTiles+onHandIndex,&game,row,col,orientation,isLinkedTo);
                }

                if (orientation==HORIZONTAL) {
                    if (isLinkedTo==LEFT) {
                        if (col==0 || col==1) {
                            game.cols+=2-col; // Se col==1 cols++, se col==0 cols=cols+2
                            moveRight(&game,2-col); // Se col==0 shifto di 2, senno' shifto di 1
                            col=0; //la tessera viene messa in prima posizione della colonna
                        }
                        else col-=2; // Se non si usano le prime due colonne, la posizione della tessera da mettere sara' di col-2
                    }
                    else if (isLinkedTo==RIGHT) {
                        if (col==game.cols-1) game.cols+=2; // Se si usa ultima colonna, bisogna aumentare il campo di 2
                        else if (col==game.cols-2) game.cols++; // Se si usa penultima colonna, bisogna aumentare il campo di 1
                        col++; // La tessera andra' messa una colonna dopo
                    }
                }
                else if (orientation==VERTICAL) {
                    game.cols++; // Aumentera' sempre di 1 la colonna del campo da gioco
                    if (col==0) moveRight(&game,1); // Se uso prima colonna, shifto il campo di 1
                    else col = (isLinkedTo==LEFT) ? col-1 : col+1; // Se linkato a sx posiziono a col-1, senno' a col+1
                    if (row==game.rows-1) game.rows++; //Se uso ultima riga, aggiungo una riga al campo
                }
          insertTile(&game, &dominoTiles[onHandIndex], dominoTiles[numTiles-1], row, col, orientation, &numTiles);
        } else printf("Non puoi fare questa mossa\n");
        }
        printTiles(numTiles, dominoTiles);
        printTilesInGame(game);
    }
    while (!endGame(numTiles, dominoTiles, game));
// Libero memoria e mostro punteggio
    //free(dominoTiles);
    freeInGameDominoTiles(game.inGameDominoTiles, (numTotalTiles * 2));
    printf("partita finita! Il tuo punteggio e' di %d\n", game.score);
}

/**
 * Controlla se tra le tessere presenti ci sono speciali
 * @param tilesOnHand tessere in mano
 * @param numTotalTiles numero di tessere in mano
 */
bool hasSpecials(dominoTile tilesOnHand[], int numTotalTiles){
    // Controllo se ci sono tessere speciali e se si allora sicuramente la partita non sarà finita
    // perchè possono essere attaccate ovunque
    for (int i = 0; i < numTotalTiles; i++) {
        if (tilesOnHand[i].left == 0 || tilesOnHand[i].left == 12 || tilesOnHand[i].left == 11) return true;
    }
    return false;
}

/**
* permette di scegliere all'utente una delle tessere in mano
* @param numTiles numero di tessere in mano
*/
int chooseTile(int numTiles) {
    int onHandIndex;
    do {
        printf("Inserisci una posizione valida: ");
        scanf("%d", &onHandIndex);
    }
    while (onHandIndex>numTiles || onHandIndex<1);
    return (onHandIndex-1);
}

/**
* inserisce una tessera nel campo da gioco
* @param game situazione di gioco contenente punteggio, righe, colonne e tessere in gioco
* @param dominoTileOnHand tessera del domino che si vuole mettere nel campo
* @param lastDominoTile ultima tessera in mano
* @param row riga della posizione dove si vuole mettere la tessera
* @param col colonna della posizione dove si vuole mettere la tessera
* @param orientation orientamento della tessera che si vuole posizionare (orizzontale o verticale)
* @param numTiles numero di tessere in mano
*/

void insertTile(gameSituation *game, dominoTile *dominoTileOnHand, dominoTile lastDominoTile,int row, int col, char orientation, int *numTiles) {
    if (orientation==HORIZONTAL) {
        game->inGameDominoTiles[row][col].value=dominoTileOnHand->left;
        game->inGameDominoTiles[row][col].side=LEFT;
        game->inGameDominoTiles[row][col+1].value=dominoTileOnHand->right;
        game->inGameDominoTiles[row][col+1].side=RIGHT;
        game->inGameDominoTiles[row][col].orientation=game->inGameDominoTiles[row][col+1].orientation=HORIZONTAL;
    } else {
        game->inGameDominoTiles[row][col].value=dominoTileOnHand->left;
        game->inGameDominoTiles[row][col].side=TOP;
        game->inGameDominoTiles[row+1][col].value=dominoTileOnHand->right;
        game->inGameDominoTiles[row+1][col].side=BOTTOM;
        game->inGameDominoTiles[row][col].orientation=game->inGameDominoTiles[row+1][col].orientation=VERTICAL;
    }
    game->score+=dominoTileOnHand->left + dominoTileOnHand->right;
    (*numTiles)--;
    *dominoTileOnHand=lastDominoTile;
}

/**
* Stampa il campo di gioco
* @param game situazione di gioco contenente punteggio, righe, colonne e tessere in gioco
*/

void printTilesInGame(gameSituation game) {
    printf("tessere sul tavolo:\n");
    printf("   ");
    for (int col=0; col<game.cols; col++) printf("%d  ",col);
    printf("\n");
    for (int row=0; row<game.rows; row++)
    {
        printf("%d ",row);
        for (int col=0; col<game.cols; col++)
        {
            if (game.inGameDominoTiles[row][col].value!=NOT_VALID_INT)
            {
                if (game.inGameDominoTiles[row][col].side==LEFT)
                    printf("[%d|", game.inGameDominoTiles[row][col].value);
                else if (game.inGameDominoTiles[row][col].side==RIGHT)
                    printf("|%d]", game.inGameDominoTiles[row][col].value);
                    else if (game.inGameDominoTiles[row][col].side==TOP)
                    printf("[%d:", game.inGameDominoTiles[row][col].value);
                    else if (game.inGameDominoTiles[row][col].side==BOTTOM)
                    printf(":%d]", game.inGameDominoTiles[row][col].value);
            }
            else printf("   ");
        }
        printf("\n");
    }
}
