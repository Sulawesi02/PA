#include "fs.h"

typedef struct {
  char *name;         // 文件名
  size_t size;        // 文件大小
  off_t disk_offset;  // 文件在ramdisk中的偏移量
  off_t open_offset;  // 文件被打开之后的读写指针
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern void ramdisk_write(const void *buf, off_t offset, size_t len);
extern void fb_write(const void *buf, off_t offset, size_t len);
extern void dispinfo_read(void *buf, off_t offset, size_t len);
extern size_t events_read(void *buf, size_t len);


void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size = _screen.height * _screen.width * 4;
  //Log("FD_FB size=%d",file_table[FD_FB].size);
}

size_t fs_filesz(int fd) {
  assert(fd >= 0 && fd < NR_FILES);
  return file_table[fd].size;
}

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < NR_FILES; i++) {
    if (strcmp(file_table[i].name, pathname) == 0) {
      file_table[i].open_offset = 0;//设置读写指针到文件开头
      return i;
    }
  }
  return -1;
}

ssize_t fs_read(int fd, void *buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);
  if (file_table[fd].open_offset + len > fs_filesz(fd)) {
    len = fs_filesz(fd) - file_table[fd].open_offset;// 偏移量不能超过文件大小
  }
  switch(fd) {
    case FD_STDIN:
    case FD_STDOUT:
    case FD_STDERR:
    case FD_FB:
      return 0;
    case FD_EVENTS:// 事件
      len = events_read(buf, len);
      break;
    case FD_DISPINFO:// 屏幕信息
      dispinfo_read(buf, file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
      break;
    default:
      ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
  }
  return len;
}

ssize_t fs_write(int fd, const void *buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);
  if (file_table[fd].open_offset + len > fs_filesz(fd)) {
    len = fs_filesz(fd) - file_table[fd].open_offset;// 偏移量不能超过文件大小
  }
  switch(fd) {
    case FD_STDIN:
      return 0;
    case FD_STDOUT:
    case FD_STDERR:
      for (int i = 0; i < len; i++) {
        _putc(((char *)buf)[i]);
      }
      break;
    case FD_FB:// 显存
      fb_write(buf, file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
      break;
    default:
      ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
      break;
  }
  return len;
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  assert(fd >= 0 && fd < NR_FILES);
  if (fd < 3){
    return 0;
  }
  switch (whence) {
    case SEEK_SET:// 从文件开头开始偏移
      file_table[fd].open_offset = offset;
      break;
    case SEEK_CUR:// 从当前位置开始偏移
      file_table[fd].open_offset += offset;
      break;
    case SEEK_END:// 从文件末尾开始偏移
      file_table[fd].open_offset = fs_filesz(fd) + offset;
      break;
    default:
      return -1;
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  assert(fd >= 0 && fd < NR_FILES);
  if (fd < 3){
    return 0;
  }
  return 0;
}