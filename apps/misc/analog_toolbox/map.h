// $Id$
/*
 * MAP program module definitions
 *
 * ==========================================================================
 *
 * Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
 * 
 * ==========================================================================
 * 
 * This file is part of a MIOS application
 *
 * This application is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with This application; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ==========================================================================
 */

#ifndef _MAP_H
#define _MAP_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

#define CONV_16BIT_TO_12BIT(val) ((val) >> 4)
#define CONV_16BIT_TO_8BIT(val)  ((val) >> 8)

#define CONV_10BIT_TO_12BIT(val) ((val) << 2)
#define CONV_10BIT_TO_11BIT(val) ((val) << 1)
#define CONV_10BIT_TO_9BIT(val)  ((val) >> 1)
#define CONV_10BIT_TO_8BIT(val)  ((val) >> 2)
#define CONV_10BIT_TO_7BIT(val)  ((val) >> 3)

#define CONV_9BIT_TO_12BIT(val)  ((val) << 3)
#define CONV_9BIT_TO_11BIT(val)  ((val) << 2)
#define CONV_9BIT_TO_10BIT(val)  ((val) << 1)
#define CONV_9BIT_TO_8BIT(val)   ((val) >> 1)
#define CONV_9BIT_TO_7BIT(val)   ((val) >> 2)

#define CONV_8BIT_TO_12BIT(val)  ((val) << 4)
#define CONV_8BIT_TO_11BIT(val)  ((val) << 3)
#define CONV_8BIT_TO_10BIT(val)  ((val) << 2)
#define CONV_8BIT_TO_9BIT(val)   ((val) << 1)
#define CONV_8BIT_TO_7BIT(val)   ((val) >> 1)

#define CONV_7BIT_TO_12BIT(val)  ((val) << 5)
#define CONV_7BIT_TO_11BIT(val)  ((val) << 4)
#define CONV_7BIT_TO_10BIT(val)  ((val) << 3)
#define CONV_7BIT_TO_9BIT(val)   ((val) << 2)
#define CONV_7BIT_TO_8BIT(val)   ((val) << 1)

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void MAP_Init(void);
void MAP_Tick(void);

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

#endif /* _MAP_H */
