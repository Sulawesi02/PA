#ifndef RAMDISK_H
#define RAMDISK_H


// 声明外部变量
extern uint8_t ramdisk_start[], ramdisk_end[];

// 声明宏
#define RAMDISK_SIZE ((size_t)(ramdisk_end - ramdisk_start))

// 声明函数
void ramdisk_read(void *buf, off_t offset, size_t len);
void ramdisk_write(const void *buf, off_t offset, size_t len);

#endif