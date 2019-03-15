#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
char playSingleTwoPlayersMatch();
void playTwoPlayersGame();

int main(int argc, char const *argv[]) {
  playTwoPlayersGame();



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

  while(option != 3){
    displayOptions();
    scanf("%d",  &option);

    switch (option) {
      case 1:
      playSingleTwoPlayersMatch();
      break;

      case 2:
      printf("Play against Computer\n");
      break;

      case 3:
      printf("GOOD BYE!!\n");
      break;

      default :
      printf("SELECTED OPTION IS INVALID!\n\n");
    }
  }
}


void playTwoPlayersGame(){
  char playerX[10];
  char playerO[10];
  printf("Please enter player's one name: ");
  scanf(" %[^\n]", playerX);
  printf("Please enter player's two name: ");
  scanf(" %[^\n]", playerO);
  int playerXwins = 0;
  int playerOwins = 0;

  char playAgain = 'n';
  do {
    char winner = playSingleTwoPlayersMatch();
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



char playSingleTwoPlayersMatch(){
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
        printf("Where would you like to place X?");
        scanf("%d", &pos);
      } while(checkValidMove(myBoard, pos) == -1);
      myBoard.position[pos-1] = 'X';

    }else{
      do {
        printf("Where would you like to place O?");
        scanf("%d", &pos);
      } while(checkValidMove(myBoard, pos) == -1);
      myBoard.position[pos-1] = 'O';
    }
    drawBoard(myBoard);

    if(checkWin(myBoard) != 'd'){
      printf("%c won the game!\n", checkWin(myBoard));
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
  printf("************************************************\n");
  printf("*  WHAT WOULD YOU LIKE TO DO?                  *\n");
  printf("*                                              *\n");
  printf("*  PRESS 1 TO PLAY PLAYER 1 VS PLAYER 2 GAME   *\n");
  printf("*  PRESS 2 TO PLAY PLAYER 1 VS CPU GAME        *\n");
  printf("*  PRESS 3 TO EXIT                             *\n");
  printf("************************************************\n");
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
