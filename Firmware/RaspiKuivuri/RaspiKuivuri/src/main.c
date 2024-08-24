/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "console.h"
#include "settings.h"
#include "ds18x20.h"

settings_t settings;
struct ds1820Measurement_t ds1820Values[3];

#define STATUS_LED IOPORT_CREATE_PIN(PIOB, 8)
#define STATUS_LED2 IOPORT_CREATE_PIN(PIOB, 7)

void SysTick_Handler()
{

}

int main (void)
{	
	WDT->WDT_MR = WDT_MR_WDDIS;

	sysclk_init();
	board_init();
	ioport_init();	
	membag_init();
	SysTick_Config(0x00FFFFFF);	
	
	ioport_set_pin_dir(OUTRELAY1PIN, IOPORT_DIR_OUTPUT);
	
	console_Setup(9600);

	console_SendString("System start\r\n");
	
	uint32_t ul_rc = flash_init(FLASH_ACCESS_MODE_128, 6);
    if (ul_rc != FLASH_RC_OK) 
		console_SendString("Flash init fails\r\n");	   

	FlashToSettings();
	SetupTods1820Values();
	
	while(1)
	{
		console_Poll();
	}
}
