#include <stdint.h>
#include "pru_hal.h"
#include "pru_main.h"

/**
 * Pin assignments
 * 
 * P9.25 [:7] - SCK
 * P9.27 [:5] - MOSI
 * P9.28 [:3] - SS0
 * P9.29 [:1] - SS1
 * P9.30 [:2] - SS2
 * P9.31 [:0] - SS3
 * P9.41 [:6] - SS4
 * P9.42 [:4] - DAC Latch
 */

/**
 * Notes:
 * use int16_t
 * max value (12 bit): 4095
 */

typedef union point{
  int16_t IntVar;
  unsigned char Bytes[2];
} POINT;

/**
 * Application entry point
 *
 * @returns {int}
 */
int main(void)
{
  unsigned int point = 0;
  unsigned char high;
  unsigned char low;
  uint32_t offset = 0;
  uint32_t points = 0;

  // latch dac
  setBit(4);

  //POINT x;
  //POINT y;

  // set up one image frame
  POINT frame[4][2];

  //top left
  frame[0][0].IntVar = 0x00;
  frame[0][1].IntVar = 0x00;

  // top right
  frame[1][0].IntVar = 0xfff;
  frame[1][1].IntVar = 0x00;

  // bottom right
  frame[2][0].IntVar = 0xfff;
  frame[2][1].IntVar = 0xfff;

  // bottom left
  frame[3][0].IntVar = 0x00;
  frame[3][1].IntVar = 0xfff;

  ocp_init();
  shm_init();

  //uint32_t i = shm_read(0);
  //shm_write_uint32(4, i);


  //shm_write_uint32(4, i);
  //shm_write_uint32(0, 0);
  //shm_write_uint32(4, 0);
  //shm_write_uint32(8, 0);
  while (1)
  {
    POINT x;
    POINT y;

    x.IntVar = shm_read(offset) & 0xFFF;
    offset+=4;
    y.IntVar = shm_read(offset) & 0xFFF;
    offset+=4;

    points++;

    // write current point X to DAC0
    x.Bytes[1] |= (1 << 4); // set active mode operation
    x.Bytes[1] |= (1 << 5); // gain = 1x
    spiWrite2Bytes(x.Bytes[1], x.Bytes[0], SS0);
    __delay_cycles(10);

    // write current point Y to DAC1
    y.Bytes[1] |= (1 << 4); //set active mode operation
    y.Bytes[1] |= (1 << 5); // gain = 1x
    y.Bytes[1] |= (1 << 7); // set DAC1
    spiWrite2Bytes(y.Bytes[1], y.Bytes[0], SS0);

    // trigger latch
    __delay_cycles(20);
    clearBit(4);
    __delay_cycles(100);
    setBit(4);

    __delay_cycles(400000);
    //__delay_cycles(500);
    //if (points >= 2484/2) {
    if (points >= 5) {
      points = 0;
      offset = 0;
    }
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
  __delay_cycles(10);
  clearBit(SS);
  __delay_cycles(10);

  // Write DAC0 high
  for (count = 0; count < 8; count++) {
    if (high & 0x80)
      setBit(MOSI);
    else
      clearBit(MOSI);

    // tick
    __delay_cycles(20);
    setBit(SCK);
    __delay_cycles(20);
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
    __delay_cycles(20);
    setBit(SCK);
    __delay_cycles(20);
    clearBit(SCK);
    low <<= 1;
  }

  clearBit(MOSI);
  setBit(SS);
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
