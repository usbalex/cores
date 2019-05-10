#ifndef _ALTERA_JTAG_HW_H_
#define _ALTERA_JTAG_HW_H_

#include <stdint.h>

//-----------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Prototypes:
//-----------------------------------------------------------------
int altera_jtag_hw_init(int interface);
int altera_jtag_hw_close(void);

// Memory Access
int altera_jtag_hw_mem_write(uint32_t addr, uint8_t *data, int length);
int altera_jtag_hw_mem_read(uint32_t addr, uint8_t *data, int length);
int altera_jtag_hw_mem_write_word(uint32_t addr, uint32_t data);
int altera_jtag_hw_mem_read_word(uint32_t addr, uint32_t *data);

// GPIO
int altera_jtag_hw_gpio_write(uint8_t value);
int altera_jtag_hw_gpio_read(uint8_t *value);



int ftdi_hw_init(int interface)
{
    return altera_jtag_hw_init(interface);
}
int ftdi_hw_close(void)
{
    return altera_jtag_hw_close();
}

// Memory Access
int ftdi_hw_mem_write(uint32_t addr, uint8_t *data, int length)
{
    return altera_jtag_hw_mem_write(addr, data, length);
}
int ftdi_hw_mem_read(uint32_t addr, uint8_t *data, int length)
{
    return altera_jtag_hw_mem_read(addr, data, length);
}
int ftdi_hw_mem_write_word(uint32_t addr, uint32_t data)
{
    return altera_jtag_hw_mem_write_word(addr, data);
}
int ftdi_hw_mem_read_word(uint32_t addr, uint32_t *data)
{
    return altera_jtag_hw_mem_read_word(addr, data);
}

// GPIO
int ftdi_hw_gpio_write(uint8_t value)
{
    altera_jtag_hw_gpio_write(value);
}
int ftdi_hw_gpio_read(uint8_t *value);
{
    altera_jtag_hw_gpio_read(value);
}

#endif
