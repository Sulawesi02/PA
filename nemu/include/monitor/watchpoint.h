#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[100];
  uint32_t val;

} WP;

#endif
