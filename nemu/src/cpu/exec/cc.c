#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:// 获取溢出标志位(OF)
      *dest = cpu.eflags.OF;
      break;
    case CC_B:// 获取进位标志位(CF)
      *dest = cpu.eflags.CF;
      break;
    case CC_E:// 获取零标志位(ZF)
      *dest = cpu.eflags.ZF;
      break;
    case CC_BE:// 检查CF=1或ZF=1
      *dest = cpu.eflags.CF | cpu.eflags.ZF;
      break;
    case CC_S:// 获取符号标志位(SF)
      *dest = cpu.eflags.SF;
      break;
    case CC_L:// 检查SF!=OF
      *dest = cpu.eflags.SF != cpu.eflags.OF;
      break;
    case CC_LE:// 检查ZF=1或SF!=OF
      *dest = cpu.eflags.ZF || (cpu.eflags.SF != cpu.eflags.OF);
      break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
