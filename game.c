#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct savedGames ** gameList; // global variable for saved games

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

struct savedGames{
  struct game thisGame;
  struct savedGames * nextGame;
};




// basic stack structure
struct stack{
  struct move moves[9];
  int top;
};

struct undoRedo{
  struct stack mainStack;
  struct stack undoneMoves;
};

//gameHistory
void startGameHist(struct game *, char *, char *);
void addMove(struct moveList **, int, char, int);
void displayGame(struct game *);
void addSavedGame(struct savedGames **, struct game*);
void askToSave(struct savedGames **, struct game *);
void replayGame(int);


// stack for udo/ redo
void init_stack(struct stack *);
void init_undoRedo(struct undoRedo *);
void push(struct stack *, struct move);
struct move * pop(struct stack *);
void display(struct undoRedo *);
void unReAddNewMove(struct undoRedo *, struct move);
struct move * undo(struct undoRedo *);
struct move * redo(struct undoRedo *);



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
void savedGamesMenu();
int generateMove(struct board);


int main(int argc, char const *argv[]) {

  gameList = (struct savedGames **)malloc(sizeof(struct savedGames));
  *gameList = NULL;

  struct board b;
  initBoard(&b);
  generateMove(b);

  //startApp();



  return 0;
}

void startApp(){
  printf("        ##############################\n");
  printf("        #         TIC TAC TOE        #\n");
  printf("        ##############################\n\n");

  int option;

  while(option != 6){
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
      savedGamesMenu();
      break;

      case 6:
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
  init_undoRedo(unRe);
  struct game * oneGame;
  oneGame = (struct game *)malloc(sizeof(struct game));
  startGameHist(oneGame, playerXname, playerOname);
  struct moveList ** recordedMoves = &(oneGame -> moves);



  int i = 0; // turns
  int j = 0; //
  int startPos = 0; // used to display positions when all moves are undone
  while(checkFinish(myBoard) == -1) {
    display(unRe);
    //display the board with positions only at the beginning
    if(i == startPos){
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
        struct move * mv = (struct move *)malloc(sizeof(struct move));
        mv -> position = pos;
        mv -> player = 'X';
        unReAddNewMove(unRe, *mv);
        i++;
      }else{
        if(pos == 10 && i > startPos){
          if(unRe -> mainStack.top != -1){
            struct move * undoneMove = undo(unRe);
            myBoard.position[(undoneMove -> position)-1] = ' ';
            i--;
            pos = undoneMove -> position;
          }

        }
        if(pos == 11){
          if(unRe -> undoneMoves.top != -1){
            struct move * redoneMove = redo(unRe);
            myBoard.position[(redoneMove -> position)-1] = 'X';
            i++;
            pos = redoneMove -> position;
          }else{
            printf("No moves to redo\n");
          }
        }
      }
      addMove(recordedMoves, pos, 'X', 1);


    }else{

      do {
        printf("Where would you like to place O %s?", playerOname);
        scanf("%d", &pos);
      } while(checkValidMove(myBoard, pos) == -1);

      if(pos < 10){
        myBoard.position[pos-1] = 'O';

        struct move * mv = (struct move *)malloc(sizeof(struct move));
        mv -> position = pos;
        mv -> player = 'O';
        unReAddNewMove(unRe, *mv);

        i++;
      }else{
        if(pos == 10 && i > startPos){
          if(unRe -> mainStack.top != -1){
            struct move * undoneMove = undo(unRe);
            myBoard.position[(undoneMove -> position)-1] = ' ';
            i--;
            pos = undoneMove -> position;
          }
        }
        if(pos == 11){
          if(unRe -> undoneMoves.top != -1){
            struct move * redoneMove = redo(unRe);
            myBoard.position[(redoneMove -> position)-1] = 'O';
            i++;
            pos = redoneMove -> position;
          }else{
            printf("No moves to redo\n");
          }
        }
      }
      addMove(recordedMoves, pos, 'O', 1);
    }
    displayGame(oneGame);
    if(i != startPos){
      drawBoard(myBoard);
    }



    char winner = checkWin(myBoard);
    if(winner != 'd'){
      if(winner == 'X'){
        printf("%s won the game!\n", playerXname);
        oneGame -> winner = 'X';
        askToSave(gameList, oneGame);
        return winner;
      }else if(winner == 'O'){
        printf("%s won the game!\n", playerOname);
        oneGame -> winner = 'O';
        askToSave(gameList, oneGame);
        return winner;
      }

      return checkWin(myBoard);
      break;
    }

  }
  oneGame -> winner = 'd';
  askToSave(gameList, oneGame);
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
  printf("*  PRESS 5 FOR SAVED GAMES                             *\n");
  printf("*  PRESS 6 TO EXIT                                     *\n");
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



void init_stack(struct stack * st){
  st -> top = -1;
}

void init_undoRedo(struct undoRedo * unRe){
  init_stack(&(unRe -> mainStack));
  init_stack(&(unRe -> undoneMoves));
}

void push(struct stack * st, struct move mv){
  //printf("The top is %d\n", st->top +1);
  //printf("Adding pos %d by player %c at position %d\n", mv.position, mv.player, st->top+1);
  if(st->top < 8){
    st -> top++;
    printf("Adding pos %d by player %c at position %d\n", mv.position, mv.player, st->top);
    st -> moves[st -> top] = mv;

  }
}

struct move * pop(struct stack * st){
  if(st -> top >= 0){
    struct move * mv = &(st -> moves[st -> top]);
    st -> top--;
    return mv;
  }
  return NULL;
}

void unReAddNewMove(struct undoRedo * unRe, struct move mv){
  push(&(unRe -> mainStack), mv);
  unRe -> undoneMoves.top = -1;// whenever a new move is added the redo function is no longer available
}

struct move * undo(struct undoRedo * unRe){
  if(unRe -> mainStack.top >= 0){
    struct move * mv = pop(&(unRe -> mainStack));
    printf("The move to be put in undone: pos - %d, pl - %c\n", mv->position, mv->player );
    push(&(unRe -> undoneMoves), *mv);
    return mv;
  }
  return NULL;
}

struct move * redo(struct undoRedo * unRe){
  if(unRe -> undoneMoves.top >= 0){
    struct move * mv = pop(&(unRe -> undoneMoves));
    printf("The move to be put in main Stack: pos - %d, pl - %c\n", mv->position, mv->player );
    push(&(unRe -> mainStack), *mv);
    return mv;
  }
  return NULL;
}





void display(struct undoRedo * unRe){
  struct stack mainSt = unRe->mainStack;
  struct stack undoneSt = unRe->undoneMoves;
  printf("Main Stack:\n");
  if(mainSt.top != -1){
    int i;

    for(i = 0; i <= mainSt.top; i++){
      printf("position - %d, player - %c\n",mainSt.moves[i].position, mainSt.moves[i].player );
    }
  }
  printf("Undone Stack:\n");
  if(undoneSt.top != -1){
    int j;

    for(j = 0; j <= undoneSt.top; j++){
      printf("position - %d, player - %c\n",undoneSt.moves[j].position, undoneSt.moves[j].player );
    }
  }

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

  if(oneGame -> winner == 'X'){
    printf("*%s* VS %s\n", oneGame -> playerX, oneGame -> playerO);
  }
  if(oneGame -> winner == 'O'){
    printf("%s VS *%s*\n", oneGame -> playerX, oneGame -> playerO);
  }
  if(oneGame -> winner == 'd'){
    printf("%s VS %s\n", oneGame -> playerX, oneGame -> playerO);
  }
}
// void displayGame(struct game * oneGame){
//   printf("in display Game\n" );
//   printf("%s VS %s\n", oneGame -> playerX, oneGame -> playerO);
//   struct moveList * temp;
//   temp = oneGame -> moves;
//   while(temp != NULL){
//     if(temp -> oneMove.player == 'X'){
//       printf("%s placed %c in position %d\n", oneGame -> playerX,  temp -> oneMove.player, temp -> oneMove.position );
//     }else{
//       printf("%s placed %c in position %d\n", oneGame -> playerO,  temp -> oneMove.player, temp -> oneMove.position );
//     }
//
//     temp = temp ->link;
//   }
// }

void addSavedGame(struct savedGames ** savedGameList, struct game * newGame){
  struct savedGames * new = (struct savedGames *)malloc(sizeof(struct savedGames));
  new -> thisGame = *newGame;
  new -> nextGame = NULL;
  if(*savedGameList == NULL){
    *savedGameList = new;
  }else{
    struct savedGames * current = *savedGameList;
    while(current -> nextGame != NULL){
      current = current -> nextGame;
    }

    current -> nextGame = new;
  }
}

void askToSave(struct savedGames ** gameList, struct game * game){
  char answer;
  printf("Would you like to save the game? y/n:\n " );
  scanf(" %c", &answer);
  if(answer == 'y' || answer == 'Y'){
    addSavedGame(gameList, game);
  }
}



void savedGamesMenu(){
  if(*gameList != NULL){
    printf("   SAVED GAMES\n");
    int i = 1;
    struct savedGames * games = *gameList;
    while (games -> nextGame != NULL) {
    printf("%d. ",i );
    displayGame(&(games -> thisGame));
    games = games -> nextGame;
    i++;
    }
    printf("%d. ",i );
    displayGame(&(games -> thisGame));
    printf("Enter the number of the game to replay, or 0 to exit: \n");
    int choice;
    scanf("%d", &choice);
    printf("\n");
    if(choice == 0){
      printf("Exit saved Games\n" );
    }
    if(choice > 0 && choice <= i){
      printf("in choice above 0\n" );
      replayGame(choice);
    }else{
      printf("Game number %d does not exist\n", choice );
      savedGamesMenu();
    }

  }else{
    printf("There are no saved games\n");
  }

}

void replayGame(int gameNo){
  int i;
  struct savedGames * games = *gameList;
  for (i = 1; i < gameNo; i++) {
    games = games -> nextGame;
  }
  printf("The selected game is \n" );
  displayGame(&(games -> thisGame));
  struct moveList * moves = games -> thisGame.moves;
  struct board replayBoard;
  initBoard(&replayBoard);
  while (moves -> link != NULL) {
    // if(moves -> oneMove.player == 'X'){
    //   printf("%s selected position %d\n", games -> thisGame.playerX, moves -> oneMove.position );
    // }else{
    //   printf("%s selected position %d\n", games -> thisGame.playerX, moves -> oneMove.position );
    // }
    if(moves -> oneMove.wasSafe == -1){
      printf("BOOM!!! THERE WAS A BOMB IN THIS POSITION! NOW IT IS SAFE!\n");
    }else{
      int pos = moves -> oneMove.position;
      char pl = moves -> oneMove.player;
      printf("\n");
      if(replayBoard.position[pos-1] != ' '){
        replayBoard.position[pos-1] = ' ';
        if(pl == 'X'){
          printf("%s undone the move at position %d\n", games -> thisGame.playerO, pos);
        }else{
          printf("%s undone the move at position %d\n", games -> thisGame.playerX, pos);
        }

      }else{
        replayBoard.position[pos-1] = pl;
        if(pl == 'X'){
          printf("%s selected position %d\n", games -> thisGame.playerX, pos);
        }else{
          printf("%s selected position %d\n", games -> thisGame.playerO, pos);
        }
      }
    }
    drawBoard(replayBoard);
    moves = moves -> link;
    sleep(2);
  }
  replayBoard.position[moves -> oneMove.position -1] = moves -> oneMove.player;
  if(moves -> oneMove.player == 'X'){
    printf("%s SELECTED POSITION %d AND WON THE GAME!\n",
    games -> thisGame.playerX, moves -> oneMove.position);
  }else{
    printf("%s SELECTED POSITION %d AND WON THE GAME!\n",
    games -> thisGame.playerO, moves -> oneMove.position);
  }

  drawBoard(replayBoard);
  sleep(2);

}


int generateMove(struct board b){
  printf("Generating move\n" );
  // if board is empty then select at randomnes
  int isEmpty = 1;
  int i;
  for (i = 0; i < 9; i++) {
    if(b.position[i] != ' '){
      isEmpty = -1;
    }
  }
  if(isEmpty ==1){
    srand(time(0)); // seed will be set to time to allow randomness
    int randomPosition = rand()%9;
    printf("random pos is: %d\n", randomPosition);
    return randomPosition;
  }
  //block or finish game
  if(b.position[0] == b.position[1] && b.position[2] == ' '){
    return 2;
  }else if (b.position[1] == b.position[2] && b.position[0] == ' ') {
    return 0;
  }else if (b.position[1] == b.position[2] && b.position[0] == ' ') {
    return 0;
  }else if (b.position[3] == b.position[4] && b.position[5] == ' ') {
    return 5;
  }else if (b.position[4] == b.position[5] && b.position[3] == ' ') {
    return 3;
  }else if (b.position[6] == b.position[7] && b.position[8] == ' ') {
    return 8;
  }else if (b.position[7] == b.position[8] && b.position[6] == ' ') {
    return 6;
  }else if (b.position[0] == b.position[2] && b.position[1] == ' ') {
    return 1;
  }else if (b.position[3] == b.position[5] && b.position[4] == ' ') {
    return 4;
  }else if (b.position[6] == b.position[8] && b.position[7] == ' ') {
    return 7;
  }else if (b.position[0] == b.position[3] && b.position[6] == ' ') {
    return 6;
  }else if (b.position[3] == b.position[6] && b.position[0] == ' ') {
    return 0;
  }else if (b.position[0] == b.position[6] && b.position[3] == ' ') {
    return 3;
  }else if (b.position[1] == b.position[4] && b.position[7] == ' ') {
    return 7;
  }else if (b.position[1] == b.position[7] && b.position[4] == ' ') {
    return 4;
  }else if (b.position[4] == b.position[7] && b.position[1] == ' ') {
    return 1;
  }else if (b.position[2] == b.position[5] && b.position[8] == ' ') {
    return 8;
  }else if (b.position[5] == b.position[8] && b.position[2] == ' ') {
    return 2;
  }else if (b.position[2] == b.position[8] && b.position[5] == ' ') {
    return 5;
  }else if (b.position[0] == b.position[4] && b.position[8] == ' ') {
    return 8;
  }else if (b.position[4] == b.position[8] && b.position[0] == ' ') {
    return 0;
  }else if (b.position[0] == b.position[8] && b.position[4] == ' ') {
    return 4;
  }else if (b.position[2] == b.position[4] && b.position[6] == ' ') {
    return 6;
  }else if (b.position[4] == b.position[6] && b.position[2] == ' ') {
    return 2;
  }else if (b.position[2] == b.position[6] && b.position[4] == ' ') {
    return 4;
  }
  return -1;
}
