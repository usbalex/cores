#include <stdio.h>
#include <altera_jtag_atlantic.h>

#include "altera_jtag_hw.h"

//-----------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------
#define CMD_NOP        0x0
#define CMD_WR         0x1
#define CMD_RD         0x2
#define CMD_GP_WR      0x3
#define CMD_GP_RD      0x4

#define HDR_SIZE       6
#define MAX_TX_SIZE    2**6 - HDR_SIZE

struct JTAGATLANTIC *jtag_link = NULL;

//-----------------------------------------------------------------
// altera_jtag_hw_init:
//-----------------------------------------------------------------
int altera_jtag_hw_init(int instance_id)
{
    // Open channel
    jtag_link = jtagatlantic_open("USB-Blaster", 0, instance_id, NULL);
    if (jtag_link == NULL)
    {
        OUT_ERROR("Unable to open altera_jtag device: %s, %x, %x.\n", m_device_name, m_device_id, m_instance_id);
        return -1;
    }

    // Flush buffers
    jtagatlantic_flush(jtag_link);

    return 0;
}
//-----------------------------------------------------------------
// altera_jtag_hw_close:
//-----------------------------------------------------------------
int altera_jtag_hw_close(void)
{
    if (jtag_link != NULL) jtagatlantic_close(jtag_link);
    jtag_link = NULL;

    return 0;
}
//-----------------------------------------------------------------
// altera_jtag_hw_mem_write:
//-----------------------------------------------------------------
int altera_jtag_hw_mem_write(uint32_t addr, uint8_t *data, int length)
{
    if (data == NULL)
    {
        fprintf(stderr, "altera_jtag_hw_mem_write: Invalid data buffer\n");
        return -1;
    }
    int i;
    int sent = 0;
    int size = length;
    int res;
    uint8_t buffer[MAX_TX_SIZE + HDR_SIZE];
    uint8_t *p;

    while (sent < length)
    {
        size = (length - sent);
        if (size > MAX_TX_SIZE)
            size = MAX_TX_SIZE;

        // Build packet header
        p = buffer;
        *p++ = (((size >> 8) & 0xF) << 4) | CMD_WR;
        *p++ = (size & 0xFF);

        *p++ = (addr >> 24);
        *p++ = (addr >> 16);
        *p++ = (addr >> 8);
        *p++ = (addr >> 0);        

        // Fill packet payload
        for (i=0;i<size;i++)
            *p++ = *data++;

        // Write request + data to FTDI device
        res = jtagatlantic_write(jtag_link, buffer, (size + HDR_SIZE));
        // Flush buffers
        jtagatlantic_flush(jtag_link);
        if (res != (size + HDR_SIZE))
        {
            fprintf(stderr, "altera_jtag_hw_mem_write: Failed to send\n");
            return -1;
        }        

        sent += size;
        addr += size;
    }

    return sent;
}
//-----------------------------------------------------------------
// altera_jtag_hw_mem_read:
//-----------------------------------------------------------------
int altera_jtag_hw_mem_read(uint32_t addr, uint8_t *data, int length)
{
    if (data == NULL)
    {
        fprintf(stderr, "altera_jtag_hw_mem_read: Invalid data buffer\n");
        return -1;
    }
    int i;
    int received = 0;
    int size = length;
    int remain;
    int res;
    uint8_t buffer[HDR_SIZE];
    uint8_t *p;

    while (received < length)
    {
        size = (length - received);
        if (size > MAX_TX_SIZE)
            size = MAX_TX_SIZE;

        // Round up to nearest 4 byte multiple
        size = (size + 3) & ~3;

        // Build packet header
        p = buffer;
        *p++ = (((size >> 8) & 0xF) << 4) | CMD_RD;
        *p++ = (size & 0xFF);

        *p++ = (addr >> 24);
        *p++ = (addr >> 16);
        *p++ = (addr >> 8);
        *p++ = (addr >> 0);

        // Write request to FTDI device
        res = jtagatlantic_write(jtag_link, buffer, HDR_SIZE);
        // Flush buffers
        jtagatlantic_flush(jtag_link);
        if (res != HDR_SIZE)
        {
            fprintf(stderr, "altera_jtag_hw_mem_read: Failed to send request\n");
            return -1;
        }

        remain = size;
        do
        {
            res = jtagatlantic_read(jtag_link, data, remain);
            if (res < 0)
            {
                fprintf(stderr, "altera_jtag_hw_mem_read: Failed to read data\n");
                return -1;
            }

            remain -= res;
            data += res;
        }
        while (remain > 0);

        received += size;
        addr     += size;
    }

    return received;
}
//-----------------------------------------------------------------
// altera_jtag_hw_mem_write_word:
//-----------------------------------------------------------------
int altera_jtag_hw_mem_write_word(uint32_t addr, uint32_t data)
{
    uint8_t buffer[4];

    buffer[3] = (data >> 24);
    buffer[2] = (data >> 16);
    buffer[1] = (data >> 8);
    buffer[0] = (data >> 0);

    return altera_jtag_hw_mem_write(addr, buffer, 4);
}
//-----------------------------------------------------------------
// altera_jtag_hw_mem_read_word:
//-----------------------------------------------------------------
int altera_jtag_hw_mem_read_word(uint32_t addr, uint32_t *data)
{
    if (data == NULL)
    {
        fprintf(stderr, "altera_jtag_hw_mem_read_word: Invalid data buffer\n");
        return -1;
    }
    uint8_t buffer[4];

    int res = altera_jtag_hw_mem_read(addr, buffer, 4);
    if (res > 0)
    {
        (*data) = ((uint32_t)buffer[3]) << 24;
        (*data)|= ((uint32_t)buffer[2]) << 16;
        (*data)|= ((uint32_t)buffer[1]) << 8;
        (*data)|= ((uint32_t)buffer[0]) << 0;
    }
    return res;
}
//-----------------------------------------------------------------
// altera_jtag_hw_gpio_write:
//-----------------------------------------------------------------
int altera_jtag_hw_gpio_write(uint8_t value)
{
    uint8_t buffer[2] = { CMD_GP_WR, value };

    // Write request to FTDI device
    int res = jtagatlantic_write(jtag_link, buffer, sizeof(buffer));
    if (res != sizeof(buffer))
    {
        fprintf(stderr, "altera_jtag_hw_gpio_write: Failed to send\n");
        return -1;
    }

    return 0;
}
//-----------------------------------------------------------------
// altera_jtag_hw_gpio_read:
//-----------------------------------------------------------------
int altera_jtag_hw_gpio_read(uint8_t *value)
{
    if (value == NULL)
    {
        fprintf(stderr, "altera_jtag_hw_gpio_read: Invalid read buffer\n");
        return -1;
    }
    // Write request to FTDI device
    uint8_t request = CMD_GP_RD;
    int res = jtagatlantic_write(jtag_link, &request, 1);
    if (res != 1)
    {
        fprintf(stderr, "altera_jtag_hw_gpio_read: Failed to send\n");
        return -1;
    }

    // Poll for response
    do
    {
        res = jtagatlantic_read(jtag_link, value, remain);
        res = ftdi_read_data(_handle, value, 1);
        if (res < 0)
        {
            fprintf(stderr, "altera_jtag_hw_gpio_read: Failed to read data\n");
            return -1;
        }
    }
    while (res != 1);

    return 0;
}
