#include "nemu.h"
#include "device/mmio.h"

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
    switch(len) {
      case 1: return pmem_rw(addr, uint8_t);
      case 2: return pmem_rw(addr, uint16_t);
      case 4: return pmem_rw(addr, uint32_t);
      default: Assert(0, "invalid memory read length %d", len);
    }
  }
  else{
    return mmio_read(addr, len, map_NO);
  }
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int map_NO = is_mmio(addr);
  if(map_NO == -1){
    switch(len) {
      case 1: pmem_rw(addr, uint8_t) = (uint8_t)data; break;
      case 2: pmem_rw(addr, uint16_t) = (uint16_t)data; break;
      case 4: pmem_rw(addr, uint32_t) = data; break;
      default: Assert(0, "invalid memory write length %d", len);
    }
  }
  else{
    mmio_write(addr, len, data, map_NO);
  }
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  paddr_write(addr, len, data);
}
