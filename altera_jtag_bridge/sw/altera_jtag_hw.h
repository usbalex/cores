#ifndef _ALTERA_JTAG_HW_H_
#define _ALTERA_JTAG_HW_H_

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#endif
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


//int ftdi_hw_init(int interface);
int ftdi_hw_init(void);
int ftdi_hw_close(void);

// Memory Access
int ftdi_hw_mem_write(uint32_t addr, uint8_t *data, int length);
int ftdi_hw_mem_read(uint32_t addr, uint8_t *data, int length);
int ftdi_hw_mem_write_word(uint32_t addr, uint32_t data);
int ftdi_hw_mem_read_word(uint32_t addr, uint32_t *data);

// GPIO
int ftdi_hw_gpio_write(uint8_t value);
int ftdi_hw_gpio_read(uint8_t *value);
#ifdef __cplusplus
}
#endif

#endif
