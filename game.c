#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct savedGames ** gameList; // global variable for saved games

#define SIZE 9

// board implemented as an array of chars
struct board{
  char position[SIZE];
};


// data structure for each move
struct move{
  int position;
  char player;
  int wasSafe; // for Tic Toc Boom, to indicate if bomb was there
};

// data structure for storing all moves of a game
// implemented as a linked list
struct moveList{
  struct move oneMove;
  struct moveList * link;
};

// a structure that holds all information about one game
struct game{
  char playerX[10];
  char playerO[10];
  char winner; // can be X, O or d for draw
  struct moveList * moves; // as each player has 3 undo/redo
};

// a data structure that holds all saved GAMES
// implemented as a linked list
struct savedGames{
  struct game thisGame;
  struct savedGames * nextGame;
};


// basic stack structure with a move as data
struct stack{
  struct move moves[9];
  int top;
};

// a structure that is used to implement undo/ redo features
// implemented with 2 stacks
struct undoRedo{
  struct stack mainStack;
  struct stack undoneMoves;
};


// stack for udo/ redo functions

// initializes the stack with starting position
void init_stack(struct stack *);
// initializes both stacks for redo and undo features
void init_undoRedo(struct undoRedo *);
// pushes a new item on the top of a stack
void push(struct stack *, struct move);
// rempves and returns
struct move * pop(struct stack *);
// adds a new move to the undoRedo structure
void unReAddNewMove(struct undoRedo *, struct move);
// undoes a move
struct move * undo(struct undoRedo *);
// redoes an undone move
struct move * redo(struct undoRedo *);


//gameHistory functions

//initializes the game history
void startGameHist(struct game *, char *, char *);
//adds a move to a move list
void addMove(struct moveList **, int, char, int);
// displays a game in the saved games option
void displayGame(struct game *);
// displays saved games
void savedGamesMenu();
// saves a game in a global list
void addSavedGame(struct savedGames **, struct game*);
// asks user if he/she wishes to save the game
void askToSave(struct savedGames **, struct game *);
// automaticaly replays a game
void replayGame(int);



// other functions

// initializes empty board
void initBoard(struct board*);
// draws position on the board. used at the begining of each game
void drawBoardPositions();
// draws the board with all pieces
void drawBoard(struct board);
// prints BOOM when a field with a bomb is selected
void printBoom();
// checks if a selected move is valid
int checkValidMove(struct board, int);
// checks the board to see if a winning combination is present
char checkWin(struct board);
// checks the board to see if all positions have been filled
int checkFinish(struct board);
// displays game options
void displayOptions();
// starts the application
void startApp();
// starts one of the 4 available games depending on choice, the chosen type
// of game can be then played multiple times
void playGame(char* , char* );
// start a single game of a chosen type
char playMatch(char *, char *, char*, char*);
// automatically selects an intelligent move
int generateMove(struct board);



int main(int argc, char const *argv[]) {
  // initializes a structure for storing saved games
  gameList = (struct savedGames **)malloc(sizeof(struct savedGames));
  *gameList = NULL;
  startApp();
  return 0;
}



// stack implementation

void init_stack(struct stack * st){
  st -> top = -1;
}

// initializes both stacks in undoRedo structure
void init_undoRedo(struct undoRedo * unRe){
  init_stack(&(unRe -> mainStack));
  init_stack(&(unRe -> undoneMoves));
}

// pushes a new item on the top of a stack
void push(struct stack * st, struct move mv){
  if(st->top < 8){
    st -> top++;
    st -> moves[st -> top] = mv;
  }
}

// removes and returnes a top item from a stack
struct move * pop(struct stack * st){
  if(st -> top >= 0){
    struct move * mv = &(st -> moves[st -> top]);
    st -> top--;
    return mv;
  }
  return NULL;
}

// adds a new move to the main stack of the undoRedo structure which then
// can be undone
void unReAddNewMove(struct undoRedo * unRe, struct move mv){
  push(&(unRe -> mainStack), mv);
  // whenever a new move is added the redo function is no longer available
  unRe -> undoneMoves.top = -1;
}

// undo function
struct move * undo(struct undoRedo * unRe){
  if(unRe -> mainStack.top >= 0){
    //popps a move from the main stack
    struct move * mv = pop(&(unRe -> mainStack));
    // and pushes it on the top of the other
    push(&(unRe -> undoneMoves), *mv);
    return mv;
  }
  return NULL;
}

// redo function, does the reverse of undo
struct move * redo(struct undoRedo * unRe){
  if(unRe -> undoneMoves.top >= 0){
    struct move * mv = pop(&(unRe -> undoneMoves));
    push(&(unRe -> mainStack), *mv);
    return mv;
  }
  return NULL;
}


// Game history functions implementation


// initializes the game structore
void startGameHist(struct game * game, char * plX, char * plO){
  strcpy(game -> playerX, plX);
  strcpy(game -> playerO, plO);
  struct moveList * moves;
  moves = NULL;
  game -> moves = moves;
}

// adds a new move to a move list
void addMove(struct moveList ** moves, int pos , char pl, int safe){
  struct move * tempMove;
  struct moveList * current;
  // when the structure is empty
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
  // when there are already items in it
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
}

// displays saved games
// checks who won the game and prints ** arount the name of the winner
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

// adds a new game to the saved games linked list
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

// asks the user if he/she wants to save the game
void askToSave(struct savedGames ** gameList, struct game * game){
  char answer;
  printf("WOULD YOU LIKE TO SAVE THE GAME? y/n:\n " );
  scanf(" %c", &answer);
  if(answer == 'y' || answer == 'Y'){
    addSavedGame(gameList, game);
  }
}


// displays a list of saved games
void savedGamesMenu(){
  if(*gameList != NULL){
    printf("   SAVED GAMES\n");
    int i = 1;// used to print the game number
    struct savedGames * games = *gameList;
    while (games -> nextGame != NULL) {
    printf("%d. ",i );
    displayGame(&(games -> thisGame));
    games = games -> nextGame;
    i++;
    }
    // prints the last game that wasnt printed in the loop
    printf("%d. ",i );
    displayGame(&(games -> thisGame));
    //ask the user for the game number to be replayed and if correct number
    // has been provided replays the game
    printf("ENTER THE NUMBER OF GHE GAME TO REPLAY, OR 0 TO EXIT: \n");
    int choice;
    scanf("%d", &choice);
    printf("\n");
    if(choice == 0){
      // DO NOTHING JUST EXIT

    }else if(choice > 0 && choice <= i){
      replayGame(choice);
    }else{
      printf("GAME NUMBER %d DOES NOT EXIST\n", choice );
      savedGamesMenu();
    }

  }else{
    printf("THERE ARE NO SAVED GAMES\n");
  }
}

// replays the game automatically giving descripton what move was made
void replayGame(int gameNo){
  int i; // used to iterate untill the selected game number
  struct savedGames * games = *gameList;
  for (i = 1; i < gameNo; i++) {
    games = games -> nextGame;
  }
  displayGame(&(games -> thisGame));
  struct moveList * moves = games -> thisGame.moves;
  struct board replayBoard;
  initBoard(&replayBoard);
  // each move is displayed separately with 2 sec pause
  while (moves -> link != NULL) {
    int pos = moves -> oneMove.position;
    char pl = moves -> oneMove.player;
    // this is for Tic Toc Boom game. displays BOOM if a position with a bomb
    // was selected
    if(moves -> oneMove.wasSafe == -1){
      printf("%s SELECTED POSITION %d\n", games -> thisGame.playerX, pos);
      printBoom();
      printf("BOOM!!! THERE WAS A BOMB IN THIS POSITION! NOW IT IS SAFE!\n");

    }else{

      printf("\n");
      // checks if the move made is an undo move and displays appropriate message
      if(replayBoard.position[pos-1] != ' '){
        replayBoard.position[pos-1] = ' ';
        if(pl == 'X'){
          printf("%s UNDONE THE MOVE AT POSITION %d\n", games -> thisGame.playerO, pos);
        }else{
          printf("%s UNDONE THE MOVE AT POSITION  %d\n", games -> thisGame.playerX, pos);
        }

      }else{
        replayBoard.position[pos-1] = pl;
        if(pl == 'X'){
          printf("%s SELECTED POSITION %d\n", games -> thisGame.playerX, pos);
        }else{
          printf("%s SELECTED POSITION %d\n", games -> thisGame.playerO, pos);
        }
      }
    }
    drawBoard(replayBoard);
    moves = moves -> link;
    sleep(2);
  }
  // prints the last move that wasn't printed in the loop
  replayBoard.position[moves -> oneMove.position -1] = moves -> oneMove.player;
  if(moves -> oneMove.player == 'X'){
    printf("%s SELECTED POSITION %d\n",
    games -> thisGame.playerX, moves -> oneMove.position);
  }else{
    printf("%s SELECTED POSITION %d\n",
    games -> thisGame.playerO, moves -> oneMove.position);
  }
  // prints who won the game
  char winner = games -> thisGame.winner;
  if(winner == 'X'){
    printf("%s WON THE GAME!\n", games -> thisGame.playerX);
  }else if(winner == 'O'){
    printf("%s WON THE GAME!\n", games -> thisGame.playerO);
  }else{
    printf("DRAW!\n");
  }

  drawBoard(replayBoard);
  sleep(2);
}

// othe functions implementation

// initializes empty board with a space char in each position
void initBoard(struct board* b){
  int i;
  for (i = 0; i < SIZE; i++) {
    b -> position[i]= ' ';
  }
}

// draws board positions, used at the beginning of each game
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


// draws board with all placed pieces, user after each move
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

// checks if a selected move is valid
int checkValidMove(struct board b, int move){
  // move can be between 1 and 11 (10 for undo, 11 for redo)
  if(move < 1 || move > 11){
    printf("Selected position must be betweeen 1 and 9 or 10 for undo, 11 for redo\n");
    return -1;
  }
  // checks if the selected position is free
  if(b.position[move -1] != ' ' && move < 10){
    printf("Position %d is already taken! \n", move);
    return -1;
  }else{
    return 1;
  }
}


// checks if either of the players won the game. If X won X will be returned,
// if O won O will be returned, otherwize d for draw will be returned
char checkWin(struct board b){
  // checks all 8 possible combinations
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

// checks if a game has been finished by checking for space char
int checkFinish(struct board b){
  int i;
  for (i = 0; i < SIZE; i++) {
    if(b.position[i] == ' '){
      return -1;
    }
  }
  return 1;
}

// prints the menu options
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


// prints BOOM
void printBoom(){

 printf(" ,-----.    ,-----.   ,-----.  ,--.   ,--.  \n");
 printf(" |  |) /_  '  .-.  ' '  .-.  ' |   `.'   |  \n");
 printf(" |  .-.  \\ |  | |  | |  | |  | |  |'.'|  |  \n");
 printf(" |  '--' / '  '-'  ' '  '-'  ' |  |   |  |  \n");
 printf(" `------'   `-----'   `-----'  `--'   `--'  \n");

}

// prints the game logo and dandles user input to displayed options
void startApp(){

  printf("\n");
  printf("\n");
  printf(" ,--------. ,--.  ,-----.     ,--------.   ,---.    ,-----.     ,--------.  ,-----.  ,------.  \n");
  printf(" '--.  .--' |  | '  .--./     '--.  .--'  /  O  \\  '  .--./     '--.  .--' '  .-.  ' |  .---'  \n");
  printf("    |  |    |  | |  |            |  |    |  .-.  | |  |            |  |    |  | |  | |  `--,   \n");
  printf("    |  |    |  | '  '--'\\        |  |    |  | |  | '  '--'\\        |  |    '  '-'  ' |  `---.  \n");
  printf("    `--'    `--'  `-----'        `--'    `--' `--'  `-----'        `--'     `-----'  `------'  \n");
  printf("\n");
  printf("\n");

  int option;

  // the menu will be displayed untill 6 for exit is selected
  while(option != 6){
    displayOptions();
    scanf("%d",  &option);

    switch (option) {
      case 1:
      // play basic 2 players game
      playGame("Basic", "human");
      break;

      case 2:
      //play basic game against computer
      playGame("Basic", "cpu");
      break;

      case 3:
      // play Tic Toc Boom 2 players
      playGame("TTB", "human");
      break;

      case 4:
      //play Tic Toc Boom against computer
      playGame("TTB", "cpu");
      break;

      case 5:
      // show saved games
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

// enters a selected game type that can be played many times
void playGame(char* type, char* against){
  char playerX[10];
  char playerO[10];
  printf("PLEASE ENTER PLAYER'S ONE NAME: ");
  scanf(" %[^\n]", playerX);
  // for player 2 it will only be asked if 2 players game was SELECTED
  // otherwise the 2nd player's name will be Computer
  if(strcmp(against, "human") == 0){
    printf("PLEASE ENTER PLAYER'S TWO NAME: ");
    scanf(" %[^\n]", playerO);
  }else if(strcmp(against, "cpu") == 0){
    strcpy(playerO,"Computer");
  }
  // keeps track of playerXwins
  int playerXwins = 0;
  int playerOwins = 0;


  char playAgain = 'n';
  char winner;
  do {
    // the single game is called here. X, O or d will be returned depending
    // on who won or if there was a draw
    winner = playMatch(playerX, playerO, type, against);

    if(winner == 'X'){
      playerXwins++;
    }else if(winner == 'O'){
      playerOwins++;
    }
    // displays the number of games won by each player in this game type
    printf("%s - %d VS %s - %d\n", playerX, playerXwins, playerO, playerOwins);
    printf("DO YOU WANT TO PLAY AGAIN? y/n\n");
    scanf(" %c", &playAgain);

  } while(playAgain == 'y' || playAgain == 'Y');

}


// plays a single match of a selected game type
char playMatch(char *playerXname, char *playerOname, char* type, char* against){
  // initializes the required data structures for a sigle game
  struct board myBoard;
  initBoard(&myBoard);
  struct undoRedo * unRe;
  unRe = (struct undoRedo *)malloc(sizeof(struct undoRedo));
  init_undoRedo(unRe);
  struct game * oneGame;
  oneGame = (struct game *)malloc(sizeof(struct game));
  startGameHist(oneGame, playerXname, playerOname);
  struct moveList ** recordedMoves = &(oneGame -> moves);


  //####bmobs
  int numberOfBombs;
  int bombs[3];

  // this code is for random generation of bombs. It will only be run if
  // Tic Toc Boom game type was selected
  if(strcmp(type,"TTB") == 0){

    do {
      printf("ENTER THE NUMBER OF BOMBS BETWEEN 1-3\n");
      scanf(" %d", &numberOfBombs);
      printf("\n");
    } while(numberOfBombs != 1 && numberOfBombs != 2 && numberOfBombs != 3);

    // randomly selects 3 positions
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

  }
  int b; // used to iterate through bombs
  char hasBomb;
  //####bmobs



  int i = 0; // turns, if even X makes a move, if uneven O makes a move
  int j = 0; //
  int startPos = 0; // used to display positions when all moves are undone
  // continues until gams has been finished
  while(checkFinish(myBoard) == -1) {
    hasBomb = 'n';
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

    // X makes a move when i is even
    if(i%2 == 0){
      do {
        pos = 0;
        printf("WHERE WOULD YOU LIKE TO PLACE X %s?", playerXname);
        scanf("%d", &pos);
      } while(checkValidMove(myBoard, pos) == -1);
      if(pos <10){
        // this code only runs when Tic Toc Boom was selected
        if(strcmp(type,"TTB") == 0){
          for (b = 0; b < numberOfBombs; b++) {
            if(bombs[b] == pos){
              printBoom();
              printf("THERE WAS A BOMB IN POSITION %d\n", pos );
              hasBomb = 'y'; // so the next part of code does not run
              bombs[b]=10; // deactivate the bmob
              addMove(recordedMoves, pos, 'X', -1);
            }
          }
          // only runs if there wasny a bom
          if(hasBomb == 'n'){
            myBoard.position[pos-1] = 'X';
            addMove(recordedMoves, pos, 'X', 1);
          }
          // code for classic game
        }else{
          myBoard.position[pos-1] = 'X';
          struct move * mv = (struct move *)malloc(sizeof(struct move));
          mv -> position = pos;
          mv -> player = 'X';
          unReAddNewMove(unRe, *mv);
          addMove(recordedMoves, pos, 'X', 1);
        }
        i++;
      }else{
        // logic for when undo was selected
        if(pos == 10 && i > startPos){
          if(unRe -> mainStack.top != -1){
            struct move * undoneMove = undo(unRe);
            myBoard.position[(undoneMove -> position)-1] = ' ';
            i--;
            pos = undoneMove -> position;
          }
        }
        // logic for redo
        if(pos == 11){
          if(unRe -> undoneMoves.top != -1){
            struct move * redoneMove = redo(unRe);
            myBoard.position[(redoneMove -> position)-1] = 'X';
            i++;
            pos = redoneMove -> position;
          }else{
            printf("NO MOVES TO REDO\n");
          }
        }
        addMove(recordedMoves, pos, 'X', 1);
      }
      // logic for when i is uneven and it's O's turn
    }else{

      do {
        printf("WHERE WOULD YOU LIKE TO PLACE O %s?", playerOname);
        if (strcmp(against, "cpu") == 0) {
          sleep(2);
          pos = generateMove(myBoard)+1;
        }else{
          scanf("%d", &pos);
        }
      } while(checkValidMove(myBoard, pos) == -1);

      if(pos < 10){
        if(strcmp(type,"TTB") == 0){
          for (b = 0; b < numberOfBombs; b++) {
            if(bombs[b] == pos){
              printBoom();
              printf("THERE WAS A BOMB IN POSITION %d\n", pos );
              hasBomb = 'y';
              bombs[b]=10; // deactivate the bmob
              addMove(recordedMoves, pos, 'O', -1);
            }
          }
          if(hasBomb == 'n'){
            myBoard.position[pos-1] = 'O';
            addMove(recordedMoves, pos, 'O', 1);
          }
        }else{
          myBoard.position[pos-1] = 'O';
          struct move * mv = (struct move *)malloc(sizeof(struct move));
          mv -> position = pos;
          mv -> player = 'O';
          unReAddNewMove(unRe, *mv);
          addMove(recordedMoves, pos, 'O', 1);
        }
        i++;
      }else{
        if(pos == 10 && i > startPos && strcmp(type,"Basic") == 0){
          if(unRe -> mainStack.top != -1){
            struct move * undoneMove = undo(unRe);
            myBoard.position[(undoneMove -> position)-1] = ' ';
            i--;
            pos = undoneMove -> position;
          }
        }
        if(pos == 11 && strcmp(type,"Basic") == 0){
          if(unRe -> undoneMoves.top != -1){
            struct move * redoneMove = redo(unRe);
            myBoard.position[(redoneMove -> position)-1] = 'O';
            i++;
            pos = redoneMove -> position;
          }else{
            printf("NO MORE MOVES TO REDO\n");
          }
        }
        addMove(recordedMoves, pos, 'O', 1);
      }

    }
    // displays the result of the game
    //displayGame(oneGame);
    // draws empty board once the game has been undone to the starting point
    if(i != startPos){
      drawBoard(myBoard);
    }

    // checks if the game was won and displays a message
    char winner = checkWin(myBoard);
    if(winner != 'd'){
      if(winner == 'X'){
        printf("%s WON THE GAME!\n", playerXname);
        oneGame -> winner = 'X';
        askToSave(gameList, oneGame);
        return winner;
      }else if(winner == 'O'){
        printf("%s WON THE GAME!\n", playerOname);
        oneGame -> winner = 'O';
        askToSave(gameList, oneGame);
        return winner;
      }

      //return checkWin(myBoard);
      //break;
    }

  }
  oneGame -> winner = 'd';
  askToSave(gameList, oneGame);
  return 'd'; // for draw
}


// generates a move for the computer player
int generateMove(struct board b){
  // if board is empty then select at randomn
  int isEmpty = 1;
  int i;
  for (i = 0; i < 9; i++) {
    if(b.position[i] != ' '){
      isEmpty = -1;
    }
  }
  if(isEmpty == 1){
    srand(time(0)); // seed will be set to time to allow randomness
    int randomPosition = rand()%8;
    return randomPosition;
  }
  //finish game if possible
  // checks if there are combinations that will allow to win the game by puttingthe
  // final piece
  if(b.position[0] == b.position[1] && b.position[2] == ' ' && b.position[1] != ' '){
    return 2;
  }else if (b.position[1] == b.position[2] && b.position[0] == ' ' && b.position[1] == 'O') {
    return 0;
  }else if (b.position[1] == b.position[2] && b.position[0] == ' ' && b.position[1] == 'O') {
    return 0;
  }else if (b.position[3] == b.position[4] && b.position[5] == ' ' && b.position[3] == 'O') {
    return 5;
  }else if (b.position[4] == b.position[5] && b.position[3] == ' ' && b.position[4] == 'O') {
    return 3;
  }else if (b.position[6] == b.position[7] && b.position[8] == ' ' && b.position[6] == 'O') {
    return 8;
  }else if (b.position[7] == b.position[8] && b.position[6] == ' ' && b.position[7] == 'O') {
    return 6;
  }else if (b.position[0] == b.position[2] && b.position[1] == ' ' && b.position[0] == 'O') {
    return 1;
  }else if (b.position[3] == b.position[5] && b.position[4] == ' ' && b.position[3] == 'O') {
    return 4;
  }else if (b.position[6] == b.position[8] && b.position[7] == ' ' && b.position[6] == 'O') {
    return 7;
  }else if (b.position[0] == b.position[3] && b.position[6] == ' ' && b.position[0] == 'O') {
    return 6;
  }else if (b.position[3] == b.position[6] && b.position[0] == ' ' && b.position[3] == 'O') {
    return 0;
  }else if (b.position[0] == b.position[6] && b.position[3] == ' ' && b.position[0] == 'O') {
    return 3;
  }else if (b.position[1] == b.position[4] && b.position[7] == ' ' && b.position[1] == 'O') {
    return 7;
  }else if (b.position[1] == b.position[7] && b.position[4] == ' ' && b.position[1] == 'O') {
    return 4;
  }else if (b.position[4] == b.position[7] && b.position[1] == ' ' && b.position[4] == 'O') {
    return 1;
  }else if (b.position[2] == b.position[5] && b.position[8] == ' ' && b.position[2] == 'O') {
    return 8;
  }else if (b.position[5] == b.position[8] && b.position[2] == ' ' && b.position[5] == 'O') {
    return 2;
  }else if (b.position[2] == b.position[8] && b.position[5] == ' ' && b.position[2] == 'O') {
    return 5;
  }else if (b.position[0] == b.position[4] && b.position[8] == ' ' && b.position[0] == 'O') {
    return 8;
  }else if (b.position[4] == b.position[8] && b.position[0] == ' ' && b.position[4] == 'O') {
    return 0;
  }else if (b.position[0] == b.position[8] && b.position[4] == ' ' && b.position[0] == 'O') {
    return 4;
  }else if (b.position[2] == b.position[4] && b.position[6] == ' ' && b.position[2] == 'O') {
    return 6;
  }else if (b.position[4] == b.position[6] && b.position[2] == ' ' && b.position[4] == 'O') {
    return 2;
  }else if (b.position[2] == b.position[6] && b.position[4] == ' ' && b.position[2] == 'O') {
    return 4;
  }
  //block opponent from winning. As above checks for combinations of pieces and blocks the
  // oponent
  if(b.position[0] == b.position[1] && b.position[2] == ' ' && b.position[1] != ' '){
    return 2;
  }else if (b.position[1] == b.position[2] && b.position[0] == ' ' && b.position[1] != ' ') {
    return 0;
  }else if (b.position[1] == b.position[2] && b.position[0] == ' ' && b.position[1] != ' ') {
    return 0;
  }else if (b.position[3] == b.position[4] && b.position[5] == ' ' && b.position[3] != ' ') {
    return 5;
  }else if (b.position[4] == b.position[5] && b.position[3] == ' ' && b.position[4] != ' ') {
    return 3;
  }else if (b.position[6] == b.position[7] && b.position[8] == ' ' && b.position[6] != ' ') {
    return 8;
  }else if (b.position[7] == b.position[8] && b.position[6] == ' ' && b.position[7] != ' ') {
    return 6;
  }else if (b.position[0] == b.position[2] && b.position[1] == ' ' && b.position[0] != ' ') {
    return 1;
  }else if (b.position[3] == b.position[5] && b.position[4] == ' ' && b.position[3] != ' ') {
    return 4;
  }else if (b.position[6] == b.position[8] && b.position[7] == ' ' && b.position[6] != ' ') {
    return 7;
  }else if (b.position[0] == b.position[3] && b.position[6] == ' ' && b.position[0] != ' ') {
    return 6;
  }else if (b.position[3] == b.position[6] && b.position[0] == ' ' && b.position[3] != ' ') {
    return 0;
  }else if (b.position[0] == b.position[6] && b.position[3] == ' ' && b.position[0] != ' ') {
    return 3;
  }else if (b.position[1] == b.position[4] && b.position[7] == ' ' && b.position[1] != ' ') {
    return 7;
  }else if (b.position[1] == b.position[7] && b.position[4] == ' ' && b.position[1] != ' ') {
    return 4;
  }else if (b.position[4] == b.position[7] && b.position[1] == ' ' && b.position[4] != ' ') {
    return 1;
  }else if (b.position[2] == b.position[5] && b.position[8] == ' ' && b.position[2] != ' ') {
    return 8;
  }else if (b.position[5] == b.position[8] && b.position[2] == ' ' && b.position[5] != ' ') {
    return 2;
  }else if (b.position[2] == b.position[8] && b.position[5] == ' ' && b.position[2] != ' ') {
    return 5;
  }else if (b.position[0] == b.position[4] && b.position[8] == ' ' && b.position[0] != ' ') {
    return 8;
  }else if (b.position[4] == b.position[8] && b.position[0] == ' ' && b.position[4] != ' ') {
    return 0;
  }else if (b.position[0] == b.position[8] && b.position[4] == ' ' && b.position[0] != ' ') {
    return 4;
  }else if (b.position[2] == b.position[4] && b.position[6] == ' ' && b.position[2] != ' ') {
    return 6;
  }else if (b.position[4] == b.position[6] && b.position[2] == ' ' && b.position[4] != ' ') {
    return 2;
  }else if (b.position[2] == b.position[6] && b.position[4] == ' ' && b.position[2] != ' ') {
    return 4;
  }

  // if none of the above found a suitable move looks for the first piece and tries to
  // add another one next to it

  for (i = 0; i < 9; i++) {
    if(b.position[i] == 'O'){
      if(i == 0){
        if(b.position[i+1] == ' '){
          return i+1;
        }else if(b.position[i+3] == ' '){
          return i+3;
        }else if(b.position[i+4] == ' '){
          return i+4;
        }
      }
      else if(i == 1){
        if(b.position[i+1] == ' '){
          return i+1;
        }else if(b.position[i+3] == ' '){
          return i+3;
        }else if(b.position[i-1] == ' '){
          return i-1;
        }
      }
      else if(i == 2){
        if(b.position[i+2] == ' '){
          return i+2;
        }else if(b.position[i+3] == ' '){
          return i+3;
        }else if(b.position[i-1] == ' '){
          return i-1;
        }
      }
      else if(i == 3){
        if(b.position[i+1] == ' '){
          return i+1;
        }else if(b.position[i+3] == ' '){
          return i+3;
        }else if(b.position[i-3] == ' '){
          return i-3;
        }
      }
      else if(i == 4){
        if(b.position[i+1] == ' '){
          return i+1;
        }else if(b.position[i+3] == ' '){
          return i+3;
        }else if(b.position[i+4] == ' '){
          return i+4;
        }
      }
      else if(i == 5){
        if(b.position[i-1] == ' '){
          return i-1;
        }else if(b.position[i+3] == ' '){
          return i+3;
        }else if(b.position[i-3] == ' '){
          return i-3;
        }
      }
      else if(i == 6){
        if(b.position[i+1] == ' '){
          return i+1;
        }else if(b.position[i-3] == ' '){
          return i-3;
        }else if(b.position[i-2] == ' '){
          return i-2;
        }
      }
      else if(i == 7){
        if(b.position[i+1] == ' '){
          return i+1;
        }else if(b.position[i-3] == ' '){
          return i-3;
        }else if(b.position[i-1] == ' '){
          return i-1;
        }
      }
      else if(i == 8){
        if(b.position[i-1] == ' '){
          return i-1;
        }else if(b.position[i-3] == ' '){
          return i-3;
        }else if(b.position[i-4] == ' '){
          return i-4;
        }
      }
    }
  }

  // if a sutable move has not been found in the above, selects a valid move at random
  int isValid = -1;
  while (isValid == -1) {
    srand(time(0)); // seed will be set to time to allow randomness
    int randomPos = rand()%8;
    if(b.position[randomPos] == ' '){
      isValid = 1;
      return randomPos;
    }
  }
  return 1;
}
