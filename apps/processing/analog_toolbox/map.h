// $Id$
/*
 * MAP program module definitions
 *
 * ==========================================================================
 *
 *  Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
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
