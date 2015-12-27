#include <stdint.h>
#include "pru_hal.h"
#include "pru_main.h"

/*
 * Pin assignments
 * 
 * P9.25 [:7] - SCK
 * P9.27 [:5] - MOSI
 * P9.28 [:3] - SS0
 * P9.29 [:1] - SS1
 * P9.30 [:2] - SS2
 * P9.31 [:0] - SS3
 * P9.41 [:6] - SS4
 * P9.42 [:4] - SS5
 */

/**
 * Application entry point
 *
 * @returns {int}
 */
int main(void)
{
  ocp_init();
  shm_init();

  while (1)
  {
    // set DAC0
    spiWrite2Bytes(0x3F, 0xFF, SS0);

    // set DAC1
    spiWrite2Bytes(0xBF, 0xFF, SS0);
  }

  __halt();

  return 0;
}

/**
 * Writes two bytes to the SPI bus
 *
 * @param {char} high High byte
 * @param {char} low Low byte
 * @param {int} SS slave-select pin
 */
void spiWrite2Bytes(char high, char low, unsigned int SS) {
  unsigned char count;

  // clock low
  clearBit(SCK);
  clearBit(SS);

  // Write DAC0 high
  for (count = 0; count < 8; count++) {
    if (high & 0x80)
      setBit(MOSI);
    else
      clearBit(MOSI);

    // tick
    setBit(SCK);
    __delay_cycles(10);
    clearBit(SCK);
    high <<= 1;
  }

  // write DAC0 low
  for (count = 0; count < 8; count++) {
    if (low & 0x80)
      setBit(MOSI);
    else
      clearBit(MOSI);

    // tock
    setBit(SCK);
    __delay_cycles(10);
    clearBit(SCK);
    low <<= 1;
  }

  clearBit(MOSI);
  setBit(SS);
  __delay_cycles(20);
}

/**
 * Sets a bit
 *
 * @param {int} bit bit number
 */
void setBit(int bit) {
  __R30 |= 1 << bit;
}

/**
 * Clears a bit
 *
 * @param {int} bit bit number
 */
void clearBit(int bit) {
  __R30 &= ~(1 << bit);
}
