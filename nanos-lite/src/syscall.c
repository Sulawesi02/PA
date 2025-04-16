#include "common.h"
#include "syscall.h"

_RegSet *sys_exit(_RegSet *r) {
  _halt(0);
  return NULL;
}

_RegSet *sys_write(_RegSet *r) {
  printf("sys_write");
  // uintptr_t a[4];
  // a[0] = SYSCALL_ARG1(r);
  // a[1] = SYSCALL_ARG2(r);
  // a[2] = SYSCALL_ARG3(r);
  // a[3] = SYSCALL_ARG4(r);
  // int fd = a[1];
  // char *buf = (char *)a[2];
  // int len = a[3];
  // int ret = 0;
  // if (fd == 1 || fd == 2) {
  //   while (len > 0) {
  //     _putc(*buf);
  //     buf++;
  //     len--;
  //     ret++;
  //   }
  //   SYSCALL_ARG1(r) = ret;
  //   return len;
  // }
  return NULL;
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
      return sys_exit(r);
      break;
    case SYS_write:
      return sys_write(r);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
