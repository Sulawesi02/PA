#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  memcpy(buf, dispinfo + offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  // 每个像素4字节
  int x = (offset / 4) % _screen.width;// 计算列号
  int y = (offset / 4) / _screen.width;// 计算行号
  int w = len / 4;// 像素宽度
  int h = 1;// 像素高度
  _draw_rect((uint32_t *)buf, x, y, w, h);
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n",_screen.width,_screen.height);
}
