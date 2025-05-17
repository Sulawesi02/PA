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

extern int current_game;
_RegSet* schedule(_RegSet *prev) {
  current->tf = prev;
  // current = &pcb[0];
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  static int count = 0;
  static const int max_count = 1000;

  if(current == &pcb[current_game]){
    //打印当前游戏名以及运行次数
    Log("game: %s, count: %d", current_game == 0? "pal" : "videotest", count);
    count++;
    if(count == max_count){
      count = 0;
      current = &pcb[1];
      Log("run hello \n");
    }
  }
  else if(current == &pcb[1]){
    current = &pcb[current_game];
  }

  _switch(&current->as);
  return current->tf;
}
