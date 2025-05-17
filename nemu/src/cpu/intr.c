#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
  //依次将 EFLAGS，CS，EIP 寄存器的值压入栈中
  memcpy(&t1, &cpu.eflags, sizeof(cpu.eflags));
  rtl_li(&t0, t1);
  rtl_push(&t0);// 将 EFLAGS 寄存器的值压入栈中
  cpu.eflags.IF = 0;// 清除IF位，使处理器进⼊关中断状态
  t0 = cpu.cs;
  rtl_push(&t0);// 将 CS 寄存器的值压入栈中
  rtl_li(&t0, ret_addr);
  rtl_push(&t0);// 将 EIP 寄存器的值压入栈中

  // 读取 IDTR 寄存器中的 IDT 的首地址
  uint32_t idtr_base = cpu.idtr.base;

  //以异常号为索引在 IDT 中找到门描述符
  vaddr_t gate_addr = idtr_base + NO * sizeof(GateDesc);

  //将门描述符中的 offset 域组合成目标地址
  uint32_t off_low = vaddr_read(gate_addr, 4) & 0x0000ffff;
  uint32_t off_high = vaddr_read(gate_addr + sizeof(GateDesc) / 2, 4) & 0xffff0000;
  uint32_t target_addr = off_high | off_low;

  decoding.is_jmp = 1;
  decoding.jmp_eip = target_addr;
}

void dev_raise_intr() {
  cpu.INTR = true;
}
