#include "common.h"
#include "syscall.h"

int sys_write(int fd,void *buf,size_t len){
  if(fd == 1 || fd == 2){
    Log("sys_write called: fd=%d, len=%d", fd, len);
    for(int i = 0; i < len; ++i){
      _putc(*((char *)buf + i));
    }
    return len;
  }
  return -1;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  switch (a[0]) {
    case SYS_none:
      SYSCALL_ARG1(r) = 1;
      break;
    case SYS_exit:
      _halt(a[1]);
      break;
    case SYS_write:
      Log("SYS_write received: fd=%d, buf=0x%x, len=%d", a[1], a[2], a[3]);
      SYSCALL_ARG1(r) = sys_write(a[1], (void*)a[2], a[3]);
      break;
    case SYS_brk:
      return 0; // 总是成功
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
