/**************************************************************************
 *	Module: General-purpose I/O driver header
 * 	File:	gpio.c
 *	Purpose:Routines to access switches and leds
 *  Created on: Nov 6, 2022
 *      Author: danielcrovo
 **************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include "alt_types.h"
#include "io.h"
#include "altera_avalon_pio_regs.h"
/*************************************************************************
 * 	Constant definition
 *************************************************************************/
#define PIO_DATA_REG_OFT	0			//data register addr offset
#define PIO_DIRT_REG_OFT	1			//direction register addr offset
#define PIO_INTM_REG_OFT	2			//interrupt register addr offset
#define PIO_EDGE_REG_OFT	3			//edge capture register addr offset



/**************************************************************************
 *  macro definitions
 **************************************************************************/

// read/write PIO data register
#define pio_read(base) IORD_ALTERA_AVALON_PIO_DATA(base)
#define pio_write(base, data) IOWR_ALTERA_AVALON_PIO_DATA(base, data)
//read/clear pushbutton edge capture register
#define btn_read(base) IORD_ALTERA_AVALON_PIO_EDGE_CAP(base)
#define btn_clear(base) IOWR_ALTERA_AVALON_PIO_EDGE_CAP(base, 0xF)
#define btn_is_pressed(base) (IORD(base, PIO_EDGE_REG_OFT)!=0)

/**************************************************************************
 * function prototypes
 *************************************************************************/

alt_u8 sseg_conv_hex(int hex);
void sseg_disp_ptn(alt_u32 base, alt_u8 *ptn);

#endif /* GPIO_H_ */
