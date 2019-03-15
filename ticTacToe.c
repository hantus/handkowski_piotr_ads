#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE 9

struct board{
  char position[SIZE];
};

// moves will be recorded in the history for replaying a game and on a stack
// for undo/redo
struct move{
  int position;
  char player;
};

struct gameHistory{
  struct move moves[22]; // as each player has 3 undo/redo
};

// basic stack structure
struct stack{
  struct move moves[22];
  int top;
};

struct undoRedo{
  struct stack mainStack;
  struct stack undoeMoves;
};

//gameHistory


// stack
void init_stack(struct stack*);
void push(struct stack*, struct move);
struct move* pop(struct stack*);
void display(struct stack);

// undoRedo
void init_undoRedo(struct undoRedo*);
void stackAddMove(struct undoRedo*, struct move);
void undo(struct undoRedo*);



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



  // struct undoRedo unRe;
  // init_undoRedo(&unRe);
  // struct move m;
  // m.player = 'X';
  // m.position = 3;
  // stackAddMove(&unRe, m);
  // struct move m2;
  // m2.player = 'O';
  // m2.position = 5;
  // stackAddMove(&unRe, m2);
  // printf("Main Stack\n" );
  // display(unRe.mainStack);
  // printf("Undone Moves\n" );
  // display(unRe.undoeMoves);
  //
  // undo(&unRe);
  // printf("Item popped\n" );
  // printf("Main Stack\n" );
  // display(unRe.mainStack);
  // printf("Undone Moves\n" );
  // display(unRe.undoeMoves);

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


  int i = 0;
  int j = 0; //
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

    if(i%2 == 0){
      do {
        printf("Where would you like to place X %s?", playerXname);
        scanf("%d", &pos);
      } while(checkValidMove(myBoard, pos) == -1);
      myBoard.position[pos-1] = 'X';

    }else{
      do {
        printf("Where would you like to place O %s?", playerOname);
        scanf("%d", &pos);
      } while(checkValidMove(myBoard, pos) == -1);
      myBoard.position[pos-1] = 'O';
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
  if(move < 1 && move > 9){
    printf("Selected position must be betweeen 1 and 9\n");
    return -1;
  }
  if(b.position[move -1] != ' '){
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
void init_stack(struct stack* s){
  s-> top = -1;
}

void push(struct stack* s, struct move m){
  s-> top++;
  s-> moves[s-> top] = m;
}

struct move* pop(struct stack* s){
  if(s-> top == -1){
    return NULL;
  }
  struct move* m;
  m = &(s-> moves[s-> top]);
  s-> top--;
  return m;
}

void display(struct stack s){
  int i;
  for (i = 0; i <= s.top; i++) {
    printf("Move number %d\n", i+1 );
    printf("Player %c position %d\n", s.moves[i].player, s.moves[i].position );
  }
}




void stackAddMove(struct undoRedo * s, struct move m){
  push(&(s->mainStack), m);
}

// undo undoRedo
void init_undoRedo(struct undoRedo * unRe){
  init_stack(&(unRe-> mainStack));
  init_stack(&(unRe-> undoeMoves));
}


void undo(struct undoRedo * unRe){
  if(unRe -> mainStack.top != -1){
    struct move* m;
    m = pop(&(unRe-> mainStack));
    push(&(unRe-> undoeMoves), *m);
  }
}
