#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "keyboard.h"
#include "kbd_types.h"
#include "get_key.h"


//#define KEYBOARD "/dev/keyboard/0raw"
#define KEYBOARD "/dev/mcu/kbd"
#define KEY_BUF_LEN 255

static void *read_keyboard(void * data);

char keybuf[KEY_BUF_LEN];
int pWrite=0; //write key buffer point 
int pRead =0;
int pHead =0;
int KEY_BUF_FULL=0;
//char cmd[255];


/**********************************************************/
int kbd_init()
{
	char key;
  	void * retval;
	if (KBD_Open() < 0) {
		printf("Can't open keyboard!\n");
		return -1;
	}
	return 0;

}
/**********************************************************/
int kbd_close()
{
	KBD_Close();
	return 0;
}
/**********************************************************/
//static void * read_keyboard(void * data)
char get_key(void)
{
	int keydown=0,old_keydown;
	char  key=0; 
	MWKEYMOD  modifiers;
	MWSCANCODE  scancode;
	while(1){
		keydown=KBD_Read(&key, &modifiers, &scancode); //block read
		if(keydown==1){//key press down  2: key up
/*			keybuf[pWrite++]=key;
			if( pWrite >= KEY_BUF_LEN){
				pWrite %= KEY_BUF_LEN;
				KEY_BUF_FULL=1;
			}
*/			
			return key;
		}
	}
}
/**********************************************************/
void get_line(char *cmd)
{
	int i=0;
	while(1){
		cmd[i]=get_key();
		
		if(cmd[i]==13){
			cmd[i]=0;
			break;
		}
		printf("%c",cmd[i]);
		fflush(stdout);
		i++;
	}

}

void get_number(char *cmd)
{
	int i=0;
	while(1){
		cmd[i]=get_key();
		
		if(cmd[i]==13){
			cmd[i]=0;
			break;
		}

		if(cmd[i]<'0' || cmd[i]>'9')
			continue;
		
		printf("%c",cmd[i]);
		fflush(stdout);
		i++;
	}

}
