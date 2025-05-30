#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  //TODO();
  // 读取中断描述符表寄存器(IDTR)的limit部分（2字节）
  cpu.idtr.limit = vaddr_read(id_dest->addr, 2);
    
  if (decoding.is_operand_size_16) {
    // 16位操作数时读取3字节基地址
    cpu.idtr.base = vaddr_read(id_dest->addr + 2, 3);
  }
  else {
    // 32位操作数时读取4字节基地址
    cpu.idtr.base = vaddr_read(id_dest->addr + 2, 4);
  }

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  //TODO();
  // 将通用寄存器(r32)的值移动到控制寄存器(cr0/cr3)
  if (id_dest->reg == 0) {
    cpu.cr0 = id_src->val;
  }
  else if (id_dest->reg == 3) {
    cpu.cr3 = id_src->val;
  }
  else {
    panic("未实现的控制寄存器: CR%d", id_dest->reg);
  }
  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  //TODO();
  // 将控制寄存器(cr0/cr3)的值移动到通用寄存器(r32)
  if (id_src->reg == 0) {
    operand_write(id_dest, &cpu.cr0);
  }
  else if (id_src->reg == 3) {
    operand_write(id_dest, &cpu.cr3);
  }
  else {
    panic("未实现的控制寄存器: CR%d", id_src->reg);
  }
  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //TODO();
  void raise_intr(uint8_t NO,vaddr_t ret_addr);
  raise_intr(id_dest->val, decoding.seq_eip);

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(&cpu.eip);
  rtl_pop(&cpu.cs);
  rtl_pop(&t0);
  memcpy(&cpu.eflags, &t0, sizeof(cpu.eflags));

  decoding.jmp_eip = 1;
  decoding.seq_eip = cpu.eip;

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

// 将设备寄存器的数据送到CPU寄存器
make_EHelper(in) {
  //TODO();
  rtl_li(&t0, pio_read(id_src->val, id_dest->width));// 从id_src中读取数据，写入t0
  operand_write(id_dest, &t0);// 将t0写入id_dest
  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

// 将CPU寄存器的数据送到设备寄存器
make_EHelper(out) {
  //TODO();
  pio_write(id_dest->val, id_src->width, id_src->val);// 将id_src中的数据写入id_dest
  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
