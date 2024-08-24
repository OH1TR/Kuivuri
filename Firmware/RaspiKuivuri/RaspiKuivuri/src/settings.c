/*
 * settings.c
 *
 * Created: 7.8.2017 16.58.57
 *  Author: Admin
 */ 
 #include <asf.h>
 #include <string.h>
 #include "main.h"
 #include "console.h"
 

 void SetupTods1820Values(void)
 {
 	//Setup struct for DS1820 measurements
 	memset(ds1820Values,0,sizeof(ds1820Values));
 	memcpy(ds1820Values[0].id,settings.ids[0].id,sizeof(((OWID_t*)0)->id));
 	memcpy(ds1820Values[1].id,settings.ids[1].id,sizeof(((OWID_t*)0)->id));
 	memcpy(ds1820Values[2].id,settings.ids[2].id,sizeof(((OWID_t*)0)->id));
}

void FlashToSettings(void)
{
	// Flash data to settings
	memcpy(&settings,(void *)LAST_PAGE_ADDRESS,sizeof(settings_t));
}

void SettingsToFlash(void)
{
	uint32_t ul_last_page_addr = LAST_PAGE_ADDRESS;

	uint32_t ul_rc = flash_unlock(ul_last_page_addr, ul_last_page_addr + IFLASH0_PAGE_SIZE - 1, 0, 0);
	if (ul_rc != FLASH_RC_OK)
	{
		console_SendString("Unlock fails\r\n");
		return;
	}
	ul_rc = flash_write(ul_last_page_addr, &settings,sizeof(settings), 1);

	if (ul_rc != FLASH_RC_OK)
	{
		console_SendString("Write fails\r\n");
		return;
	}
}