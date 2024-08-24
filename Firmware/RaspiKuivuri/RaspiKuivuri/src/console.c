#include <asf.h>
#include "ringbuffer.h"
#include "console.h"
#include "ds18x20.h"
#include "onewire.h"
#include "crc8.h"
#include "settings.h"
#include "main.h"
#include "settings.h"


ring_buffer_t RbRx,RbTx;

#define CMDLEN 128
char command[CMDLEN];

unsigned int cmdPos=0;

int echo=0;

extern volatile int allowRun;

uint32_t sscdata[5]  __attribute__((aligned(8)));
uint32_t ssctxdata[5]  __attribute__((aligned(8))) = {0xff01ff00,0x11021100,0x22032200,0x33043300,0x44054400};

#define CONSOLEUSART USART0

int console_Setup(uint16_t bps)
{
	if(!rb_init(&RbRx,32))
	return(0);
	
	if(!rb_init(&RbTx,256))
	return(0);

	pmc_enable_periph_clk(ID_UART);
	pio_configure(PIOA,PIO_PERIPH_A,(PIO_PA11A_URXD|PIO_PA12A_UTXD),PIO_DEFAULT);
	
	const sam_uart_opt_t uart_settings =
	{ sysclk_get_cpu_hz(), bps, UART_MR_PAR_NO };

	uart_init(UART,&uart_settings);

	return(1);

		/*


	const sam_usart_opt_t usart_console_settings = {
		bps,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL
	};

	sysclk_enable_peripheral_clock(CONSOLEUSART);
	usart_init_rs232(CONSOLEUSART, &usart_console_settings,sysclk_get_main_hz());
	usart_enable_tx(CONSOLEUSART);
	usart_enable_rx(CONSOLEUSART);
	return(1);*/
}


int console_DataReceived(unsigned char *data,int len)
{
	rb_write_many(&RbRx,data,len);
	return(rb_half_full(&RbRx));
}

void IntToHex(char *buffer,uint32_t val,int digits)
{
	int i;
	int tmp;

	for(i=0;i<digits;i++)
	{
		tmp=val & 0x0f;
		if(tmp<10)
		tmp+='0';
		else
		tmp+='A'-10;

		buffer[digits-i-1]=(char)tmp;
		val=val>>4;
	}
}


void console_SendHex(uint32_t val,int digits)
{
	char SendHexBuf[10];
	IntToHex(SendHexBuf,val,digits);
	SendHexBuf[digits]=0;
	console_SendString(SendHexBuf);
}


int StrBeginsWith(const char *begin,char *str)
{
	int i=0;
	while(begin[i]!=0)
	{
		if(begin[i]!=str[i])
		return(0);
		i++;
	}
	return(1);
}


void console_SendString(const char *msg)
{
	unsigned int i = 0;
	while(msg[i]!=0)
	{
		rb_write(&RbTx,msg[i]);
		/*
		while(!uart_is_tx_ready(UART));
		uart_write(UART,msg[i]);
		*/
		i++;
	}
}

uint32_t ParseHex(char *str,int count,int *ok)
{
	int i;
	uint32_t retval=0;
	uint8_t part;

	for(i=0;i<count;i++)
	{
		part=255;
		if(str[i]>='0' && str[i]<='9')
		part=str[i]-'0';

		if(str[i]>='A' && str[i]<='F')
		part=str[i]+10-'A';

		if(str[i]>='a' && str[i]<='f')
		part=str[i]+10-'a';

		if(part==255)
		{
			*ok=0;
			return(0);
		}
		retval=retval<<4;
		retval|=part & 0x0f;
	}
	*ok=1;
	return(retval);
}

void DumpMem(uint32_t address)
{
	int x,y;

	for(y=0;y<16;y++)
	{
		console_SendHex(address,8);
		console_SendString(":");
		for(x=0;x<16;x++)
		{
			console_SendHex((*(volatile uint8_t *)address),2);
			console_SendString(" ");
			address++;
		}
		console_SendString("\r\n");
	}
}


void ProcessCommand(char *cmd)
{
	if(StrBeginsWith("HELLO",cmd))
	{
		console_SendString("Hello\r\n");
		return;
	}
	
	if(StrBeginsWith("ECHO ON",cmd))
	{
		console_SendString("Echo on\r\n");
		echo=1;
		return;
	}

	if(StrBeginsWith("ECHO OFF",cmd))
	{
		console_SendString("Echo off\r\n");
		echo=0;
		return;
	}



	if(StrBeginsWith("DUMPMEM",cmd))
	{
		int ok;
		uint32_t addr=ParseHex(cmd+8,8,&ok);
		if(ok)
		DumpMem(addr);
		else
		console_SendString("Hex parse fails");
		return;
	}
	
	if(StrBeginsWith("DS1820 SCAN",command))
	{
		uint8_t id[OW_ROMCODE_SIZE], sp[DS18X20_SP_SIZE], diff;
		uint8_t i;
		uint16_t meas;
		uint8_t subzero, cel, cel_frac_bits;


		if(DS18X20_start_meas(0,NULL)!=DS18X20_OK)
		{
			console_SendString( "Measure start failed\r\n" );
			return; // <--- early exit!
		}

		while(DS18X20_conversion_in_progress());

		for( diff = OW_SEARCH_FIRST; diff != OW_LAST_DEVICE; )
		{
			diff = ow_rom_search( diff, &id[0]);

			if( diff == OW_PRESENCE_ERR )
			{
				console_SendString( "No sensors\r\n" );
				return; // <--- early exit!
			}

			if( diff == OW_DATA_ERR )
			{
				console_SendString( "Bus error\r\n" );
				return;     // <--- early exit!
			}

			console_SendString("Found sensor:");
			for(i=0;i<OW_ROMCODE_SIZE;i++)
			console_SendHex(id[i],2);
			console_SendString("\n\r");

			if( id[0] == DS18B20_FAMILY_CODE || id[0] == DS18S20_FAMILY_CODE || id[0] == DS1822_FAMILY_CODE )
			{
				console_SendString("Is DS1820 family\r\nSP=");

				ow_byte_wr( DS18X20_READ );           // read command

				for ( i=0 ; i< DS18X20_SP_SIZE; i++ )
				{
					sp[i]=ow_byte_rd();
				}

				for(i=0;i<DS18X20_SP_SIZE;i++)
				console_SendHex(sp[i],2);

				if ( crc8( &sp[0], DS18X20_SP_SIZE ) )
				{
					console_SendString( "(FAIL)\r\n");
				}
				else
				{
					console_SendString( "(OK)\r\n");
				}

				meas = sp[0]; // LSB Temp. from Scrachpad-Data
				meas |= (uint16_t) (sp[1] << 8); // MSB

				console_SendString( "T_raw = 0x");
				console_SendHex( meas,4 );
				console_SendString("\r\n");

				if( id[0] == DS18S20_FAMILY_CODE )
				{ // 18S20
					console_SendString( "(S20/09) " );
				}
				else if ( id[0] == DS18B20_FAMILY_CODE || id[0] == DS1822_FAMILY_CODE )
				{ // 18B20 or 1822
					i=sp[DS18B20_CONF_REG];
					if ( (i & DS18B20_12_BIT) == DS18B20_12_BIT )
					{
						console_SendString( "(B20/12) ");
					}
					else if ( (i & DS18B20_11_BIT) == DS18B20_11_BIT )
					{
						console_SendString( "(B20/11) ");
					}
					else if ( (i & DS18B20_10_BIT) == DS18B20_10_BIT )
					{
						console_SendString( "(B20/10) ");
					}
					else
					{ // if ( (i & DS18B20_9_BIT) == DS18B20_9_BIT ) {
						console_SendString( "(B20/09) ");
					}
				}

				DS18X20_meas_to_cel( id[0], sp, &subzero, &cel, &cel_frac_bits );
				console_SendHex(cel,4);
				console_SendString(".");
				console_SendHex(cel_frac_bits*DS18X20_FRACCONV,4);
				console_SendString("\r\n");

			} // if meas-sensor
		} // loop all sensors

		return;
	}

	if(StrBeginsWith("SET THERMO ",command))
	{
		int ok;

		uint32_t cels=ParseHex(command+11,2,&ok);
		if(ok)
		{
			uint32_t cels_frag=ParseHex(command+14,2,&ok);
			if(ok)
			{
				settings.thermo_celcius = (int8_t)cels;
				settings.thermo_cel_frac_bits = (uint8_t)cels_frag;
				
				SettingsToFlash();
				console_SendString("OK\r\n");
				return;
			}
		}
		console_SendString("Hex parse fails\r\n");
	}
	
	if(StrBeginsWith("READ THERMO",command))
	{
		console_SendHex(settings.thermo_celcius,2);
		console_SendString(" ");
		console_SendHex(settings.thermo_cel_frac_bits,2);
		console_SendString("\r\n");
		return;
	}	
	
	if(StrBeginsWith("DS1820 CONF ",command))
	{
		int ok,i;
		uint8_t rom[OW_ROMCODE_SIZE];

		uint32_t addr=ParseHex(command+12,2,&ok);

		if(ok)
		{
			for(i=0;i<OW_ROMCODE_SIZE;i++)
			{
				rom[i]=(uint8_t)ParseHex(command+15+i*2,2,&ok);
				if(ok==0)
				break;
			}
			if(ok)
			{
				for(i=0;i<OW_ROMCODE_SIZE;i++)
					settings.ids[addr].id[i]=rom[i];

				SettingsToFlash();
				SetupTods1820Values();

				console_SendString("OK\r\n");
				return;
			}
		}
		console_SendString("Hex parse fails\r\n");
		return;
	}

	if(StrBeginsWith("DS1820 READ SENSORS",command))
	{
		int s,i;
		int retval=DS18X20ReadSensors(ds1820Values,3);
		if(retval==0)
		{
			update_Relay();
			console_SendString("OK:");
			for(s=0;s<3;s++)
			{
				for(i=0;i<OW_ROMCODE_SIZE;i++)
				console_SendHex(ds1820Values[s].id[i],2);
				console_SendString(":");
				console_SendHex(ds1820Values[s].celcius,4);
				console_SendString(":");	
				console_SendHex(ds1820Values[s].cel_frac_bits,4);
				console_SendString(":");			
			}
			console_SendString("\r\n");
		}
		else
			console_SendString("Error\r\n");			
		return;
	}
	
	console_SendString("Cannot parse command:");
	console_SendString(cmd);
	console_SendString("\r\n");
}

	int relayFilter = 0;

void update_Relay()
{
	if((ds1820Values[0].celcius == settings.thermo_celcius && ds1820Values[0].cel_frac_bits >= settings.thermo_cel_frac_bits) || ds1820Values[0].celcius > settings.thermo_celcius)
		relayFilter++;
	else
		relayFilter=0;
	
	if(relayFilter > 5)
	{
		relayFilter=5;
		ioport_set_pin_level(OUTRELAY1PIN, 1);		
	}
	else
		ioport_set_pin_level(OUTRELAY1PIN, 0);
	
}

void console_Poll()
{
	unsigned char ch;
	
	while(uart_is_rx_ready(UART))
	{
		uint8_t c;
		uart_read(UART,&c);
		rb_write(&RbRx,(unsigned char)c);
	}
	while (rb_has_data(&RbRx))
	{
		ch=rb_read(&RbRx);
		if(ch==13)
		{
			command[cmdPos]=0;
			ProcessCommand(command);
			cmdPos=0;
			continue;
		}
		command[cmdPos]=ch;
		cmdPos++;

		if(cmdPos==CMDLEN)
		cmdPos=0;
	}
	
	while(rb_has_data(&RbTx) && uart_is_tx_ready(UART))
	{
		ch=rb_read(&RbTx);
		uart_write(UART,ch);
	}
}

