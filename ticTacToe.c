#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE 9

struct board{
  char position[SIZE];
};




// linked dist to store gameHistory
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




// basic stack structure
struct stack{
  struct move oneMove;
  struct stack * next;
  struct stack * prev;
};



struct undoRedo{
  struct stack * mainStack;
  struct stack * undoneMoves;
};

//gameHistory
void startGameHist(struct game *, char *, char *);
void addMove(struct moveList **, int, char, int);
void displayGame(struct game *);


// stack for udo/ redo
void push(struct stack **, int, char, int);
struct stack * pop(struct stack *);
void displayStack(struct stack *); // just for testing
void mainStackPush(struct undoRedo * , int, char, int);
void undoneMovesPush(struct undoRedo * , int, char, int);
struct move * undo(struct undoRedo *);
struct move * redo(struct undoRedo *);
void resetUndoneStack(struct undoRedo *); // used whenever a new move is made



void initBoard(struct board*);
void drawBoard(struct board);
void drawBoardPositions();
int checkValidMove(struct board, int);
char checkWin(struct board);
int checkFinish(struct board);
void displayOptions();
void startApp();
char playSingleTwoPlayersMatch(char *, char *);
char playSingleTwoPlayersMatchRRS(char *, char *);
void playTwoPlayersGame(char *);

int main(int argc, char const *argv[]) {

  startApp();


  return 0;
}

void startApp(){
  printf("        ##############################\n");
  printf("        #         TIC TAC TOE        #\n");
  printf("        ##############################\n\n");

  int option;

  while(option != 5){
    displayOptions();
    scanf("%d",  &option);

    switch (option) {
      case 1:
      playTwoPlayersGame("Basic"); // classic TIC-TAC-TOE
      break;

      case 2:
      playTwoPlayersGame("RRS"); // Russian Roulette style
      break;

      case 3:
      printf("2 PLAYERS TIC TOC BOOM\n");
      break;

      case 4:
      printf("Play against Computer TIC TOC BOOM\n");
      break;

      case 5:
      printf("GOOD BYE!!\n");
      break;

      default :
      printf("SELECTED OPTION IS INVALID!\n\n");
    }
  }
}


void playTwoPlayersGame(char* type){
  char playerX[10];
  char playerO[10];
  printf("Please enter player's one name: ");
  scanf(" %[^\n]", playerX);
  printf("Please enter player's two name: ");
  scanf(" %[^\n]", playerO);
  int playerXwins = 0;
  int playerOwins = 0;

  char playAgain = 'n';
  char winner;
  do {
    if(strcmp(type,"Basic") == 0){
      winner = playSingleTwoPlayersMatch(playerX, playerO);
    }else{
      winner = playSingleTwoPlayersMatchRRS(playerX, playerO);
    }

    if(winner == 'X'){
      playerXwins++;
    }else if(winner == 'O'){
      playerOwins++;
    }

    printf("%s - %d VS %s - %d\n", playerX, playerXwins, playerO, playerOwins);

    printf("Do you want to play again? y/n\n");
    scanf(" %c", &playAgain);

  } while(playAgain == 'y' || playAgain == 'Y');

}



char playSingleTwoPlayersMatch(char *playerXname, char *playerOname){
  struct board myBoard;
  initBoard(&myBoard);
  struct undoRedo * unRe;
  unRe = (struct undoRedo *)malloc(sizeof(struct undoRedo));

  int i = 0; // turns
  int j = 0; //
  int startPos = 0;
  while(checkFinish(myBoard) == -1) {
    //display the board with positions only at the beginning
    if(i == 0){
      drawBoardPositions();
    }
    // adds 1 or 0 to i making sure that the first player is chosen at random
    // this is only done once at the begining of the game
    if(j == 0){
      srand(time(0)); // seed will be set to time to allow randomness
      i += rand()%2;
      startPos = i;
      j++;
    }

    int pos; // variable that will hold the selected position

    if(i%2 == 0){
      do {
        pos = 0;
        printf("Where would you like to place X %s?", playerXname);
        scanf("%d", &pos);
      } while(checkValidMove(myBoard, pos) == -1);
      if(pos <10){
        myBoard.position[pos-1] = 'X';
        mainStackPush(unRe, pos, 'X', 1);
        if(unRe -> undoneMoves != NULL){
          printf("in if undone is NULL\n" );
          resetUndoneStack(unRe); // each new move stops the ability to redo prev moves
        }
        i++;
      }else{
        if(pos == 10 && i > startPos){
          if(unRe -> mainStack != NULL){
            struct move * undoneMove = undo(unRe);
            myBoard.position[(undoneMove -> position)-1] = ' ';
            i--;
          }
        }
        if(pos == 11){
          if(unRe -> undoneMoves != NULL){
            printf("in redo\n" );
            int isLast;
            if(unRe -> undoneMoves -> next == NULL){
              isLast = 1;
            }else{
              isLast = -1;
            }
            struct move * undoneMove = redo(unRe);
            myBoard.position[(undoneMove -> position)-1] = 'X';
            if(isLast == 1){
              unRe -> undoneMoves = NULL;
            }
            i++;
          }else{
            printf("No moves to redo\n");
          }
        }
      }
      //displayStack(unRe -> mainStack);

    }else{

      do {
        printf("Where would you like to place O %s?", playerOname);
        scanf("%d", &pos);
      } while(checkValidMove(myBoard, pos) == -1);

      if(pos < 10){
        myBoard.position[pos-1] = 'O';
        mainStackPush(unRe, pos, 'O', 1);
        if(unRe -> undoneMoves != NULL){
          resetUndoneStack(unRe); // each new move stops the ability to redo prev moves
        }
        i++;
      }else{
        if(pos == 10 && i > startPos){
          if(unRe -> mainStack != NULL){
            struct move * undoneMove = undo(unRe);
            myBoard.position[(undoneMove -> position)-1] = ' ';
            i--;
          }
        }
        if(pos == 11){
          if(unRe -> undoneMoves != NULL){
            printf("in redo\n" );
            int isLast;
            if(unRe -> undoneMoves -> next == NULL){
              isLast = 1;
            }else{
              isLast = -1;
            }
            struct move * undoneMove = redo(unRe);
            myBoard.position[(undoneMove -> position)-1] = 'O';
            if(isLast == 1){
              unRe -> undoneMoves = NULL;
            }
            i++;
          }else{
            printf("No moves to redo\n");
          }
        }
      }
    }
    printf("%d %d for i and st pos\n", i , startPos);
    if(i == startPos){
      printf("in reset main stack\n" );
      unRe -> mainStack = NULL;
    }
    printf("Main stack:\n" );
    displayStack(unRe -> mainStack);
    printf("Undone Moves:\n");
    displayStack(unRe -> undoneMoves);

    if(i != startPos){
      drawBoard(myBoard);
    }



    char winner = checkWin(myBoard);
    if(winner != 'd'){
      if(winner == 'X'){
        printf("%s won the game!\n", playerXname);
        return winner;
      }else if(winner == 'O'){
        printf("%s won the game!\n", playerOname);
        return winner;
      }

      return checkWin(myBoard);
      break;
    }

  }
  return 'd'; // for draw
}

char playSingleTwoPlayersMatchRRS(char *playerXname, char *playerOname){
  struct board myBoard;
  initBoard(&myBoard);
  int numberOfBombs;
  int bombs[3];
  do {
    printf("Enter the number of bombs between 1-3\n");
    scanf(" %d", &numberOfBombs);
    printf("\n");
  } while(numberOfBombs != 1 && numberOfBombs != 2 && numberOfBombs != 3);

  printf("number of bombs %d\n", numberOfBombs );
  srand(time(0));
  int i = 0;
  // select at random locations of the bombs (number specified by user)
  while(i != numberOfBombs){
    int bomb;
    bomb = ((rand()%9)+1);
    // the first bomb position is always accepted
    if(i == 0){
      bombs[i] = bomb;
      i++;
    }
    // in case where more than one bombs is selected every other position is
    // checked if it is not alredy in the selected list
    else{
      int j;
      char exists = 'n';
      for (j = 0; j <= i; j++) {
        if(bombs[j] == bomb){
          exists = 'y';
        }
      }
      if(exists == 'n'){
        bombs[i] = bomb;
        i++;
      }
    }
  }

  int j = 0;
  i = 0; //
  while(checkFinish(myBoard) == -1) {
    //display the board with positions only at the beginning
    if(i == 0){
      drawBoardPositions();
    }
    // adds 1 or 0 to i making sure that the first player is chosen at random
    // this is only done once at the begining of the game
    if(j == 0){
      srand(time(0)); // seed will be set to time to allow randomness
      i += rand()%2;
      j++;
    }

    int pos; // variable that will hold the selected position
    int b;
    char hasBomb = 'n';
    if(i%2 == 0){

      do {
        printf("Where would you like to place X %s?", playerXname);
        scanf("%d", &pos);
      } while(checkValidMove(myBoard, pos) == -1);

      for (b = 0; b < numberOfBombs; b++) {
        if(bombs[b] == pos){
          printf("BOOM!!!!\n" );
          hasBomb = 'y';
          bombs[b]=10; // deactivate the bmob
        }
      }
      if(hasBomb == 'n'){
        myBoard.position[pos-1] = 'X';
      }


    }else{
      do {
        printf("Where would you like to place O %s?", playerOname);
        scanf("%d", &pos);
      } while(checkValidMove(myBoard, pos) == -1);
      for (b = 0; b < numberOfBombs; b++) {
        if(bombs[b] == pos){
          printf("BOOM!!!!\n" );
          hasBomb = 'y';
          bombs[b]=10; // deactivate the bmob
        }
      }
      if(hasBomb == 'n'){
        myBoard.position[pos-1] = 'O';
      }

    }
    drawBoard(myBoard);

    char winner = checkWin(myBoard);
    if(winner != 'd'){
      if(winner == 'X'){
        printf("%s won the game!\n", playerXname);
        return winner;
      }else if(winner == 'O'){
        printf("%s won the game!\n", playerOname);
        return winner;
      }

      return checkWin(myBoard);
      break;
    }
    i++;
  }
  return 'd';
}



int checkValidMove(struct board b, int move){
  if(move < 1 || move > 11){
    printf("Selected position must be betweeen 1 and 9 or 10 for undo, 11 for redo\n");
    return -1;
  }
  if(b.position[move -1] != ' ' && move < 10){
    printf("Position %d is already taken! \n", move);
    return -1;
  }else{
    return 1;
  }
}




void initBoard(struct board* b){
  int i;
  for (i = 0; i < SIZE; i++) {
    b -> position[i]= ' ';
  }
}



void drawBoard(struct board b)
{
  printf("\n     |     |     \n");
  printf("  %c  |  %c  |  %c  \n", b.position[0], b.position[1], b.position[2]);
  printf("_____|_____|_____\n");
  printf("     |     |     \n");
  printf("  %c  |  %c  |  %c  \n", b.position[3], b.position[4], b.position[5]);
  printf("_____|_____|_____\n");
  printf("     |     |     \n");
  printf("  %c  |  %c  |  %c  \n", b.position[6], b.position[7], b.position[8]);
  printf("     |     |     \n");

}
void drawBoardPositions()
{
  printf("\n     |     |     \n");
  printf("  1  |  2  |  3  \n");
  printf("_____|_____|_____\n");
  printf("     |     |     \n");
  printf("  4  |  5  |  6  \n");
  printf("_____|_____|_____\n");
  printf("     |     |     \n");
  printf("  7  |  8  |  9  \n");
  printf("     |     |     \n");
}

void displayOptions(){
  printf("********************************************************\n");
  printf("*  WHAT WOULD YOU LIKE TO DO?                          *\n");
  printf("*                                                      *\n");
  printf("*  PRESS 1 TO PLAY PLAYER 1 VS PLAYER 2 CLASSIC GAME   *\n");
  printf("*  PRESS 2 TO PLAY PLAYER 1 VS CPU GAME                *\n");
  printf("*  PRESS 3 TO PLAY PLAYER 1 VS PLAYER 2 TIC TOC BOOM   *\n");
  printf("*  PRESS 4 TO PLAY PLAYER 1 VS CPU TIC TOC BOOM        *\n");
  printf("*  PRESS 5 TO EXIT                                     *\n");
  printf("********************************************************\n");
}

// checks if either of the players won the game. If X won X will be returned,
// if O won O will be returned, otherwize d for draw will be returned
char checkWin(struct board b){
  if(b.position[0] == b.position[1] &&  b.position[1] == b.position[2]
    && b.position[0] != ' '){
    return b.position[0];
  }else if (b.position[3] == b.position[4] &&  b.position[4] == b.position[5]
    && b.position[3] != ' ') {
    return b.position[3];
  }else if (b.position[6] == b.position[7] &&  b.position[7] == b.position[8]
    && b.position[6] != ' ') {
    return b.position[6];
  }else if (b.position[0] == b.position[3] &&  b.position[3] == b.position[6]
    && b.position[0] != ' ') {
    return b.position[0];
  }else if (b.position[1] == b.position[4] &&  b.position[4] == b.position[7]
    && b.position[1] != ' ') {
    return b.position[1];
  }else if (b.position[2] == b.position[5] &&  b.position[5] == b.position[8]
    && b.position[2] != ' ') {
    return b.position[2];
  }else if (b.position[0] == b.position[4] &&  b.position[4] == b.position[8]
    && b.position[0] != ' ') {
    return b.position[0];
  }else if (b.position[2] == b.position[4] &&  b.position[4] == b.position[6]
    && b.position[2] != ' ') {
    return b.position[2];
  }
  return 'd'; // returns d for draw
}


int checkFinish(struct board b){
  int i;
  for (i = 0; i < SIZE; i++) {
    if(b.position[i] == ' '){
      return -1;
    }
  }
  printf("Draw!!\n");
  return 1;
}














// stack

void displayStack(struct stack * myStack){
  while(myStack != NULL){
    printf("%c at position %d  -- ", myStack -> oneMove.player, myStack -> oneMove.position);
    if(myStack -> prev != NULL){
      printf("Prev: %c at position %d\n", myStack -> prev -> oneMove.player, myStack -> prev -> oneMove.position);
    }
    printf("\n");
    myStack = myStack -> next;
  }
}

// acts as a push method for stack
void push(struct stack ** moves, int pos , char pl, int safe){
  struct move * tempMove;
  struct stack * current;
  //struct stack ** previous;
  if(*moves == NULL){

    tempMove = (struct move *) malloc(sizeof(struct move));
    tempMove -> position = pos;
    tempMove -> player = pl;
    tempMove -> wasSafe = safe;

    struct stack * tempstack;
    tempstack = (struct stack *) malloc (sizeof(struct stack));
    tempstack -> oneMove = *tempMove;
    tempstack -> next = NULL;
    tempstack -> prev = NULL;

    *moves = tempstack;
  }
  else{
    current = *moves;
    while(current -> next != NULL){
      //previous = &current;
      current = current -> next;
    }
    tempMove = (struct move *) malloc(sizeof(struct move));
    tempMove -> position = pos;
    tempMove -> player = pl;
    tempMove -> wasSafe = safe;

    struct stack * tempstack;
    tempstack = (struct stack *) malloc (sizeof(struct stack));
    tempstack -> oneMove = *tempMove;
    tempstack -> next = NULL;
    tempstack -> prev = current;

    current -> next = tempstack;

  }
}


void mainStackPush(struct undoRedo* unRedo, int pos, char pl, int safe){
  struct stack ** mainSt;
  mainSt = &(unRedo -> mainStack);
  push(mainSt, pos, pl, safe);
}

void undoneMovesPush(struct undoRedo* unRedo, int pos, char pl, int safe){
  struct stack ** undoneSt;
  undoneSt = &(unRedo -> undoneMoves);
  push(undoneSt, pos, pl, safe);
}


struct stack * pop(struct stack * st){
  struct stack * top;
  top = st;
  if(top == NULL){
    return NULL;
  }

  while(top -> next != NULL){
    top = top ->next;
  }

  if(top -> prev != NULL){
    top -> prev -> next = NULL;
  }
  return top;
}

struct move * undo(struct undoRedo * unRe){
  struct stack * mainSt;
  struct stack * undoneSt;
  struct stack * popped;
  mainSt = unRe -> mainStack;
  undoneSt = unRe -> undoneMoves;

  if(mainSt != NULL){
    popped = pop(mainSt);
    struct move * mv = &(popped -> oneMove);
    // could also push the popped item but I have already created a func that takes
    // other arguments than a stack
    int pos = mv -> position;
    char pl = mv -> player;
    int safe = mv -> wasSafe;
    undoneMovesPush(unRe, pos , pl, safe);
    free(popped);
    return mv;
  }
  return NULL;
}

struct move * redo(struct undoRedo * unRe){
  struct stack * mainSt;
  struct stack * undoneSt;
  struct stack * popped;
  mainSt = unRe -> mainStack;
  undoneSt = unRe -> undoneMoves;

  if(undoneSt != NULL){
    popped = pop(undoneSt);
    struct move * mv = &(popped -> oneMove);
    // could also push the popped item but I have already created a func that takes
    // other arguments than a stack
    int pos = mv -> position;
    char pl = mv -> player;
    int safe = mv -> wasSafe;
    mainStackPush(unRe, pos , pl, safe);
    free(popped);
    return mv;
  }
  return NULL;
}


void resetUndoneStack(struct undoRedo * unRe){
  printf("Im in reset undone\n" );
  unRe -> undoneMoves = NULL;
}



// Gamie history


void startGameHist(struct game * game, char * plX, char * plO){
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
