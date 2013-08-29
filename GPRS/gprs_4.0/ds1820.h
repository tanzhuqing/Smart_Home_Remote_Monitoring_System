//**********************************************************************
//function:		ds1820 interface
//author:		microbird
//date:			2007-5-25
//ver:			v1.0
//modify:
//***********************************************************************
#ifndef __DS1820_H__
#define __DS1820_H__
#endif

#ifndef TRUE
#define	TRUE	1
#define FALSE 	0
//#endif

#define CHIP_NUM_IN_WIRE	1

int gpioinit();
unsigned char chipcheck(void);
void readrom(unsigned char *psn);
void startconverttmpt(unsigned char *psn);
void delay(int i);
int setbit1(int i);
void setbit(int i);
unsigned char wrtmptalarmlim(unsigned char *psn,signed short nhighalarmlim,signed short nlowalarmlim);
signed short rdtemperature(unsigned char *psn);
#endif
