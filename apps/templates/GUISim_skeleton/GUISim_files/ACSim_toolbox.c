/*
 *  ACSim_toolbox.c
 *  v 0.0.5
 *
 *  2006 April 17 Created by Michael Markert, audiocommander.de
 *  Copyright 2006 Michael Markert, http://www.audiocommander.de
 *  hexview based on publicdomain code: http://www.eggdrop.ch/texts/cschnipsel/
 *  toolbox for mios: http://www.midibox.org
 *
 */

/*
 * Released under GNU General Public License
 * http://www.gnu.org/licenses/gpl.html
 *
 * This program is free software; you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation
 *
 * YOU ARE ALLOWED TO COPY AND CHANGE
 * BUT YOU MUST RELEASE THE SOURCE TOO (UNDER GNU GPL) IF YOU RELEASE YOUR PRODUCT
 * YOU ARE NOT ALLOWED NOT USE IT WITHIN PROPRIETARY CLOSED-SOURCE PROJECTS
 */



#include "ACSim_toolbox.h"

#include "ACSim_console.h"




// generate some random numbers
unsigned char ACRandomBool(void)	{ return (rand() % 2); }
signed char   ACRandomEncoder(void)	{ return ((rand() % 2) ? -1 : 1 ); }
unsigned char ACRandomPin(void)		{ return (rand() % DEBUG_AIN_NUM); }
unsigned char ACRandomChar(void)	{ return (rand() % 128); }
unsigned int  ACRandomInt(void)		{ return (rand() % 1024); }




// hexview output
// maybe configure defines in header!

void hexview(unsigned char *buf, int size) {
	/*	usage:
	char buf[256];
	strcpy(buf, "some data");
	hexview(buf, sizeof(buf));  */

	int i;
	unsigned char c;
	unsigned char r = 0;

	while (size>0) {
		printf("%i:\t\t",(r*16));
		for (i=0;i<HEXVIEW_CHARS;i++) {
			if (size-i>0) {
				printf("%02x ", (unsigned char)*(buf+i));
			} else {
				printf("   ");
			}
		}
		printf(" ");
		for (i=0;i<HEXVIEW_CHARS&&size-i>0;i++) {
			c = (unsigned char)*(buf+i);
			printf("%c", c>=32&&c<127 ? c : '.');
		}
		buf+=HEXVIEW_CHARS;
		size-=HEXVIEW_CHARS;
		printf("\n");
		r++;
	}
}


