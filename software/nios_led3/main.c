/*******************************************************************
* Module: Advanced flashing-LED system using Altera HAL/C stdlib
* Purpose: Task routines and main program
********************************************************************/
/* include section */
/* General C library */
#include <stdio.h>
#include <unistd.h>


/* Altera specific library */
#include "alt_types.h"
#include "system.h"
#include "gpio.h"

/********************************************************
* data type definitions
********************************************************/
typedef struct flash_cmd{
	int pause;
	int prd;
} cmd_type;

/*******************************************************************
* function: flashsys.init.v2()
* purpose: system initialization
* argument:
* 	btn.base: base address of pushbutton PIO
* return:
* note :
********************************************************************/
void flashsys_init_v2(alt_u32 btn_base)
{
	btn_clear(btn_base); // clear button edge-capture reg
}

/*******************************************************************
* function : jtag-uart-disp-msg_v2()
* purpose: display the interval when it is changed
* argument:s
* cmd: command
* return:
* note:
********************************************************************/
void jtag_uart_disp_msg_v2(cmd_type cmd){
	static int old=0;
	if (cmd.prd!=old){
		printf("Interval: %03u ms \n", cmd.prd);
		old = cmd.prd;
	}
}
/*******************************************************************
* function: led.flash.v2 ()
* purpose :
* argument:
* 	cmd: command
* return:
* note :
* 	The delay is done by continuously checking 1-ms tick
********************************************************************/
void led_flash_v2(alt_u32 led_base, cmd_type cmd){
	static alt_u8 led_pattern = 0x01;
	if (cmd.pause){ // no toggle if pause asserted
		return;}
	led_pattern ^= 0x03; // toggle 2 LSBs of LEDs
	pio_write(led_base , led_pattern) ; // write LEDs
	usleep(1000*cmd.prd); // delay for cmd.prd ms
}

/*******************************************************************
* function : sseg_disp_msg_v1(alt_u32 sseg_base, cmd_type cmd)
* purpose: display the msg in the sseg
* argument:sseg_base and cmd
* cmd: command
* return:
* note:
********************************************************************/

void sseg_disp_msg_v1(alt_u32 sseg_base, cmd_type cmd){
	int pd;
	alt_u8 hex, msg[4];
	if(cmd.prd > 999){ //999 is max # to be displayed
		pd = 999;
	}else{
		pd = cmd.prd;
	}
	hex = pd%10; // units digit
	msg[3] = sseg_conv_hex(hex);
	hex = (pd/10)%10; //tens digit
	msg[2] = sseg_conv_hex(hex);
	hex = pd/100;//100 digit
	msg[1] = sseg_conv_hex(hex);
	// specify pattern for the most significant digit
	if(cmd.pause){
		msg[0] = 0x0C; // pattern for letter P
	}else{
		msg[0] = 0xFF; // blank
	}
	sseg_disp_ptn(sseg_base, msg);
}



void sw_get_command_v1(alt_u32 btn_base, alt_32 sw_base, cmd_type *cmd, alt_u32 jtag_base){
	alt_u8 btn;
	btn = (alt_u8) btn_read(btn_base) & 0xF; //read 4 pushbuttons
	if(btn != 0){ // button pressed
		if(btn & 0x01){ // btn 0 pressed
			cmd->pause = cmd->pause ^ 1; // toogle pause bit
			//printf("%s", "0 preessed\n");
			btn_clear(btn_base); //clear edge-capture reg

		}
		if (btn & 0x02){ //btn 1 pressed
			cmd->prd = pio_read(sw_base) & 0x03FF; // load new interval
			btn_clear(btn_base); //clear edge-capture reg
			//printf("%s", "1 preessed\n");
		}
		if (btn & 0x04){ //btn 1 pressed
			int new_prd;
			printf("%s", "Type new interval");
			scanf("%d", &new_prd);
			if(new_prd>999){
				cmd->prd = 999;
				printf("solo se pueden valores menores de 999, se toma 999 como intervalo\n");

			} else{
				cmd->prd = new_prd;
			}
			btn_clear(btn_base); //clear edge-capture reg

		}
	}
}

/*******************************************************************
* function : main()
* purpose: advanced flashing -LED system using HAL
* note :
********************************************************************/
int main(){
	cmd_type sw_cmd={0,100}; // not pause; 100 ms interval
	flashsys_init_v2(BTN_BASE);
	while(1){
		sw_get_command_v1(BTN_BASE, SWITCH_BASE ,&sw_cmd, JTAG_UART_BASE);
		jtag_uart_disp_msg_v2(sw_cmd);
		sseg_disp_msg_v1(SSEG_BASE, sw_cmd);
		led_flash_v2(LEDR_BASE,sw_cmd);
	}
}
