#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {
  return (uint64_t)a * (uint64_t)b >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  assert(b != 0);
  FLOAT x = Fabs(a);
  FLOAT y = Fabs(b);
  FLOAT ret = x / y;
  x = x % y;
  //处理低16位
  for (int i = 0; i < 16; i++) {
    x <<= 1;
    ret <<= 1;
    if (x >= y) {
      x -= y;
      ret++;
    }
  }
  if (((a ^ b) & 0x80000000) == 0x80000000) {
    ret = -ret;
  }
  return ret;
}

FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */

   union float_ {
    struct {
      uint32_t m : 23;
      uint32_t e : 8;
      uint32_t s : 1;
    };
    uint32_t value;
  } f;
  f.value = *(uint32_t*)&a;
  
  int exponent = f.e - 127;// 真值
  uint32_t mantissa = f.m | (1 << 23);// 加上隐含最高位1的尾数

  FLOAT result;
  int shift = exponent + 16 - 23;
  if (shift >= 0) {
    result = mantissa << shift;
  } else {
    result = mantissa >> (-shift);
  }

  if (f.s == 1) {
    result = -result;
  }

  return result;
}

FLOAT Fabs(FLOAT a) {
  if(a < 0){
    return -a;
  }
  else{
    return a;
  }
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

