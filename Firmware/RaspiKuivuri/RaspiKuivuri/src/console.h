#ifndef _CONSOLE_H
#define _CONSOLE_H

void IntToDec(char *buffer,uint32_t val,int digits);
void IntToHex(char *buffer,uint32_t val,int digits);
void console_SendString(const char *msg);
int console_Setup(uint16_t bps);

void console_SendHex(uint32_t val,int digits);
int StrBeginsWith(const char *begin,char *str);

int console_DataReceived(unsigned char *data,int len);
uint32_t ParseHex(char *str,int count,int *ok);
void DumpMem(uint32_t address);
void ProcessCommand(char *cmd);
void update_Relay(void);

void console_Poll(void);

#endif


