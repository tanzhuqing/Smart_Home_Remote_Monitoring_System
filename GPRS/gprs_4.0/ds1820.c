//********************************************************************
//function:		ds1820 interface operation
//author:			Wang yong hui
//date:			2007-5-25
//modify log:
//*********************************************************************

//#include"ds1820_main.h"
#include<sys/time.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include "ds1820.h"
#define DEVICE_GPIOTEST "/dev/demo"
static int fd;
/*-------------------------------------------------------------*/
int gpioinit(void)
{
	
	
	fd=open(DEVICE_GPIOTEST,O_RDONLY | O_NONBLOCK);
	if(fd<0)
	{
		printf("can not open device");
		exit(1);
	}
}
/*-------------------------------------------------------------*/

void delay(int i)			//delay 1 micro_second
{
	while(i)
	{
		i--;
	}
}
/*-------------------------------------------------------------*/
void setbit(int i)
{
	ioctl(fd,i,0);
}
/*-------------------------------------------------------------*/
int setbit1(int i)
{
	int f;
	f=ioctl(fd,i,0);
	return f;
}
/*-------------------------------------------------------------*/
void resetchip(void)
{
	ioctl(fd,9,0);
	delay(45);	
	ioctl(fd,10,0);		//write 0 to F6
	delay(10000);
	ioctl(fd,9,0);		//write 1 to F6
	delay(90);
	

}
/*-------------------------------------------------------------*/
unsigned char chipcheck(void)
{
	
	int flag;
	resetchip();
	delay(1100);	
	flag=ioctl(fd,8,0);
	
		if(flag)
		{
			return FALSE;	
		}
	
		delay(2500);		//wait for 400us
		
		flag=ioctl(fd,8,0);
		if(flag)
		{
			return 	TRUE;
		}
		else
		{	
			return FALSE;
		}
	
}
/*-------------------------------------------------------------*/
int readbit(void)
{
	int bit;
	ioctl(fd,10,0);		//write 0 to F6
	delay(30);
	
	ioctl(fd,9,0);		//write 1 to F6
	delay(60);
	
	bit=ioctl(fd,8,0);	//read a bit from F6
	delay(900);
	return (bit);
}
/*-------------------------------------------------------------*/
int readbyte(void)
{
	int i,j;
	int byreaddata;
		for(i=0;i<8;i++)
	{
		j=readbit();
		byreaddata>>=1;
		byreaddata=(j<<7) | byreaddata;
	}
	return (byreaddata);
}
/*-------------------------------------------------------------*/
void writebit(int bit)
{
	if(bit)
	{
		ioctl(fd,10,0);		//write 0 to F6
		delay(60);
		ioctl(fd,9,0); 		//write 1 to F6
		delay(900);
		
	}
	else
	{
		ioctl(fd,10,0);		//write 0 to F6
		delay(900);
		ioctl(fd,9,0);		//write 1 to F6
		delay(60);
	}	
}
/*-------------------------------------------------------------*/
void writebyte(int data)
{
	int i,testbit;
	for(i=0;i<8;i++)
	{
		testbit=data>>i ;
		testbit&=0x01;
		writebit(testbit);
	}
}
/*-------------------------------------------------------------*/
void sendheaderdata(int *psn,int bycommand)
{
	int i;
	resetchip();
	delay(16000);
	
	#if(CHIP_NUM_IN_WIRE==1)
		writebyte(0xcc);
	#else
		writebyte(0x55);
		for(i=0;i<8;i++)
		{
			writebyte(*psn++); 	//send 64 bit serial number
		}
	#endif
	writebyte(bycommand);
}
/*------------------------------------------------------------------------*/
void rdscratchpad(unsigned char *psn,unsigned char *prddata,unsigned char bylen)
{
	int i;
	if(bylen>9)
	{
		bylen=9;
	}

	sendheaderdata((int *)psn,0xbe);
	
	for(i=0;i<bylen;i++)
	{
		*prddata++=readbyte();
	}
	
	resetchip();
	delay(16000);		//delay 1ms

}
/*----------------------------------------------------------------------*/
void wrscratchpad(unsigned char *psn,unsigned char *pwrdata,unsigned char bylen)
{
	int i;
	if(bylen>2)
	{
		bylen=2;
	}
	sendheaderdata((int *)psn,0x4e);

	for(i=0;i<8;i++)
	{
		writebyte(*pwrdata++);
	}
	
	resetchip();
	delay(16000);
}
/*----------------------------------------------------------------------*/
void startconverttmpt(unsigned char *psn)
{
	unsigned char bybusyflag=0;
	
	sendheaderdata((int *)psn,0x44);

	while(1)
	{
		bybusyflag=readbyte();
//		printf("-------------%d\n",bybusyflag);
		if(bybusyflag==0xff)
		{
			break;
		}
	}
	
	resetchip();
	delay(16000);
}
/*---------------------------------------------------------------------------*/ 	
signed short readtemperature(unsigned char *psn)
{
	unsigned char byrddata[8];
	unsigned char bytemp_read;
	unsigned char bycount_per_c;
	unsigned char bycount_remain;
	signed short t;
	float ft;
	resetchip();
	sendheaderdata((int *)psn,0x44);
	resetchip();	
	rdscratchpad(psn,byrddata,8);

	ft=((byrddata[0]+(byrddata[1]<<8))*0.06);
	t=ft-3;
	return t;

	#if 0
	t=byrddata[0]+(byrddata[1]<<8);
	bytemp_read=t>>1;
	bycount_remain=byrddata[6];
	bycount_per_c=byrddata[7];
	
	ft=(float)bytemp_read-0.25+(float)(bycount_per_c-bycount_remain)/(float)bycount_per_c;
	t=ft*10;
	return t;
	#endif
}
/*------------------------------------------------------------------------------*/
void readrom(unsigned char *psn)
{
	int i,j=0;
	resetchip();
	delay(16000);
	writebyte(0x33);
	for(i=0;i<8;i++)
	{	
		
		*psn++=readbyte();
		
	}


}
/*==================----------------------------------------------------------*/
/*--------------------------------end-------------------------------------*/

