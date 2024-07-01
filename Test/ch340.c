#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define CH341_BAUDBASE_FACTOR 1532620800
#define CH341_BAUDBASE_DIVMAX 3

static int ch341_set_baudrate_lcr(uint32_t baud_rate, uint8_t lcr)
{
	short a;
	int r;
	unsigned long factor;
	short divisor;

	if (!baud_rate)
		return -1;
	factor = (CH341_BAUDBASE_FACTOR / baud_rate);
	divisor = CH341_BAUDBASE_DIVMAX;

	while ((factor > 0xfff0) && divisor) {
		factor >>= 3;
		divisor--;
	}

	if (factor > 0xfff0)
		return -1;

	factor = 0x10000 - factor;
	a = (factor & 0xff00) | divisor;

	/*
	 * CH341A buffers data until a full endpoint-size packet (32 bytes)
	 * has been received unless bit 7 is set.
	 */
	// a |= BIT(7);
    a |= (1 << 7);

    printf("0x%4X\r\n", a & 0xFFFF);
	// r = ch341_control_out(dev, CH341_REQ_WRITE_REG, 0x1312, a);


	// r = ch341_control_out(dev, CH341_REQ_WRITE_REG, 0x2518, lcr);

	return 0;
}




int main (void){

    uint32_t bauds[] = {50, 75, 100, 110, 134.5, 150, 300, 600, 900, 1200, 1800, 2400, 3600, 4800, 9600, 14400, 19200, 28800, 33600, 38400, 56000, 57600, 76800, 115200, 128000, 153600, 230400, 460800, 921600, 1500000, 2000000, 3000000};
    int len = sizeof(bauds) / sizeof(uint32_t);
    int i;

    for(i =0; i< len ; i++){
        printf("%d: ",bauds[i]);
        ch341_set_baudrate_lcr(bauds[i],0);
    }

    return 0;
}