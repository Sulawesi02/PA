#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

int current_game = 0; // 当前游戏的进程号
_RegSet* schedule(_RegSet *prev) {
  if(current != NULL){
    current->tf = prev;
  }
  else{
    current = &pcb[current_game];
  }
  // current = &pcb[0];
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  static int count = 0;
  static const int max_count = 1000;
  if(current == &pcb[current_game])
  {
      if(current_game == 0)
        Log("run pal %d\n",count);
      else if(current_game == 2)
        Log("run videotest %d\n",count);
       count++;
  }
  else{
    current = &pcb[current_game];
  }
  if(count == max_count){
    current = &pcb[1];
    Log("run hello \n");
    count = 0;
  }
  
  _switch(&current->as);
  return current->tf;
}