#include "cpu/exec.h"

// 两个操作数相与，根据结果设置eflags，不保存结果
make_EHelper(test) {
  //TODO();
  rtl_and(&t2, &id_dest->val, &id_src->val);

  // 更新零标志位(ZF)和符号标志位(SF)
  rtl_update_ZFSF(&t2, id_dest->width);

  // 进位标志位(CF)和溢出标志位(OF)置零
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  rtl_and(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);

  // 更新零标志位(ZF)和符号标志位(SF)
  rtl_update_ZFSF(&t2, id_dest->width);

  // 进位标志位(CF)和溢出标志位(OF)置零
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();
  rtl_xor(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  
  // 更新零标志位(ZF)和符号标志位(SF)
  rtl_update_ZFSF(&t2, id_dest->width);
  
  // 进位标志位(CF)和溢出标志位(OF)置零
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtl_or(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);

  // 更新零标志位(ZF)和符号标志位(SF)
  rtl_update_ZFSF(&t2, id_dest->width);

  // 逻辑运算清除进位标志位(CF)和溢出标志位(OF)
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(or);
}

// 算术右移
make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_sext(&t2, &id_dest->val, id_dest->width);
  rtl_sar(&t2, &t2, &id_src->val);
  operand_write(id_dest, &t2);
  // 更新零标志位(ZF)和符号标志位(SF)
  rtl_update_ZFSF(&t2, id_dest->width);
  print_asm_template2(sar);
}

// 逻辑左移
make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  // 更新零标志位(ZF)和符号标志位(SF)
  rtl_update_ZFSF(&t2, id_dest->width);

  print_asm_template2(shl);
}

// 逻辑右移
make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  // 更新零标志位(ZF)和符号标志位(SF)
  rtl_update_ZFSF(&t2, id_dest->width);

  print_asm_template2(shr);
}

// 循环左移
// 等价于逻辑左移n位和逻辑右移(width * 8 - n)位进行或运算
make_EHelper(rol) {
  rtl_shl(&t2, &id_dest->val, &id_src->val);
  rtl_shri(&t1, &id_dest->val, id_dest->width * 8 - id_src->val);
  rtl_or(&t2, &t2, &t1);

  operand_write(id_dest, &t2);

  // 更新进位标志位(CF)
  // t1 和 1 进行与运算，得到最低位的结果
  rtl_li(&t2, 1);
  rtl_and(&t2, &t1, &t2);
  rtl_set_CF(&t2);
  
  print_asm_template2(rol);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  //TODO();
  rtl_not(&id_dest->val);
  operand_write(id_dest, &id_dest->val);

  print_asm_template1(not);
}
