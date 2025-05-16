#include "proc.h"
#include "memory.h"

static void *pf = NULL;

void* new_page(void) {
  assert(pf < (void *)_heap.end);
  void *p = pf;
  pf += PGSIZE;
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uint32_t new_brk) {
  if (current->cur_brk == 0) {
    current->cur_brk = current->max_brk = new_brk;
  }
  else {
    if (new_brk > current->max_brk) {
      // 计算起始地址和结束地址对应的页边界
      uintptr_t start_page = PGROUNDUP(current->max_brk);
      uintptr_t end_page = PGROUNDUP(new_brk);
      // 如果new_brk不是页对齐的，需要额外映射一页
      if ((new_brk & (PGSIZE-1)) != 0) {
        end_page += PGSIZE;
      }
      // 按页映射内存区域
      for (uintptr_t va = start_page; va < end_page; va += PGSIZE) {
        void *pa = new_page();
        _map(&current->as, (void*)va, pa);
      }
      current->max_brk = new_brk;
    }
    current->cur_brk = new_brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _pte_init(new_page, free_page);
}
