/*
 *  ACToolbox.h
 *  kII
 *
 *  Created by audiocommander on 25.02.07.
 *  Copyright 2006 Michael Markert, http://www.audiocommander.de
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
 *
 */


#ifndef _ACTOOLBOX_H
#define _ACTOOLBOX_H

#ifdef _DEBUG_C
	#include "ACSim_toolbox.h"
#else
	#include <cmios.h>
	#include <pic18fregs.h>
#endif


// **** globals ****

extern unsigned char random_seed_l;
extern unsigned char random_seed_h;



// **** prototypes ****

extern unsigned int  ACMath_Divide(unsigned int a, unsigned int b) __wparam;
extern unsigned char ACMath_Scale(unsigned char v, unsigned char min, unsigned char max) __wparam;

extern unsigned char ACMath_Random(void);
extern unsigned char ACMath_RandomInRange(unsigned char rmax) __wparam;

#endif /* _ACTOOLBOX_H */
