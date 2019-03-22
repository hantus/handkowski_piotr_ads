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


struct stack{
  struct move oneMove;
  struct stack * next;
  struct stack * prev;
};



struct undoRedo{
  struct stack * mainStack;
  struct stack * undoneMoves;
};
// a different name for push as it puts the item on top

void push(struct stack **, int, char, int);
struct stack * pop(struct stack *);
void displayStack(struct stack *); // just for testing
void mainStackPush(struct undoRedo * , int, char, int);
void undoneMovesPush(struct undoRedo * , int, char, int);


int main(int argc, char const *argv[]) {

  // struct game myGame;
  // startGame(&myGame, "Peter", "Yola");
  // printf("%s plays with %s\n", myGame.playerX, myGame.playerO);
  // struct stack ** gameMoves;
  // gameMoves = &(myGame.moves);
  struct undoRedo gameStack;
  //mainSt = &(gameStack.mainStack);
  //undoneSt = &(gameStack.undoneMoves);
  mainStackPush(&gameStack, 5, 'X', 1);
  mainStackPush(&gameStack, 1, 'O', 1);
  mainStackPush(&gameStack, 6, 'X', 1);
  mainStackPush(&gameStack, 4, 'O', 1);
  displayStack(gameStack.mainStack);


  return 0;
}

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

  printf("Move added\n" );
}


void mainStackPush(struct undoRedo* unRedo, int pos, char pl, int safe){
  struct stack ** mainSt;
  mainSt = &(unRedo -> mainStack);
  push(mainSt, pos, pl, safe);
}

void undoneMovesPush(struct undoRedo* unRedo, int pos, char pl, int safe){
  struct stack ** mainSt;
  mainSt = &(unRedo -> mainStack);
  push(mainSt, pos, pl, safe);
}

struct stack * pop(struct stack * st){
  struct stack * top;
  top = st;
  while(top -> next != NULL){
    top = top ->next;
  }
  return top;
}
