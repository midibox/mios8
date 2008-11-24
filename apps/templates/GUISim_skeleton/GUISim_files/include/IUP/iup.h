/** \file
 * \brief User API
 * IUP - A Portable User Interface Toolkit
 * Tecgraf: Computer Graphics Technology Group, PUC-Rio, Brazil
 * http://www.tecgraf.puc-rio.br/iup	mailto:iup@tecgraf.puc-rio.br
 *
 * See Copyright Notice at the end of this file
 */
 
#ifndef __IUP_H 
#define __IUP_H

#include <iupkey.h>
#include <iupdef.h>

#ifdef __cplusplus
extern "C" {
#endif


#define IUP_NAME "IUP - Portable User Interface"
#define IUP_COPYRIGHT	"Copyright (C) 1994-2008 Tecgraf, PUC-Rio."
#define IUP_DESCRIPTION	"Portable toolkit for building graphical user interfaces."
#define IUP_VERSION "2.7.0"
#define IUP_VERSION_DATE "2008/10/14"
#define IUP_VERSION_NUMBER 207000

typedef struct Ihandle_ Ihandle;
typedef int (*Icallback)(Ihandle*);

/************************************************************************/
/*                      pre-definided dialogs                           */
/************************************************************************/
Ihandle* IupFileDlg(void);

int  IupGetFile    (char *arq);
void IupMessage    (const char *title, const char *msg);
void IupMessagef   (const char *title, const char *format, ...);
int  IupAlarm      (const char *title, const char *msg, const char *b1, const char *b2, const char *b3);
int  IupScanf      (const char *format, ...);
int  IupListDialog (int type, const char *title, int size, const char *lst[],
                    int op, int col, int line, int marks[]);
int  IupGetText    (const char* title, char* text);

/************************************************************************/
/*                      Functions                                       */
/************************************************************************/

int       IupOpen          (int *argc, char ***argv);
void      IupClose         (void);

int       IupMainLoop      (void);
int       IupLoopStep      (void);
void      IupFlush         (void);
void      IupExitLoop      (void);

void      IupUpdate        (Ihandle* ih);
void      IupRefresh       (Ihandle* ih);
char*     IupMapFont       (const char *iupfont);
char*     IupUnMapFont     (const char *driverfont);
int       IupHelp          (const char* url);
char *    IupLoad          (const char *filename);

char*     IupVersion       (void);
char*     IupVersionDate   (void);
int       IupVersionNumber (void);
void      IupSetLanguage   (const char *lng);
char*     IupGetLanguage   (void);

void      IupDestroy       (Ihandle *ih);
void      IupDetach        (Ihandle *child);
Ihandle*  IupAppend        (Ihandle *ih, Ihandle *child);
Ihandle*  IupGetNextChild  (Ihandle *ih, Ihandle *child);
Ihandle*  IupGetBrother    (Ihandle *ih);
Ihandle*  IupGetParent     (Ihandle *ih);
Ihandle*  IupGetDialog     (Ihandle *ih);

int       IupPopup         (Ihandle *ih, int x, int y);
int       IupShow          (Ihandle *ih);
int       IupShowXY        (Ihandle *ih, int x, int y);
int       IupHide          (Ihandle *ih);
int       IupMap           (Ihandle *ih);

void      IupSetAttribute  (Ihandle *ih, const char* name, const char* value);
void      IupStoreAttribute(Ihandle *ih, const char* name, const char* value);
Ihandle*  IupSetAttributes (Ihandle *ih, const char *str);
char*     IupGetAttribute  (Ihandle *ih, const char* name);
char*     IupGetAttributes (Ihandle *ih);
int       IupGetInt        (Ihandle *ih, const char* name);
int       IupGetInt2       (Ihandle *ih, const char* name);
float     IupGetFloat      (Ihandle *ih, const char* name);
void      IupSetfAttribute (Ihandle *ih, const char* name, const char* format, ...);

void      IupSetGlobal     (const char* name, const char* value);
void      IupStoreGlobal   (const char* name, const char* value);
char*     IupGetGlobal     (const char* name);

Ihandle*  IupSetFocus      (Ihandle *ih);
Ihandle*  IupGetFocus      (void);
Ihandle*  IupPreviousField  (Ihandle *ih);  
Ihandle*  IupNextField      (Ihandle *ih);

Icallback IupGetCallback(Ihandle *ih, const char *name);
Icallback IupSetCallback(Ihandle *ih, const char *name, Icallback func);
Ihandle*  IupSetCallbacks(Ihandle* ih, ...);

Icallback   IupGetFunction   (const char *name);
Icallback   IupSetFunction   (const char *name, Icallback func);
const char* IupGetActionName (void);

Ihandle*  IupGetHandle     (const char *name);
Ihandle*  IupSetHandle     (const char *name, Ihandle *ih);
int       IupGetAllNames   (char *names[], int n);
int       IupGetAllDialogs (char *names[], int n);
char*     IupGetName       (Ihandle* ih);

void      IupSetAttributeHandle (Ihandle *ih, const char* name, Ihandle *ih_named);
Ihandle*  IupGetAttributeHandle(Ihandle *ih, const char* name);

char*     IupGetClassName  (Ihandle *ih);

Ihandle*  IupCreate        (const char *name);
Ihandle*  IupCreatev       (const char *name, void* *params);
Ihandle*  IupCreatep       (const char *name, void* first, ...);

Ihandle*  IupFill          (void);
Ihandle*  IupRadio         (Ihandle *child);
Ihandle*  IupVbox       (Ihandle* child, ...);
Ihandle*  IupVboxv      (Ihandle* *children);
Ihandle*  IupZbox       (Ihandle* child, ...);
Ihandle*  IupZboxv      (Ihandle* *children);
Ihandle*  IupHbox       (Ihandle* child,...);
Ihandle*  IupHboxv      (Ihandle* *children);


Ihandle*  IupFrame         (Ihandle* child);

Ihandle*  IupImage         (int width, int height, const unsigned char *pixmap);
Ihandle*  IupButton        (const char* title, const char* action);
Ihandle*  IupCanvas        (const char* action);
Ihandle*  IupDialog        (Ihandle* child);
Ihandle*  IupUser          (void);
Ihandle*  IupItem          (const char* title, const char* action);
Ihandle*  IupSubmenu       (const char* title, Ihandle* child);
Ihandle*  IupSeparator     (void);
Ihandle*  IupLabel         (const char* title);
Ihandle*  IupList          (const char* action);
Ihandle*  IupMenu          (Ihandle *child,...);
Ihandle*  IupMenuv         (Ihandle **children);
Ihandle*  IupText          (const char* action);
Ihandle*  IupMultiLine     (const char* action);
Ihandle*  IupToggle        (const char* title, const char* action);
Ihandle*  IupTimer         (void);

#ifdef __cplusplus
}
#endif

/************************************************************************/
/*                   Common Return Values                               */
/************************************************************************/
#define IUP_ERROR     1
#define IUP_NOERROR   0
#define IUP_OPENED    -1
#define IUP_INVALID   -1

/************************************************************************/
/*                   Callback Return Values                             */
/************************************************************************/
#define IUP_IGNORE    -1
#define IUP_DEFAULT   -2
#define IUP_CLOSE     -3
#define IUP_CONTINUE  -4

/************************************************************************/
/*           IupPopup and IupShowXY Parameter Values                    */
/************************************************************************/
#define IUP_CENTER    0xFFFF  /* 65535 */
#define IUP_LEFT      0xFFFE  /* 65534 */
#define IUP_RIGHT     0xFFFD  /* 65533 */
#define IUP_MOUSEPOS  0xFFFC  /* 65532 */
#define IUP_CURRENT   0xFFFB  /* 65531 */
#define IUP_CENTERPARENT  0xFFFA
#define IUP_TOP       IUP_LEFT
#define IUP_BOTTOM    IUP_RIGHT

/************************************************************************/
/*               SHOW_CB Callback Values                                */
/************************************************************************/
enum{IUP_SHOW, IUP_RESTORE, IUP_MINIMIZE, IUP_MAXIMIZE, IUP_HIDE};

/************************************************************************/
/*               SCROLL_CB Callback Values                              */
/************************************************************************/
enum{IUP_SBUP,   IUP_SBDN,    IUP_SBPGUP,   IUP_SBPGDN,    IUP_SBPOSV, IUP_SBDRAGV, 
     IUP_SBLEFT, IUP_SBRIGHT, IUP_SBPGLEFT, IUP_SBPGRIGHT, IUP_SBPOSH, IUP_SBDRAGH};

/************************************************************************/
/*               Mouse Button Values and Macros                         */
/************************************************************************/
#define IUP_BUTTON1   '1'
#define IUP_BUTTON2   '2'
#define IUP_BUTTON3   '3'
#define IUP_BUTTON4   '4'
#define IUP_BUTTON5   '5'

#define isshift(_s)    (_s[0]=='S')
#define iscontrol(_s)  (_s[1]=='C')
#define isbutton1(_s)  (_s[2]=='1')
#define isbutton2(_s)  (_s[3]=='2')
#define isbutton3(_s)  (_s[4]=='3')
#define isdouble(_s)   (_s[5]=='D')
#define isalt(_s)      (_s[6]=='A')
#define issys(_s)      (_s[7]=='Y')
#define isbutton4(_s)  (_s[8]=='4')
#define isbutton5(_s)  (_s[9]=='5')


/************************************************************************/
/*              Replacement for the WinMain in Windows,                 */
/*        this allows the application to start from "main".             */
/************************************************************************/
#if defined (__WATCOMC__) || defined (__MWERKS__)
#ifdef __cplusplus
extern "C" {
int IupMain (int argc, char** argv); /* In C++ we have to declare the prototype */
}
#endif
#define main IupMain /* this is the trick for Watcom and MetroWerks */
#endif

/******************************************************************************
* Copyright (C) 1994-2008 Tecgraf, PUC-Rio.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#endif
