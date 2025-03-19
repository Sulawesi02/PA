#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
  rtl_pop(&t0);
  operand_write(id_dest, &t0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

// 函数返回时，更新栈顶和栈底指针
make_EHelper(leave) {
  //TODO();
  rtl_mv(&cpu.esp, &cpu.ebp);
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

// 把eax的32位整数扩展为64位，高32位用eax的符号位填充保存到edx
// 或ax的16位整数扩展为32位，高16位用ax的符号位填充保存到dx
make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //TODO();
    // 将AX符号扩展到DX:AX
    rtl_lr(&t0, R_AX, 2); //加载AX到t0
    rtl_sext(&t0, &t0, 2); //符号扩展t0
    rtl_msb(&t1, &t0, 2); // 获取符号位

    if(t1 == 0){
      rtl_li(&t1, 0);
    }
    else{
      rtl_li(&t1, 0xffff);
    }
    rtl_sr(R_DX, 2, &t1); // 设置DX

  }
  else {
    //TODO();
    // 将EAX符号扩展到EDX:EAX
    rtl_lr(&t0, R_EAX, 4); //加载EAX到t0
    rtl_sext(&t0, &t0, 4); //符号扩展t0
    rtl_msb(&t1, &t0, 4); // 获取符号位
    if(t1==0){
      rtl_li(&t1, 0);
    }
    else{
      rtl_li(&t1, 0xffffffff);
    }
    rtl_sr(R_EDX, 4, &t1); // 设置EDX
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
