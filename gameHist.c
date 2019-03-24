#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


// moves will be recorded in the history for replaying a game and on a stack
// for undo/redo
struct move{
  int position;
  char player;
  int wasSafe; // for Tic Toc Boom, to indicate if bomb was there
};


struct moveList{
  struct move oneMove;
  struct moveList * link;
};


struct game{
  char playerX[10];
  char playerO[10];
  char winner;
  struct moveList * moves; // as each player has 3 undo/redo
};



void startGameHist(struct game *, char *, char *);

void addMove(struct moveList **, int, char, int);

void displayGame(struct game *);

int main(int argc, char const *argv[]) {
  struct game myGame;
  startGame(&myGame, "Peter", "Yola");
  printf("%s plays with %s\n", myGame.playerX, myGame.playerO);
  struct moveList ** gameMoves;
  gameMoves = &(myGame.moves);
  addMove(gameMoves, 5, 'X', 1);
  addMove(gameMoves, 1, 'O', 1);
  addMove(gameMoves, 6, 'X', 1);
  addMove(gameMoves, 4, 'O', 1);
  displayGame(&myGame);


  return 0;
}


void startGame(struct game * game, char * plX, char * plO){
  strcpy(game -> playerX, plX);
  strcpy(game -> playerO, plO);
  struct moveList * moves;
  moves = NULL;
  //moves = (struct moveList *) malloc (sizeof(struct moveList));
  game -> moves = moves;

}

void addMove(struct moveList ** moves, int pos , char pl, int safe){
  struct move * tempMove;
  struct moveList * current;
  if(*moves == NULL){

    tempMove = (struct move *) malloc(sizeof(struct move));
    tempMove -> position = pos;
    tempMove -> player = pl;
    tempMove -> wasSafe = safe;

    struct moveList * tempMoveList;
    tempMoveList = (struct moveList *) malloc (sizeof(struct moveList));
    tempMoveList -> oneMove = *tempMove;
    tempMoveList -> link = NULL;

    *moves = tempMoveList;
  }
  else{
    current = *moves;
    while(current -> link != NULL){
      current = current -> link;
    }
    tempMove = (struct move *) malloc(sizeof(struct move));
    tempMove -> position = pos;
    tempMove -> player = pl;
    tempMove -> wasSafe = safe;

    struct moveList * tempMoveList;
    tempMoveList = (struct moveList *) malloc (sizeof(struct moveList));
    tempMoveList -> oneMove = *tempMove;
    tempMoveList -> link = NULL;

    current -> link = tempMoveList;

  }
  printf("Move added\n" );
}


void displayGame(struct game * oneGame){
  printf("%s* VS %s\n", oneGame -> playerX, oneGame -> playerO);
  struct moveList * temp;
  temp = oneGame -> moves;
  while(temp != NULL){
    if(temp -> oneMove.player == 'X'){
      printf("%s placed %c in position %d\n", oneGame -> playerX,  temp -> oneMove.player, temp -> oneMove.position );
    }else{
      printf("%s placed %c in position %d\n", oneGame -> playerO,  temp -> oneMove.player, temp -> oneMove.position );
    }

    temp = temp ->link;
  }
}
