// $Id: glcd_font.h 328 2008-05-05 19:35:12Z tk $
/*
 * Header File for GLCD Fonts
 *
 * ==========================================================================
 */

#ifndef _GLCD_FONT_H
#define _GLCD_FONT_H

/////////////////////////////////////////////////////////////////////////////
// Defines array pointers to all available fonts
//
// (hope that SDCC ignores predefined constants which are not
// compiled into the project)
//
// Pointers are defined as 4-byte arrays. Actually, this is only the font
// header, the bitmap data follows after the header, but is not counted
// here (not required)
/////////////////////////////////////////////////////////////////////////////

extern code char GLCD_FONT_BIG[4];
extern code char GLCD_FONT_SMALL[4];
extern code char GLCD_FONT_NORMAL[4];
extern code char GLCD_FONT_KNOB_ICONS[4];
extern code char GLCD_FONT_METER_ICONS_H[4];
extern code char GLCD_FONT_METER_ICONS_V[4];

#endif /* _GLCD_FONT_H */
