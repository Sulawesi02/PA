#ifndef RAMDISK_H
#define RAMDISK_H

extern uint8_t ramdisk_start[], ramdisk_end[];

#define RAMDISK_SIZE ((size_t)(ramdisk_end - ramdisk_start))

void ramdisk_read(void *buf, off_t offset, size_t len);

#endif