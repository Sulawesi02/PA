#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int map_NO = is_mmio(addr);
  if(map_NO == -1){
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
  else{
    return mmio_read(addr, len, map_NO);
  }
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int map_NO = is_mmio(addr);
  if(map_NO == -1){
    memcpy(guest_to_host(addr), &data, len);
  }
  else{
    mmio_write(addr, len, data, map_NO);
  }
}

paddr_t page_translate(vaddr_t addr, bool is_write){
  CR0 cr0;
  cr0.val = cpu.cr0;
  // 检查分页机制是否启用（PE=1且PG=1）
  if(!(cr0.protect_enable && cr0.paging))
    return addr;

  CR3 cr3;
  cr3.val = cpu.cr3;

  PDE pde;  // 页目录项
  uint32_t pgdir_base = cr3.page_directory_base << 12;// 页目录表基址（cr3的高20位）
  uint32_t pd_idx = (addr >> 22) & 0x3ff;// 页目录项索引
  paddr_t pde_addr = pgdir_base + (pd_idx * 4);// 页目录项地址
  pde.val = paddr_read(pde_addr, 4);
  assert(pde.present);
  
  // 更新页目录项的访问位
  if (!pde.accessed) {
    pde.accessed = 1;
    paddr_write(pde_addr, 4, pde.val);
  }

  PTE pte;  // 页表项
  uint32_t pgtab_base = pde.page_frame << 12;// 页表基址（pde的高20位）
  uint32_t pt_idx = (addr >> 12) & 0x3ff;// 页表项索引
  paddr_t pte_addr = pgtab_base + (pt_idx * 4);// 页表项地址
  pte.val = paddr_read(pte_addr, 4);
  assert(pte.present);
  
  if (!pte.accessed) {
    // 更新页表项的访问位
    pte.accessed = 1;
    pte.val |= 0x20; // 设置accessed位
    paddr_write(pte_addr, 4, pte.val);
  }

  // 写操作更新脏位
  if (is_write) {
    pte.dirty = 1;
    pte.val |= 0x40; // 设置dirty位
    paddr_write(pte_addr, 4, pte.val);
  }

  uint32_t offset = addr & PAGE_MASK;// 页内偏移

  // 合成物理地址：页框基址 + 页内偏移
  return (pte.page_frame << 12) | offset;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  CR0 cr0;
  cr0.val = cpu.cr0;
  // 检查分页机制是否启用（PE=1且PG=1）
  if(!(cr0.protect_enable && cr0.paging))
    return paddr_read(addr, len);
  
  // 跨页读取
  if (((addr + len - 1) & ~PAGE_MASK) != (addr & ~PAGE_MASK)) {
    int len1 = PAGE_SIZE - (addr & PAGE_MASK);// 第一部分长度
    int len2 = len - len1;// 第二部分长度

    paddr_t paddr1 = page_translate(addr, false);// 获取第一部分的物理地址
    paddr_t paddr2 = page_translate(addr + len1, false);// 获取第二部分的物理地址
    
    uint32_t low = paddr_read(paddr1, len1);// 读取第一部分数据（低位）
    uint32_t high = paddr_read(paddr2, len2);// 读取第二部分数据（高位）
    
    return high << (8*len1) | low;
  }
  // 单页读取
  else{
    paddr_t paddr = page_translate(addr, false);
    return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  CR0 cr0;
  cr0.val = cpu.cr0;
  // 检查分页机制是否启用（PE=1且PG=1）
  if(!(cr0.protect_enable && cr0.paging)){
    paddr_write(addr, len, data);
  }
  else{
    // 跨页写入
    if (((addr + len - 1) & ~PAGE_MASK) != (addr & ~PAGE_MASK)) {
      int len1 = PAGE_SIZE - (addr & PAGE_MASK);// 第一部分长度
      int len2 = len - len1;// 第二部分长度

      paddr_t paddr1 = page_translate(addr, true);// 获取第一部分的物理地址
      paddr_t paddr2 = page_translate(addr + len1, true);// 获取第二部分的物理地址

      uint32_t low = data & (~0u >> (32 - len1*8));// 写入第一部分数据（低位）
      uint32_t high = data >> (len1 * 8) & (~0u >> (32 - len2*8));// 写入第二部分数据（高位）

      paddr_write(paddr1, len1, low);// 写入第一部分数据（低位）
      paddr_write(paddr2, len2, high);// 写入第二部分数据（高位）
    }
    // 单页写入
    else{
      paddr_t paddr = page_translate(addr, true);
      paddr_write(paddr, len, data);
    }
  }
}