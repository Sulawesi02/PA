#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

int current_game = 0; // 当前游戏的进程号
size_t events_read(void *buf, size_t len) {
  int key = _read_key();
  bool is_down = false;
  if (key & 0x8000) {
    key ^= 0x8000;
    is_down = true;
  }
  if(key == _KEY_NONE){
    sprintf(buf, "t %d\n", _uptime());//时钟事件
  }
  else {
    sprintf(buf, "%s %s\n", is_down?"kd":"ku", keyname[key]);//按键事件
    if(key == 13 && is_down){// 按下F12，切换游戏
      current_game = (current_game == 0 ? 1 : 0);
      Log("current_game = %d", current_game);
    }  
  }
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  memcpy(buf, dispinfo + offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  int index = offset / 4;
  int x1 = index % _screen.width;// 开始的列号
  int y1 = index / _screen.width;// 开始的行号
  int x2 = (index + len / 4) % _screen.width;// 结束的列号
  int y2 = (index + len / 4) / _screen.width;// 结束的行号
  if(y2 == y1) {
    _draw_rect((uint32_t *)buf, x1, y1, x2 - x1, 1);
    return;
  }
  else if(y2 - y1 == 1) {
    _draw_rect((uint32_t *)buf, x1, y1, _screen.width - x1, 1);
    _draw_rect((uint32_t *)buf + 4 * (_screen.width - x1), 0, y2, x2, 1);
    return;
  }
  else{
    _draw_rect((uint32_t *)buf, x1, y1, _screen.width - x1, 1);
    _draw_rect((uint32_t *)buf + 4*(_screen.width - x1), 0, y1 + 1, _screen.width, y2 - y1 - 2);
    _draw_rect((uint32_t *)buf + 4*(_screen.width - x1 + _screen.width*(y2 - y1 - 2)), 0, y2, x2, 1);
    return;
  }
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n",_screen.width,_screen.height);
}
