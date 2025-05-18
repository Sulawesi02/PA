#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
    //return 0;
    int key=_read_key();
    bool down = false;
    if (key & 0x8000) {
          key ^= 0x8000;
          down = true;
    }
    char temp[20];
    if(down && key == _KEY_F12) {
        extern void switch_current_game();
        switch_current_game();
        Log("key down:_KEY_F12, switch current game!");
     }
    if (key != _KEY_NONE)
    {
      if(down){
          sprintf(temp,"kd %s\n",keyname[key]);}
      else{
          sprintf(temp,"ku %s\n",keyname[key]);}
    }
    else{
      sprintf(temp,"t %d\n",_uptime());}
  
    if(strlen(temp)<=len)
    {
      strncpy((char*)buf,temp,len);
      return strlen(temp);
    }
    Log("strlen(event)=%d>len=%d",strlen(temp),len);
    return 0;
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