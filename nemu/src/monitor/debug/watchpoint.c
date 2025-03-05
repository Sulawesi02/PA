#include "monitor/watchpoint.h"
#include "monitor/expr.h"

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

WP* new_wp(){
	if(free_ == NULL){
		printf("wp_pool 已满\n");
		assert(0);
	}
	WP* wp = free_;
	free_ = free_ -> next;

	wp -> next = head;
	head = wp;

  return head;
}

void free_wp(int n){
  WP* wp = head;
  if(wp == NULL){
    printf("没有监视点\n");
    assert(0);
  }
  while (wp) {
    if (wp->NO == n) {
      break;
    }
    wp = wp->next;
    if(wp == NULL){
      printf("编号为%d的监视点不存在\n", n);
      assert(0);
      }
  }

  wp->next = free_;
  free_ = wp;
}

void print_watchpoints() {
  WP *wp = head;
  
  if (wp == NULL) {
    printf("没有监视点\n");
    assert(0);
  }

  printf("监视点信息：\n");
  printf("编号\t表达式\t\t值\n");
   
  while (wp != NULL) {
    printf("%d\t%s\t%d\n", wp->NO, wp->expr, wp->val);
    wp = wp->next;
  }
}

