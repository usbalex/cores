/*
 * This header file just defines some prototypes for the routines that we'll
 * use in the jtag_atlantic.dll / libjtag_atlantic.so.
 *
 * With some help of:
 *  http://www.alteraforum.com/forum/archive/index.php/t-6002.html
 *      -> http://alterawiki.com/wiki/High_Speed_Image_Download_Demo (Image_download_demo.zip)
 *      -> http://savant.co.il/Business_Downloads.html (FPGA_PC_USB_interface.rar)
 *          -> "FPGA State Machine based jtag_uart communicates over USB jtag cable"
 */

#ifndef _ALTERA_JTAG_ATLANTIC_H_
#define _ALTERA_JTAG_ATLANTIC_H_

typedef struct JTAGATLANTIC JTAGATLANTIC;

struct JTAGATLANTIC {};
enum JATL_ERROR
{
    JTAL_SUCCESS                    = 0,
    JTAL_LINK_BROKEN                = -1,
    JTAL_CABLE_NOT_UNIQUE           = -2,
    JTAL_CABLE_NOT_EXISTING         = -3,
    JTAL_CABLE_NOT_CONNECTED_HOST   = -4,
    JTAL_CABLE_NOT_CONNECTED_BOARD  = -5,
    JTAL_IN_USE                     = -6,
    JTAL_NODE_NOT_UNIQUE            = -7,
    JTAL_NODE_NOT_EXISTING          = -8,
    JTAL_HW_BROKEN                  = -9,
    JTAL_BAD_ERROR                  = -99
};

int jtagatlantic_bytes_available(JTAGATLANTIC * link);

JTAGATLANTIC * jtagatlantic_open(const char * chain, int device_index, int link_instance, const char * app_name);
enum JATL_ERROR jtagatlantic_get_error(const char * * other_info);
void jtagatlantic_close(JTAGATLANTIC * link);
int jtagatlantic_write(JTAGATLANTIC * link, const char * data, unsigned int count);
int jtagatlantic_flush(JTAGATLANTIC * link);
int jtagatlantic_read(JTAGATLANTIC * link, char * buffer, unsigned int buffsize);

int jtagatlantic_wait_open(JTAGATLANTIC * link);
int jtagatlantic_cable_warning(JTAGATLANTIC * link);
void jtagatlantic_get_info(JTAGATLANTIC * link, char const * * chain, int * device_index, int * link_instance);

#endif
