#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
//#include "ftdi_hw.h"
#include "altera_jtag_hw.h"

//-----------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------
#define DEFAULT_FTDI_IFACE  1

//-----------------------------------------------------------------
// main:
//-----------------------------------------------------------------
int main(int argc, char *argv[])
{
    int i = 0;
    int err = 0;
    int quiet = 0;
    uint8_t value = 0;

    // Try and communicate with FTDI interface
    if (ftdi_hw_init(-1) != 0)
    {
        fprintf(stderr, "ERROR: Could not open FTDI interface, try SUDOing / check connection\n");
        exit(-2);
    }

    for (i = 0; i < (1<<8); ++i)
    {
        value = i;
        if (!quiet) 
            printf("Write 0x%x to GPIO\n", value);

        if (ftdi_hw_gpio_write(value) != 0)
        {
            fprintf(stderr, "ERROR: Could not write to device\n");
            err = 1;
        }
    }

    ftdi_hw_close();

    return err;
}
