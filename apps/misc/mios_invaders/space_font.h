// $Id: glcd_font.h 328 2008-05-05 19:35:12Z tk $
/*
 * Header File for GLCD Fonts
 *
 * ==========================================================================
 */

#ifndef _SPACE_FONT_H
#define _SPACE_FONT_H

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

extern code char SPACE_FONT[4];
extern code char SPACE_FONT_SPLASH[4];

#endif /* _GLCD_FONT_H */
