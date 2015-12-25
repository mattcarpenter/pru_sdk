#include <stdint.h>
#include "pru_hal.h"

volatile register uint32_t __R30;

// P8.39 = __R30[6]

int main(void)
{
  ocp_init();
  shm_init();
 
  while (1)
  {
    __R30 |= 1 << 7;
    __R30 &= ~(1 << 7);
	
    //shm_write_uint32(0, 0xF);
    //shm_write_uint32(4, 0);
    //shm_write_uint32(8, 0);
    //shm_write_uint32(0, 0xdeadbeef);
    //shm_write_uint32(4, 0x2b2b2c2d);
    //shm_write_float(8, x);
  }

  __halt();

  return 0;
}
