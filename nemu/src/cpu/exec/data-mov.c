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

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //TODO();
    // cwtl:将AX符号扩展到DX:AX
    t0 = reg_w(0);// 获取AX
    rtl_sext(&t1, &t0, 2); // AX符号扩展
    reg_w(2) = (uint16_t) ((uint32_t)t1 >> 16); // 取高16位给DX
  }
  else {
    //TODO();
    // cltd:将EAX符号扩展到EDX:EAX
    t0 = reg_l(0); // 获取EAX
    rtl_sext(&t1, &t0, 4); // EAX符号扩展
    reg_l(2) = (uint32_t) ((uint64_t)t1 >> 32); // 取高32位给EDX
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    //TODO();
    // cbtw:将AL扩展到AX
    printf("cbtw");
    t0 = reg_b(0); // 获取AL
    rtl_sext(&t1, &t0, 2); // AL符号扩展
    reg_w(0) = (uint16_t)t1;
  }
  else {
    //TODO();
    // cwtl:将AX符号扩展到EAX
    printf("cwtl\n");

    printf("AL: 0x%08x\n", reg_b(0));
    printf("AH: 0x%08x\n", reg_b(4));
    printf("AX: 0x%08x\n", reg_w(0));
    printf("EAX: 0x%08x\n", reg_l(0));

    printf("\n");

    t0 = reg_w(0); // 获取AX

    printf("AX: 0x%08x\n", t0);

    rtl_sext(&t1, &t0, 4); // EAX符号扩展

    printf("\n");
    printf("EAX: 0x%08x\n", t1);

    reg_l(0) = (uint32_t)t1;

    printf("\n");

    printf("AL: 0x%08x\n", reg_b(0));
    printf("AH: 0x%08x\n", reg_b(4));
    printf("AX: 0x%08x\n", reg_w(0));
    printf("EAX: 0x%08x\n", reg_l(0));
    
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
