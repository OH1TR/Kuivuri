#include <asf.h>
#include "delay.h"

void delay_us(uint16_t delay)
{

        uint32_t now=SysTick->VAL;
        uint32_t target=now - delay * 96; // 96 000 000/1000000
        target &= 0x00FFFFFF;

        if(target>now)
                while(SysTick->VAL<now);

        while(SysTick->VAL>target);
		
}

void delay_ms(uint16_t delay)
{

        uint32_t now=SysTick->VAL;
        uint32_t target=now - delay * 96000; // 96 000 000/1000
        target &= 0x00FFFFFF;

        if(target>now)
                while(SysTick->VAL<now);

        while(SysTick->VAL>target);		
}


void delay_ms_init(uint16_t delay,uint32_t *handle)
{
        uint32_t now=SysTick->VAL;
        uint32_t target=now - delay * 96000; // 96 000 000/1000
        target &= 0x00FFFFFF;
        if(target>now)
        	target|=0x80000000;
        *handle=target;


int delay_is_done(uint32_t *handle)
{
	if(target & 0x80000000)
	{
		if(SysTick->VAL>now)
			*handle&=0x7fffffff;
		else
			return(0);
	}
	if(SysTick->VAL<target)
		return(1);
	else
		return(0);
	}
}

