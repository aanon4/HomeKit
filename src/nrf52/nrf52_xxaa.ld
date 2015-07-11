/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

/*
 * Total FLASH: 512K
 * Total RAM: 32K (shouldn't this be 64K?)
 *
 * S132: 112K FLASH, 10K RAM
 */

MEMORY
{
  FLASH (rx) : ORIGIN = 0x0001F000, LENGTH = 512K - 0x1F000
  RAM (rwx) :  ORIGIN = 0x20002800, LENGTH =  32K - 0x02800
}


INCLUDE "nrf52_common.ld"
