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
  }

  if (wp == head) {
    head = wp->next;
  } else {
    WP* prev = head;
    while (prev->next!= wp) {
      prev = prev->next;
    }
    prev->next = wp->next;
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
    printf("%d\t%s\t\t%d\n", wp->NO, wp->expr, wp->val);
    wp = wp->next;
  }
}

bool check_watchpoints(){
  WP *wp = head;
  bool is_changed = false;
  bool success = false;
  while (wp) {
    uint32_t new_val = expr(wp->expr, &success);
    if (!success) {
      printf("表达式求值失败\n");
      continue;
    } 
    if (new_val != wp->val) {
      printf("监视点发生变化：\n");
      printf("编号\t表达式\t\t旧值\t\t新值\n");
      printf("%d\t%s\t\t%d\t\t%d\n", wp->NO, wp->expr, wp->val, new_val);
      is_changed = true;
      wp->val = new_val;
    }
    wp = wp->next;
  }
  return is_changed;
}
