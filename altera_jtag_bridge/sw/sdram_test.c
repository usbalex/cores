#include <stdio.h>
#include <sys/time.h> 

//#include "ftdi_hw.h"
#include "altera_jtag_hw.h"

//-----------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------
//#define DEFAULT_FTDI_IFACE  1
#define DEFAULT_FTDI_IFACE  0
//#define BLOCK_SIZE  1024
#define BLOCK_SIZE  8192

//#define MEM_SIZE    ((1 * 1024) / 4)
#define MEM_SIZE    ((16*1024) / 4)

static uint32_t mem[MEM_SIZE];

//-----------------------------------------------------------------
// main
//-----------------------------------------------------------------
int main(void)
{
    uint32_t buffer[BLOCK_SIZE/4];
    uint32_t req;
    uint32_t resp;
    uint32_t addr;
    uint8_t gpio;
    struct timeval t1, t2;
    double elapsedTime;    
    int i;
    int xfer;

    if (ftdi_hw_init(DEFAULT_FTDI_IFACE) != 0)
    {
        fprintf(stderr, "ERROR: Could not open FTDI interface, try SUDOing\n");
        return 0;
    }

    addr = 0x00000000;
    req = 0x12345678;
    ftdi_hw_mem_write_word(addr, req);
    ftdi_hw_mem_read_word(addr, &resp);

    if (req != resp)
    {
        printf("ERR0: %x != %x\n", req, resp);
    }

    addr = 0x00000004;
    req = 0xcafebabe;
    ftdi_hw_mem_write_word(addr, req);
    ftdi_hw_mem_read_word(addr, &resp);

    if (req != resp)
    {
        printf("ERR1: %x != %x\n", req, resp);
    }

    addr = 0x00000000;
    req = 0x12345678;
    resp = 0xdeadbeef;
    xfer = ftdi_hw_mem_read_word(addr, &resp);
    if (xfer != 4)
    {
        printf("WARN: Read of %d != %d (expected) bytes", xfer, 4);
    }

    if (req != resp)
    {
        printf("ERR2: %x != %x\n", req, resp);
    }

    for (i=0;i<MEM_SIZE;i++)
        mem[i] = 0;

    printf("Erasing memory\n");
    xfer = ftdi_hw_mem_write(0, (uint8_t*)mem, MEM_SIZE);
    if (xfer != MEM_SIZE)
    {
        printf("WARN: Write of %d != %d (expected) bytes", xfer, MEM_SIZE);
    }
    printf("Erasing memory - done\n");

    // Start timer
    gettimeofday(&t1, NULL);

    int sent = 0;
    while (1)
    {
        // SINGLE
        if (rand() & 1)
        {
            addr = rand() & ((MEM_SIZE * 4) - 1);
            addr &= ~3;

            req = rand();

            xfer = ftdi_hw_mem_write(addr, (uint8_t*)&req, 4);
            if (xfer != 4)
            {
                printf("WARN: Write of %d != %d (expected) bytes", xfer, 4);
            }
            xfer = ftdi_hw_mem_read(addr, (uint8_t*)&resp, 4);
            if (xfer != 4)
            {
                printf("WARN: Read of %d != %d (expected) bytes", xfer, 4);
            }
            //printf("Read: %x - %x\n", addr, resp);
            sent += 4;

            if (req != resp)
            {
                printf("ERR (RB): %x - %x (written) != %x (read)\n", addr, req, resp);
            }

            mem[addr/4] = req;
            
            addr = rand() & (MEM_SIZE - 1);
            addr &= ~3;

            ftdi_hw_mem_read(addr, (uint8_t*)&resp, 4);
            //printf("Read: %x - %x\n", addr, resp);
            sent += (4 * 3);

            if (mem[addr/4] != resp)
            {
                printf("ERR (MEM): %x - %x (written) != %x (read)\n", addr, mem[addr/4], resp);
            }
        }
        // BLOCK
        else
        {
            addr = rand() & ((MEM_SIZE * 4) - 1);
            addr &= ~3;

            // Stop block from overflowing RAM
            if (addr > ((MEM_SIZE * 4) - BLOCK_SIZE))
                addr = ((MEM_SIZE * 4) - BLOCK_SIZE);

            for (i=0;i<BLOCK_SIZE / 4;i++)
            {
                buffer[i] = rand();
                mem[(addr/4)+i] = buffer[i];
            }

            // Write block
            ftdi_hw_mem_write(addr, (uint8_t*)&buffer, BLOCK_SIZE);
            sent += BLOCK_SIZE;
        }

        // Stop timer
        gettimeofday(&t2, NULL);        

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms   

        if (((int)elapsedTime) >= 1000)
        {
            printf("%dKB/s\n", sent / 1024);

            gettimeofday(&t1, NULL);
            sent = 0;
        }
    }    

    ftdi_hw_close();
    return 0;
}
