#include "common.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

// extern uint8_t ramdisk_start,ramdisk_end;
// #define RAMDISK_SIZE ((&ramdisk_end)-(&ramdisk_start))
// extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern void* new_page(void);

size_t fs_filesz(int fd);
int fs_open(const char *pathname, int flags, int mode);
ssize_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  // ramdisk_read(DEFAULT_ENTRY, 0, RAMDISK_SIZE);
  int fd = fs_open(filename, 0, 0);
  if(fd == -1){
    Log("open file failed");
    return 0;
  }
  // Log("filename=%s,fd=%d",filename,fd);
  // fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
  int size = fs_filesz(fd);// 获取文件大小
  int page_num = (size + PGSIZE - 1) / PGSIZE;// 页面数

  void *pa = NULL;
  void *va = DEFAULT_ENTRY;
  for(int i = 0; i < page_num; i++){
    pa = new_page();
    _map(as, va, pa);
    fs_read(fd, pa, PGSIZE);
    va += PGSIZE;
  }
  
  fs_close(fd);

  return (uintptr_t)DEFAULT_ENTRY;
}
