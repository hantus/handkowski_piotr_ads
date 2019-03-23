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
struct move * undo(struct undoRedo *);
struct move * redo(struct undoRedo *);
void resetUndoneStack(struct undoRedo *); // used whenever a new move is made


int main(int argc, char const *argv[]) {

  struct undoRedo gameStack;
  undo(&gameStack);
  mainStackPush(&gameStack, 5, 'X', 1);
  mainStackPush(&gameStack, 1, 'O', 1);
  mainStackPush(&gameStack, 6, 'X', 1);
  mainStackPush(&gameStack, 4, 'O', 1);
  displayStack(gameStack.mainStack);
  undo(&gameStack);
  undo(&gameStack);
  printf("Now two undone moves\n" );
  printf("the Main stack is:\n" );
  displayStack(gameStack.mainStack);
  printf("And the undone moves:\n" );
  displayStack(gameStack.undoneMoves);
  redo(&gameStack);
  printf("One redo\n" );
  displayStack(gameStack.mainStack);
  printf("And the undone moves:\n" );
  displayStack(gameStack.undoneMoves);
  resetUndoneStack(&gameStack);

  printf("after reset main\n" );
  displayStack(gameStack.mainStack);
  printf("and undo\n" );
  displayStack(gameStack.undoneMoves);
  undo(&gameStack);
  printf("after reset main\n" );
  undo(&gameStack);
  undo(&gameStack);
  undo(&gameStack);
  printf("Main stack:\n" );
  displayStack(gameStack.mainStack);
  printf("and undo\n" );
  displayStack(gameStack.undoneMoves);


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


struct stack * pop(struct stack * top){

  if(top == NULL){
    return NULL;
  }

  while(top -> next != NULL){
    top = top ->next;
  }

  if(top -> prev != NULL){
    top -> prev -> next = NULL;
  }else{
    printf("in the last stack\n" );
    struct stack * lastStack;
    lastStack = (struct stack *)malloc(sizeof(struct stack));
    *lastStack = *top;
    free(top);
    top  = (struct stack *)malloc(sizeof(struct stack));
    top = NULL;
    return lastStack;
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
  unRe -> undoneMoves = NULL;
}