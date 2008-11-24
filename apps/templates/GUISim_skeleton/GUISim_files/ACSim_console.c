/*
 *  ACSim_console.c + Modified for GUISIM_windows - Andre 11/08/2008
 *  v 0.0.7
 *
 *  see header for version info and details
 */

 //////////////////////////////////////////////////////////////////////////
 // Andre 11/13/2008 - : GUI Modification Summary:
 //
 // ACSim_console.c - kept original logic from ACSim, but moved to GUISIM_windows
 // as the runloop logic had to be relocated inside a case statement.
 //
 // ACSim_mios.c functions modified to call GUI functions as appropriate
 //
 // GUISim_windows.c - Contains all GUI related logic for simulator
 //////////////////////////////////////////////////////////////////////////



// import standard I/O headers for console in- and output (printf and scanf)

//////////////////////////////////////////////////////////////////////////
//Andre: LCD_SDL.h contains all the required .h files and defines for GUI
//////////////////////////////////////////////////////////////////////////
#include <LCD_SDL.h>

#include <stdio.h>

#ifdef _DEBUG_OS_MAC
	#include <carbon/carbon.h>
#endif


// ACSim_mios.c contains all MIOS_FUNCTIONS() that are used throughout the program
// eg MIOS_MIDI_AIN_PinGet(0) returns some random numbers in a range 0..127
// open up ACSim_mios.c to tweak around some values...
// normally there should be no need to change the content of these files:
#include "ACSim_mios.h"

// toolbox, handy functions like random number generation, hex-output...
#include "ACSim_toolbox.h"


// import c files that are included in "main.h"
//#include "../ACMidi.c" // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ADD ADDITIONAL C.-FILES HERE!!!!

// import main c source file
#include "../../main.c"


/////////////////////////////////////////////////////////////////////////////////////////////////
// GUISIM_windows.c has the GUI logic, and I have included a modified runloop from ACSim's original
// to support his original text based commands in addition to the GUI interface
/////////////////////////////////////////////////////////////////////////////////////////////////
#include "GUISIM_windows.c"

int WINAPI WinMain (HINSTANCE hThisInstance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nCmdShow)
{
    LCD_SDL_Run();
    return 0;
}

int random(void) {

    return rand();

}
