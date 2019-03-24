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
  struct move moves[9];
  int top;
};

struct undoRedo{
  struct stack mainStack;
  struct stack undoneMoves;
};



// a different name for push as it puts the item on top

void init_stack(struct stack *);
void init_undoRedo(struct undoRedo *);
void push(struct stack *, struct move);
struct move * pop(struct stack *);
void display(struct undoRedo *);
void unReAddNewMove(struct undoRedo *, struct move);
struct move * undo(struct undoRedo *);
struct move * redo(struct undoRedo *);



int main(int argc, char const *argv[]) {

  struct undoRedo st;
  init_undoRedo(&st);
  display(&st);
  struct move mv;
  mv.player = 'X';
  mv.position = 5;
  unReAddNewMove(&st, mv);
  struct move mv2;
  mv2.player = 'O';
  mv2.position = 3;
  unReAddNewMove(&st, mv2);
  struct move mv3;
  mv3.player = 'X';
  mv3.position = 2;
  unReAddNewMove(&st, mv3);
  //display(&st);
  struct move mv5;
  mv5.player = 'X';
  mv5.position = 4;
  unReAddNewMove(&st, mv5);
  struct move mv12;
  mv12.player = 'O';
  mv12.position = 9;
  unReAddNewMove(&st, mv12);
  struct move mv15;
  mv15.player = 'X';
  mv15.position = 4;
  unReAddNewMove(&st, mv15);
  struct move mv112;
  mv112.player = 'O';
  mv112.position = 99;
  unReAddNewMove(&st, mv112);
  struct move mv121;
  mv121.player = 'O';
  mv121.position = 9;
  unReAddNewMove(&st, mv121);
  struct move mv151;
  mv151.player = 'X';
  mv151.position = 4;
  unReAddNewMove(&st, mv151);
  struct move mv1121;
  mv1121.player = 'O';
  mv1121.position = 999;
  unReAddNewMove(&st, mv1121);
  display(&st);
  undo(&st);
  undo(&st);
  undo(&st);
  printf("after 3 undos\n" );
  display(&st);
  redo(&st);
  redo(&st);
  printf("after 2 redos\n" );
  display(&st);
  redo(&st);
  printf("after additional redo\n" );
  display(&st);

  struct move mv11121;
  mv11121.player = 'O';
  mv11121.position = 999;
  unReAddNewMove(&st, mv11121);
  display(&st);



  return 0;
}


// ############

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
