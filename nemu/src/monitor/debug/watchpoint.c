#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void print_watchpoints() {
  WP *wp = head;
  
  if (wp == NULL) {
    printf("没有监视点\n");
    return;
  }

  printf("监视点信息：\n");
  printf("编号\t表达式\t\t值\n");
  
  while (wp != NULL) {
    printf("%d\t%s\t\t0x%08x\n", wp->NO, wp->expr, wp->old_val);
    wp = wp->next;
  }
}

