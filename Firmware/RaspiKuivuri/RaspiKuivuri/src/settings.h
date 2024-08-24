/*
 * settings.h
 *
 * Created: 4.8.2017 20.15.04
 *  Author: Admin
 */ 
 #include "onewire.h"

#ifndef SETTINGS_H_
#define SETTINGS_H_


#define LAST_PAGE_ADDRESS (IFLASH0_ADDR + IFLASH0_SIZE- IFLASH0_PAGE_SIZE*4)

typedef struct
{
	uint8_t id[OW_ROMCODE_SIZE];
} OWID_t;

typedef struct
{
	OWID_t ids[3];
	int8_t thermo_celcius;
	uint8_t thermo_cel_frac_bits;
} settings_t;


void SetupTods1820Values(void);
void FlashToSettings(void);
void SettingsToFlash(void);

#endif /* SETTINGS_H_ */