/*
 *  ACSim_toolbox.h
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
 * YOU ARE NOT ALLOWED TO USE IT WITHIN PROPRIETARY CLOSED-SOURCE PROJECTS
 */


#import <Stdio.h>


#ifndef _ACSIM_TOOLBOX_H
#define _ACSIM_TOOLBOX_H



// generate some random numbers
extern unsigned char ACRandomBool(void);
extern signed char   ACRandomEncoder(void);
extern unsigned char ACRandomPin(void);
extern unsigned char ACRandomChar(void);
extern unsigned int  ACRandomInt(void);



// hexview output
#define HEXVIEW_COLUMNS 68
#define HEXVIEW_CHARS ((HEXVIEW_COLUMNS-1)/4)

extern void hexview(unsigned char *buf, int size);


#endif	/* _ACSIM_TOOLBOX_H */
