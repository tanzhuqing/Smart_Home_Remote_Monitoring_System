/*
 * Microwindows keyboard driver for Compaq iPAQ
 *
 * Copyright (c) 2000, 2003 Century Software Embedded Technologies
 * Written by Jordan Crouse
 */
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "kbd_types.h"
#include "keyboard.h"


//#define KEYBOARD "/dev/keyboard/0raw"
#define KEYBOARD "/dev/mcu/kbd"

static int fd;

typedef struct{
	MWKEY mwkey;
	int scancode;
}KeyMap;

static MWKEY scancodes[64];

#define __I2C_MEGA8__
#ifdef __ZLG7289__
static KeyMap keymap[] = {
      {MWKEY_KP0,  0x1d}, 
      {MWKEY_KP1,  0x21}, 
      {MWKEY_KP2,  0x25}, 
      {MWKEY_KP3,  0x23}, 
      {MWKEY_KP4,  0x29}, 
      {MWKEY_KP5,  0x2d}, 
      {MWKEY_KP6,  0x2b}, 
      {MWKEY_KP7,  0x31}, 
      {MWKEY_KP8,  0x35}, 
      {MWKEY_KP9,  0x33}, 
      {MWKEY_NUMLOCK, 	0x11}, 
      {MWKEY_KP_DIVIDE,	0x15}, 
      {MWKEY_KP_MULTIPLY,	0x13}, 
      {MWKEY_KP_MINUS,	0x0b}, 
      {MWKEY_KP_PLUS,		0x34}, 
      {MWKEY_KP_ENTER,	0x24}, 
      {MWKEY_KP_DEL,	0x1b}, 
};
#else
#ifdef __I2C_MEGA8__
static KeyMap keymap[] = {		//update map policy
      {MWKEY_KP0,  0x0b}, 
      {MWKEY_KP1,  0x02}, 
      {MWKEY_KP2,  0x03}, 
      {MWKEY_KP3,  0x04}, 
      {MWKEY_KP4,  0x05}, 
      {MWKEY_KP5,  0x06}, 
      {MWKEY_KP6,  0x07}, 
      {MWKEY_KP7,  0x08}, 
      {MWKEY_KP8,  0x09}, 
      {MWKEY_KP9,  0x0a}, 
      
      {MWKEY_NUMLOCK, 	0x2a}, 
      {MWKEY_KP_DIVIDE,	0x35}, 
      {MWKEY_KP_MULTIPLY,	0x37}, 
      {MWKEY_KP_MINUS,	0x4a}, 
      {MWKEY_KP_PLUS,		0x4e}, 
      {MWKEY_KP_ENTER,	0x1c}, 
      {MWKEY_KP_DEL,	0x53}, 
};
/*
static KeyMap keymap[] = {
      {MWKEY_KP0,  0x3f}, 
      {MWKEY_KP1,  0x58}, 
      {MWKEY_KP2,  0x57}, 
      {MWKEY_KP3,  0x44}, 
      {MWKEY_KP4,  0x1}, 
      {MWKEY_KP5,  0xe}, 
      {MWKEY_KP6,  0x49}, 
      {MWKEY_KP7,  0x0}, 
      {MWKEY_KP8,  0x1c}, 
      {MWKEY_KP9,  0x51}, 
//      {MWKEY_NUMLOCK, 	0x11}, 
      {MWKEY_KP_DIVIDE,	0x48}, 
//      {MWKEY_KP_MULTIPLY,	0x13}, 
      {MWKEY_KP_MINUS,	0x4b}, 
      {MWKEY_KP_PLUS,		0x37}, 
      {MWKEY_KP_ENTER,	0x43}, 
      {MWKEY_KP_DEL,	0x3e}, 
};
*/
#endif //__I2C_MEGA8__
#endif //__ZLG7289__

int
KBD_Open(void)
{
	int i;

	/* Open the keyboard and get it ready for use */
	fd = open(KEYBOARD, O_RDONLY | O_NONBLOCK);

	if (fd < 0) {
		printf("%s - Can't open keyboard!\n", __FUNCTION__);
		return -1;
	}

	for (i=0; i<sizeof(scancodes)/sizeof(scancodes[0]); i++)
		scancodes[i]=MWKEY_UNKNOWN;

	for (i=0; i< sizeof(keymap)/sizeof(keymap[0]); i++)
		scancodes[keymap[i].scancode]=keymap[i].mwkey;

	return fd;
}


void
KBD_Close(void)
{
	close(fd);
	fd = -1;
}

void
KBD_GetModifierInfo(MWKEYMOD * modifiers, MWKEYMOD * curmodifiers)
{
	if (modifiers)
		*modifiers = 0;	/* no modifiers available */
	if (curmodifiers)
		*curmodifiers = 0;
}


int
KBD_Read(char* kbuf, MWKEYMOD * modifiers, MWSCANCODE * scancode)
{
	int keydown = 0;
	int cc = 0;
	char buf,key;

	cc = read(fd, &buf, 1);

	if (cc < 0) {
		if ((errno != EINTR) && (errno != EAGAIN)
		    && (errno != EINVAL)) {
			perror("KBD KEY");
			return (-1);
		} else {
			return (0);
		}
	}
	if (cc == 0)
		return (0);

	/* If the code is big than 127, then we know that */
	/* we have a key down.  Figure out what we've got */

	*modifiers = 0;

	if (buf & 0x80) {
		keydown = 1;	/* Key pressed but not released */
	} else {
		keydown = 2;	/* key released */
	}

	buf &= (~0x80);
//	if( buf >= sizeof(scancodes) ) *kbuf = MWKEY_UNKNOWN;
	*scancode = scancodes[(int) buf];
	*kbuf = *scancode ;
//	printf("%c",*kbuf);  	
//	printf("by threewater: orgvalue=%x key=%c %x keystatus=%d, scancode=%x\n",buf, *kbuf,*kbuf,keydown, *scancode);
	return keydown;
	
}
