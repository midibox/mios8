/*
 * MIOS Invaders 
 * 
 * This is a MIOS version of the classic Space Invaders Game
 * based on a PROTON+ BASIC program Written by Les Johnson 2003.
 * For the orignal software visit: (licensed under a non-commerial license)
 * http://users.picbasic.org/projects/Space%20Invaders/space_invaders.htm
 *
 * ==========================================================================
 *
 *  Copyright (C) 2009 Phil Taylor (phil@taylor.org.uk)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#undef DEBUG
 
#ifdef DEBUG
#include <debug_msg.h> 
#endif

#include "cmios.h"
#include "pic18fregs.h"

#include "space_font.h"
#include "invader.h"


unsigned char flash_count=0;
unsigned char cycle_count=0;

game_status_t game; 	// Various bits used for game status information
invader_status_t invader;	// More bits used for invader game.
        
// Space invader variables		            
unsigned char base_enabled[9];	//Whether or not the section of the BASE is enabled
unsigned char base_hits[9];		//Holds the amount of hits each part of the bases has had
unsigned char invader_xpos[18];  //Array to hold the space invader's X position
unsigned char invader_ypos[18]; //Array to hold the space invader's Y position
unsigned char invader_enabled[18]; //Array to hold the space invader' config. i.e. hit or active
        
#pragma udata lcdmem lcd_buffer		
unsigned char lcd_buffer[1024];
unsigned char button_pressed=0;
unsigned char ship_xpos;	// X position of BASE ship
unsigned char missile_ypos;	//Y position of Ship's MISSILE
unsigned char missile_xpos;	//X position of Ship's MISSILE
unsigned char missile_status;
unsigned char ship_speed;	
unsigned char invader_tick; // Constant tick within program
unsigned char invader_loop; // Scans the invader arrays
unsigned char invader_missile_tick; 
unsigned char ship_missile_tick;
unsigned char temp_loop; 		
unsigned char invaders_enabled; 	// Count the INVADERS enabled
unsigned char invader_speed; 	// The speed of the invaders
unsigned char base_xpos_test;	// Used for detecting a missile hit on a BASE
unsigned char invader_missile_ypos;	// Y position of Invader's MISSILE
unsigned char invader_missile_xpos;	// X position of Invader's MISSILE
unsigned char invader_missile_speed; // The speed of the invader's missile. This slows down as invader near the bottom of the screen
unsigned char default_invader_speed; // Saves the initial speed of the invaders
        
unsigned char invader_missile2_tick; 
unsigned char invader_missile2_ypos;	// Y position of Invader's second MISSILE
unsigned char invader_missile2_xpos;	// X position of Invader's second MISSILE
unsigned char invader_missile2_speed; // The speed of the invader's second missile. This slows down as invader near the bottom of the screen
int invader_character;	// This selects the primary or secondary INVADER Character

unsigned char missile_char;
        
unsigned char saucer_xpos;
unsigned char saucer_speed; 		
unsigned int score;
unsigned int level;
unsigned char lives;

unsigned char random_value;
/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // select LCD type #7
  // (custom LCD driver, hooks in $MIOS_PATH/modules/app_lcd/ks0108/app_lcd.asm module will be used)
  MIOS_LCD_TypeSet(0x07, 0x00, 0x00);
  // Setup Digital IN, this is a single AIN4.
  MIOS_SRIO_UpdateFrqSet(1); // ms
  MIOS_SRIO_NumberSet(4);
  MIOS_SRIO_DebounceSet(10);
  // If the second value is 0x01, chip select line polarity will be inverted
  // The third value is not used by the driver
  game.CURRENT_SCREEN=SPLASH_SCREEN; // Select the splash screen initially.
  //game.CURRENT_SCREEN=SCORE_PAGE; // Select the splash screen initially.
  INVADER_New_Game(); // setup new game defaults.
#ifdef DEBUG
  // only for debugging - remove this (or change "#if 1" to "#if 0") in a common application!
  DEBUG_MSG_SendHeader();
  DEBUG_MSG_SendCString("Starting MIOS Invader");
  DEBUG_MSG_SendFooter();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{

}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when the display content should be 
// initialized. Thats the case during startup and after a temporary message
// has been printed on the screen
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Init(void) __wparam
{
}

void INVADER_New_Game(void)
{
	level = 0; // Start at level 0								
	lives = 3;	//Set the initial lives counter to 3
	ship_speed = 3;
	score = 0;
	ship_xpos = 64;
	saucer_speed = 7;	//Speed of the saucer
	invader_speed = 70;// the initial speed of the invaders
	default_invader_speed = invader_speed;	
	temp_loop = 0;
	for (temp_loop=0;temp_loop<9;temp_loop++)
	{
		base_enabled[temp_loop] = 1;
		base_hits[temp_loop] = 0; 
	}
}

void INVADER_Screen_Intro(void)
{
  unsigned char f;
  MIOS_LCD_Clear();

 // Intiially I tried to create the font as 128x8 but when displayed
 // There was a blank line between each printed line.
 // A 16x1 font seems to work much better.
  MIOS_GLCD_FontInit(SPACE_FONT_SPLASH);    
  MIOS_GLCD_GCursorSet(  0,0);
  for (f=0;f<64;f++)
  {
	if (f==8)
		MIOS_GLCD_GCursorSet(  0,1);
	else if (f==16)
		MIOS_GLCD_GCursorSet(  0,2);
	else if (f==24)
		MIOS_GLCD_GCursorSet(  0,3);
	else if (f==32)
		MIOS_GLCD_GCursorSet(  0,4);
	else if (f==40)
		MIOS_GLCD_GCursorSet(  0,5);
	else if (f==48)
		MIOS_GLCD_GCursorSet(  0,6);
	else if (f==56)
		MIOS_GLCD_GCursorSet(  0,7);
	MIOS_LCD_PrintChar(f);
  }
}

void INVADER_Screen_Scoring(void)
{
	unsigned char xpos,ypos;
	MIOS_LCD_Clear();
	for(ypos=0;ypos<8;ypos++)
	{
		for (xpos=0;xpos<113;xpos=xpos+14)
		{
			if ((xpos==0 || xpos==112) || (ypos==0 || ypos==7))
			{
				MIOS_GLCD_GCursorSet(xpos,ypos);
				MIOS_LCD_Data(0x00);
				MIOS_LCD_Data(0x04);
				MIOS_LCD_Data(0x0e);
				MIOS_LCD_Data(0x3a);
				MIOS_LCD_Data(0x1b);
				MIOS_LCD_Data(0x0f);
				MIOS_LCD_Data(0x1b);
				MIOS_LCD_Data(0x0f);
				MIOS_LCD_Data(0x1b);
				MIOS_LCD_Data(0x0f);
				MIOS_LCD_Data(0x1b);
				MIOS_LCD_Data(0x3a);
				MIOS_LCD_Data(0x0e);
				MIOS_LCD_Data(0x04);
				MIOS_LCD_Data(0x00);
			}
		}
	}
	MIOS_GLCD_GCursorSet(38,1);
	MIOS_LCD_PrintCString("Scoring");

	MIOS_GLCD_GCursorSet(18,2);
	MIOS_LCD_Data(0x00);
	MIOS_LCD_Data(0x04);
	MIOS_LCD_Data(0x0e);
	MIOS_LCD_Data(0x3a);
	MIOS_LCD_Data(0x1b);
	MIOS_LCD_Data(0x0f);
	MIOS_LCD_Data(0x1b);
	MIOS_LCD_Data(0x0f);
	MIOS_LCD_Data(0x1b);
	MIOS_LCD_Data(0x0f);
	MIOS_LCD_Data(0x1b);
	MIOS_LCD_Data(0x3a);
	MIOS_LCD_Data(0x0e);
	MIOS_LCD_Data(0x04);
	MIOS_LCD_Data(0x00);
	MIOS_GLCD_GCursorSet(36,2);
	MIOS_LCD_PrintCString(" =100+ Points");

	MIOS_GLCD_GCursorSet(18,3);
	MIOS_LCD_Data(0x00);
	MIOS_LCD_Data(0x18);
	MIOS_LCD_Data(0x4c);
	MIOS_LCD_Data(0x2e);
	MIOS_LCD_Data(0x5b);
	MIOS_LCD_Data(0x2f);
	MIOS_LCD_Data(0x2f);
	MIOS_LCD_Data(0x5b);
	MIOS_LCD_Data(0x2e);
	MIOS_LCD_Data(0x4c);
	MIOS_LCD_Data(0x18);
	MIOS_LCD_Data(0x00);
   	MIOS_GLCD_GCursorSet(36,3);
	MIOS_LCD_PrintCString(" =20 Points");

	
	MIOS_GLCD_GCursorSet(18,4);
	MIOS_LCD_Data(0x00);
	MIOS_LCD_Data(0x18);
	MIOS_LCD_Data(0x4c);
	MIOS_LCD_Data(0x2e);
	MIOS_LCD_Data(0x5b);
	MIOS_LCD_Data(0x2f);
	MIOS_LCD_Data(0x2f);
	MIOS_LCD_Data(0x5b);
	MIOS_LCD_Data(0x2e);
	MIOS_LCD_Data(0x4c);
	MIOS_LCD_Data(0x18);
	MIOS_LCD_Data(0x00);
	MIOS_GLCD_GCursorSet(36,4);
	MIOS_LCD_PrintCString(" =10 Points");

	MIOS_GLCD_GCursorSet(18,5);
	MIOS_LCD_Data(0x00);
	MIOS_LCD_Data(0x4c);
	MIOS_LCD_Data(0x6e);
	MIOS_LCD_Data(0x3a);
	MIOS_LCD_Data(0x1b);
	MIOS_LCD_Data(0x2f);
	MIOS_LCD_Data(0x2f);
	MIOS_LCD_Data(0x1b);
	MIOS_LCD_Data(0x3a);
	MIOS_LCD_Data(0x6e);
	MIOS_LCD_Data(0x4c);
	MIOS_LCD_Data(0x00);
	MIOS_GLCD_GCursorSet(36,5);
	MIOS_LCD_PrintCString(" =5 Points");
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
// As everything about this program is about LCD display, this is effectively the MAINLOOP
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
	unsigned char f;
	if (game.CURRENT_SCREEN != GAME_RUNNING) // If game is not running, FIRE_BUTTON starts new game
	{
		if (button_pressed==FIRE_BUTTON)
		{
			MIOS_LCD_Cmd(0xa6); // Set NORMAL characters. (we may be in GAME_OVER)
		    game.CURRENT_SCREEN=GAME_RUNNING;
			INVADER_New_Sheet(); // Draw the first sheet.
			button_pressed==0; // Clear fire button.
			MIOS_Delay(200);
			return;
		}
	}
	
	if (game.CURRENT_SCREEN==GAME_OVER)
	{
		if (!flash_count)
			MIOS_LCD_Cmd(0xa7); // Set NORMAL characters. (we may be in GAME_OVER)
		else if (flash_count==128)
		{
			cycle_count++;
			MIOS_LCD_Cmd(0xa6); // Set NORMAL characters. (we may be in GAME_OVER)
		}
		if (cycle_count>8)
		{
			MIOS_LCD_Cmd(0xa6); // Set NORMAL characters. (we may be in GAME_OVER)
			game.CURRENT_SCREEN=SPLASH_SCREEN;
			cycle_count=0;
		}
		MIOS_Delay(8);
	}
	if (game.CURRENT_SCREEN==SPLASH_SCREEN)
	{
		if (!flash_count)
			INVADER_Screen_Intro();
		else if (flash_count==128)
		{
			cycle_count++;
			MIOS_GLCD_FontInit(SPACE_FONT);    
			MIOS_GLCD_GCursorSet(  6,4);
			MIOS_LCD_PrintCString("PRESS FIRE TO START");	
		}
		if (cycle_count>4)
		{
			game.CURRENT_SCREEN=SCORE_PAGE;
			cycle_count=0;
		}
		MIOS_Delay(8);
	}
	else if (game.CURRENT_SCREEN==SCORE_PAGE)
	{
		MIOS_GLCD_FontInit(SPACE_FONT);    
		if (!flash_count) {
			INVADER_Screen_Scoring();
		} else if (flash_count==128) {
			cycle_count++;
			MIOS_GLCD_GCursorSet(7,6);
			MIOS_LCD_PrintCString("PRESS FIRE TO START");
		}
		if (cycle_count>4)
		{
			game.CURRENT_SCREEN=SPLASH_SCREEN;
			cycle_count=0;
		}
		MIOS_Delay(8);
	}
	else if (game.CURRENT_SCREEN==GAME_RUNNING)
	{
		// Main Game Running Loop
		MIOS_GLCD_FontInit(SPACE_FONT);    
		//random_value = rand();
        random_value = random_value & 0b00111000;
        invader_tick++;
        invader.TIME_TO_MOVE_INVADERS = 0;
        if (invader_tick > invader_speed)
		{
			invader_tick = 0;
			INVADER_Move_Invaders();
            INVADER_Move_Invaders();
            invader_character = ~invader_character;	// Use a new invader character
            if (invader.INVADERS_REACHED_BOTTOM)
			{
				// Game over
				INVADER_Game_Over();
				return;
			}
		}
		INVADER_Move_Saucer();	//Move the flying saucer (if required)
            
        INVADER_Govern_Speeds();  //Check whether a piece should be moving
            
        if(button_pressed==L_BUTTON)
			INVADER_Ship_Left();  					// Move ship left if LEFT button pressed
        if (button_pressed==R_BUTTON)
			INVADER_Ship_Right();					// Move ship right if RIGHT button pressed

		if (button_pressed==FIRE_BUTTON)
		{
			game.MISSILE_FIRED=1;
			missile_xpos=ship_xpos+4;
			 //	MISSILE_SOUND_ENABLE = 1							' Enable the missile's sound
		}

		if (button_pressed==PAUSE_BUTTON)
		{
			game.CURRENT_SCREEN=GAME_PAUSED;
		}
					
        INVADER_Move_Missile(); // Move the ship's missile if OK to do so
		INVADER_Check_For_Missile_Hit(); //Check if the ship's missile has hit anything
            
        if (game.MISSILE_HIT || game.SAUCER_HIT)
		{
           	if (!game.BASE_HIT)
			{
				MIOS_GLCD_FontInit(SPACE_FONT); 
				MIOS_GLCD_GCursorSet(0,0);
				MIOS_LCD_PrintBCD1(level);
				MIOS_LCD_PrintCString(" ");
				MIOS_LCD_PrintBCD5(score);
				MIOS_GLCD_GCursorSet(120,0);
				MIOS_LCD_PrintBCD1(lives);         
			}
        }
            
       	if (invader_character)
	        INVADER_Fire_Invader_Missile(); // Fire an invader missile is possible
        INVADER_Move_Invader_Missile();	// Move the invader's missile (if fired)
        INVADER_Check_For_Invader_Missile_Hit(); // Check if the invader's missile has hit anything
            
        if (invaders_enabled == 0)// Have all the invaders been destroyed ?
		{
			invaders_enabled = 18;	//Yes.. So enable them all again
            if (invader_speed > 2)
				invader_speed--; // Increase the speed of the invaders
            INVADER_New_Sheet(); // and start a new sheet
		}
        if (level > 10) //Add a second invader missile after level 10
		{
			if (invader_character == 0)
				INVADER_Fire_Invader_Missile2(); //Fire an invader missile is possible
           	INVADER_Move_Invader_Missile2(); // Move the invader's missile (if fired)
           	INVADER_Check_For_Invader_Missile2_Hit(); //Check if the invader's missile has hit anything
		}

        if (game.SHIP_HIT) // Has the ship been hit ?
		{
           	game.SHIP_HIT = 0;
			//    SAUCER_SOUND_ENABLE = 1								' Yes. So enable SAUCER sound channel
			//	MISSILE_SOUND_ENABLE = 1       						' Enable MISSILE sound channel
			//	MISSILE_FREQ = 70									' Set MISSILE channel frequency to 70 
			//	SAUCER_FREQ = 90     								// Set SAUCER channel frequency to 90 
			MIOS_GLCD_GCursorSet(ship_xpos,7);	// Draw first part of ship exploding
			MIOS_LCD_Data(0x10);
			MIOS_LCD_Data(0x24);
			MIOS_LCD_Data(0xac);
			MIOS_LCD_Data(0xf8);
			MIOS_LCD_Data(0xf0);
			MIOS_LCD_Data(0xe0);
			MIOS_LCD_Data(0xf0);
			MIOS_LCD_Data(0xf8);
			MIOS_LCD_Data(0xac);
			MIOS_LCD_Data(0x24);
			MIOS_LCD_Data(0x10);
			MIOS_Delay(40);
			
			//   SAUCER_FREQ = 100									' Increase the SAUCER channel's frequency
			//	MISSILE_FREQ = 120									' Increase the MISSILE channel's frequency
			MIOS_GLCD_GCursorSet(ship_xpos,7);	// Draw second part of ship exploding
			MIOS_LCD_Data(0x00);
			MIOS_LCD_Data(0x20);
			MIOS_LCD_Data(0x28);
			MIOS_LCD_Data(0xf0);
			MIOS_LCD_Data(0xa8);
			MIOS_LCD_Data(0xc0);
			MIOS_LCD_Data(0xe0);
			MIOS_LCD_Data(0xf0);
			MIOS_LCD_Data(0x28);
			MIOS_LCD_Data(0x04);
			MIOS_LCD_Data(0x00);
			MIOS_Delay(40);
			//   SAUCER_FREQ = 150									' Increase the SAUCER channel's frequency
			//	MISSILE_FREQ = 135									' Increase the MISSILE channel's frequency
			MIOS_GLCD_GCursorSet(ship_xpos,7);	// Draw third part of ship exploding
			MIOS_LCD_Data(0x00);
			MIOS_LCD_Data(0x00);
			MIOS_LCD_Data(0x30);
			MIOS_LCD_Data(0x60);
			MIOS_LCD_Data(0x80);
			MIOS_LCD_Data(0xc0);
			MIOS_LCD_Data(0x60);
			MIOS_LCD_Data(0x30);
			MIOS_LCD_Data(0x00);
			MIOS_LCD_Data(0x00);
			MIOS_LCD_Data(0x00);
			MIOS_Delay(100);
			MIOS_GLCD_GCursorSet(ship_xpos,7);	// clear ship graphic
			for (f=0;f<11;f++)
				MIOS_LCD_Data(0x00);				
			MIOS_Delay(100);
			lives--;	//Decrement the lives counter
			//   MISSILE_SOUND_ENABLE = 0							' Disable the MISSILE sound channel
			//	SAUCER_SOUND_ENABLE = 0								' Disable the SAUCER sound channel
			if (lives > 0) // Do we have any lives left ?
			{
				INVADER_Draw_Ship();
				MIOS_GLCD_GCursorSet(120,0);
				MIOS_LCD_PrintBCD1(lives); //Update the Display for the amount of lives
			} else {
				//So game over					
				INVADER_Game_Over();
				return;
			}
		}
        MIOS_Delay(6); // Main game delay 
	}
	flash_count++;
	
}


//----------------------------------------------------------------------------
// Move the invader's missile
// The missile can only be fired if the flag INVADER_MISSILE_FIRED is false, 
// otherwise there is already a missile in the air
void INVADER_Move_Invader_Missile(void)
{
    if (invader.INVADER_MISSILE_FIRED) //Don't enter the routine if the invader's missile is already flying       	                       
	{
		if (invader.TIME_TO_MOVE_INV_MISSILE) // Is it time to move the invader's missile ?
		{
			INVADER_Draw_Invader_Missile();
            invader_missile_ypos++;
        }	     		       	     
        if (invader_missile_ypos > 62 || invader.INVADER_MISSILE_HIT) // ' Has the invader's missile reached the bottom of the display or hit something ?
		{
			invader.INVADER_MISSILE_FIRED = 0;	// Yes. So signal the invader's missile is finished
			INVADER_Clear_Invader_Missile(); // Clear the invader's missile
            invader.INVADER_MISSILE_HIT = 0;
		}
	}
}
			
			
//----------------------------------------------------------------------------
// Move the invader's second missile
// The missile can only be fired if the flag INVADER_MISSILE2_FIRED is false, 
// otherwise there is already a missile in the air
void INVADER_Move_Invader_Missile2(void)
{
	if (invader.INVADER_MISSILE2_FIRED) // Don't enter the routine if the invader's missile is already flying       	                       
	{
		if (invader.TIME_TO_MOVE_INV_MISSILE2) // Is it time to move the invader's missile ?
		{
			INVADER_Draw_Invader_Missile2();
            invader_missile2_ypos++;
		}
        if (invader_missile2_ypos > 62 || invader.INVADER_MISSILE2_HIT) // Has the invader's missile reached the bottom of the display or hit something ?
		{
			invader.INVADER_MISSILE2_FIRED = 0; // Yes. So signal the invader's missile is finished
        	INVADER_Clear_Invader_Missile2(); //Clear the invader's missile
            invader.INVADER_MISSILE2_HIT = 0;
        }
    }
}
	
//----------------------------------------------------------------------------
// Check if the ship's missile has hit something
// Returns with MISSILE_HIT set if the ship's missile has hit something
void INVADER_Check_For_Missile_Hit()
{
	unsigned char f;
	game.MISSILE_HIT = 0; //Default to not hit    
	game.SAUCER_HIT = 0;	// Default to saucer not hit
	if (game.MISSILE_FIRED) // First make sure a missile is actually launched
	{
		// Check if the ship's missile has hit a BASE
        if (missile_ypos  == 53)
		{
			base_xpos_test = missile_xpos;// Transfer the ship's missile XPOS for testing
            INVADER_Check_Base_Hit();	// Check if a base was hit
           	if (game.BASE_HIT)
			{
            	//MISSILE_SOUND_ENABLE = 0; // Disable the missile's sound
                game.MISSILE_HIT = 1; //Transfer the hit detector into the missile detector
				return; // And return from the subroutine prematurely
        	}
		}
		// Check if the ship's missile has hit an invader's missile
		if (missile_xpos == invader_missile_xpos)
		{
			if (missile_ypos == invader_missile_ypos)
			{
        		//MISSILE_SOUND_ENABLE = 0							' Disable the missile's sound
                game.MISSILE_HIT = 1;	// Indicate the ship's missile has hit a target
        		invader.INVADER_MISSILE_HIT = 1;	// Indicate the invader's missile has also been hit
            	score++; //Increment the score by one
                return; // And return from the subroutine prematurely
        	} 
		}
 
		//Check if the ship's missile has hit the saucer
		if (game.SAUCER_ENABLED)
		{
			if (missile_ypos < 7)
			{
				if ((missile_xpos > saucer_xpos-1) && (missile_xpos < saucer_xpos  + SAUCER_WIDTH+1))
                {
					game.MISSILE_HIT = 1; //Indicate the ship's missile has hit a target	
            		game.SAUCER_HIT = 1;
            		//SAUCER_SOUND_ENABLE = TRUE
                    //SAUCER_FREQ = 30
					MIOS_GLCD_GCursorSet(saucer_xpos,0);	// Draw second part of ship exploding
					MIOS_LCD_Data(0x8c);
					MIOS_LCD_Data(0xc5);
					MIOS_LCD_Data(0x6b);
					MIOS_LCD_Data(0x36);
					MIOS_LCD_Data(0x0c);
					MIOS_LCD_Data(0x20);
					MIOS_LCD_Data(0x68);
					MIOS_LCD_Data(0xcc);
					MIOS_LCD_Data(0x96);
					MIOS_LCD_Data(0x33);
					MIOS_LCD_Data(0x69);
					MIOS_LCD_Data(0xcc);
					MIOS_LCD_Data(0x86);
					MIOS_LCD_Data(0x02);
					MIOS_Delay(10);
					MIOS_GLCD_GCursorSet(saucer_xpos,0);	// Display text hit
					MIOS_LCD_Data(0x7f);
					MIOS_LCD_Data(0x08);
					MIOS_LCD_Data(0x08);
					MIOS_LCD_Data(0x08);
					MIOS_LCD_Data(0x7f);
					MIOS_LCD_Data(0x00);
					MIOS_LCD_Data(0x00);
					MIOS_LCD_Data(0x41);
					MIOS_LCD_Data(0x7f);
					MIOS_LCD_Data(0x41);
					MIOS_LCD_Data(0x00);
					MIOS_LCD_Data(0x00);
					MIOS_LCD_Data(0x01);
					MIOS_LCD_Data(0x01);
					MIOS_LCD_Data(0x7f);
					MIOS_LCD_Data(0x01);
					MIOS_LCD_Data(0x01);
                    //  	SAUCER_FREQ = 20
					MIOS_Delay(5);
                    // 	SAUCER_FREQ = 10
                    MIOS_Delay(10);
                    score = score + (100 + saucer_xpos); // Add 100 + xpos to the score for hitting the saucer
                    //SAUCER_SOUND_ENABLE = FALSE
					return;
        		}
        	}
		}
                          
		//Check if the ship's missile has hit an INVADER
        invader_loop = 0;
        while (invader_loop < 18)
		{
        	if (invader_enabled[invader_loop]) //Only check if the invader is enabled
			{
            	if (invader_ypos[invader_loop] == missile_ypos / 8) 
				{
                	if ((missile_xpos > invader_xpos[invader_loop]-1) && (missile_xpos < invader_xpos[invader_loop] + INVADER_WIDTH +1))
					{
						invader_enabled[invader_loop] = 0;
                    	game.MISSILE_HIT = 1; //Indicate the ship's missile has hit a target
                        //MISSILE_SOUND_ENABLE = 1
                        // MISSILE_FREQ = 30
                        // Decide on the score depending on which invader is hit
						if (invader_loop < 6) // Top layer of invaders score 20
                        {
							MIOS_GLCD_GCursorSet(invader_xpos[invader_loop],invader_ypos[invader_loop]);
							MIOS_LCD_Data(0x42);
							MIOS_LCD_Data(0x61);
							MIOS_LCD_Data(0x51);
							MIOS_LCD_Data(0x49);
							MIOS_LCD_Data(0x46);
							MIOS_LCD_Data(0x00);
							MIOS_LCD_Data(0x3e);
							MIOS_LCD_Data(0x51);
							MIOS_LCD_Data(0x49);
							MIOS_LCD_Data(0x45);
							MIOS_LCD_Data(0x3e);
							MIOS_LCD_Data(0x00);
                            score = score + 20;
                        } 
						else if (invader_loop > 5 && invader_loop < 12) //Middle layer of invaders score 10
						{
							MIOS_GLCD_GCursorSet(invader_xpos[invader_loop],invader_ypos[invader_loop]);
							MIOS_LCD_Data(0x00);
							MIOS_LCD_Data(0x42);
							MIOS_LCD_Data(0x7f);
							MIOS_LCD_Data(0x40);
							MIOS_LCD_Data(0x00);
							MIOS_LCD_Data(0x00);
							MIOS_LCD_Data(0x3e);
							MIOS_LCD_Data(0x51);
							MIOS_LCD_Data(0x49);
							MIOS_LCD_Data(0x45);
							MIOS_LCD_Data(0x3e);
							MIOS_LCD_Data(0x00);
                            score = score + 10;
                        } 
						else if (invader_loop > 11 && invader_loop < 18) // Bottom layer of invaders score 5
						{
							MIOS_GLCD_GCursorSet(invader_xpos[invader_loop],invader_ypos[invader_loop]);
							MIOS_LCD_Data(0x3e);
							MIOS_LCD_Data(0x51);
							MIOS_LCD_Data(0x49);
							MIOS_LCD_Data(0x45);
							MIOS_LCD_Data(0x3e);
							MIOS_LCD_Data(0x00);
							MIOS_LCD_Data(0x27);
							MIOS_LCD_Data(0x45);
							MIOS_LCD_Data(0x45);
							MIOS_LCD_Data(0x45);
							MIOS_LCD_Data(0x39);
							MIOS_LCD_Data(0x00);
                           	score = score + 5;
						}
						MIOS_Delay(10);
                        //MISSILE_FREQ = 20
						MIOS_Delay(10);
                        //   MISSILE_FREQ = 10                              
						MIOS_Delay(20);
						MIOS_GLCD_GCursorSet(invader_xpos[invader_loop],invader_ypos[invader_loop]);
						for (f=0;f<12;f++)
							MIOS_LCD_Data(0x00); // Erase the INVADER that was hit
                    	
						invaders_enabled--; //Decrement the hits counter
                    		
                        if (invaders_enabled == 1) // Increase the speed substantially if only one invader left
                            invader_speed = 5;
                        else if (invaders_enabled == 2)
                            invader_speed = 10; // Increase the speed to fast if there are only two invaders left
                        else
						{
                           	if (invader_speed > 2)  //Is the inveders speed greater than 3 ?
							{
								if (invaders_enabled < 5) // Are there 4 or less invaders left ?
									invader_speed = invader_speed - 3;	// Yes.. So increase their speed by a factor of three
           						else
									invader_speed--; // Otherwise.. Increase the speed of the remaining invaders
							}
						}
                        //MISSILE_SOUND_ENABLE = 0; // Disable the missile's sound
                        return; // And return from the subroutine prematurely
                	}
            	}
			}
            invader_loop++;
		}			
    }
}


//----------------------------------------------------------------------------
// Move the ship's missile
// The missile can only be fired if the flag MISSILE_FIRED is false, 
// otherwise there is already a missile in the air
//
void INVADER_Move_Missile(void) 
{      
    if (game.MISSILE_FIRED) //Don't enter the routine if the ship's missile is already flying       	                       
    {
		if (game.TIME_TO_MOVE_SHIP_MISSILE) // Is it time to move the ship's missile ?
		{
			INVADER_Draw_Missile();
            missile_ypos--;
            //    MISSILE_FREQ = 63 - missile_ypos
        }	     		       	     
        if (missile_ypos == 0 || game.MISSILE_HIT) // Has the missile reached the top of the display or hit something ?
		{
            //MISSILE_SOUND_ENABLE = 0
            game.MISSILE_FIRED = 0;	// Yes. So signal the missile is finished
        	INVADER_Clear_Missile(); // Clear the missile		
        	missile_ypos = 63 - 9; // Reset the missile to the bottom of the display
        }
    }
}


//----------------------------------------------------------------------------
// Move the ship right
void INVADER_Ship_Right(void)
{
	if (ship_xpos > 117)
		return;
	INVADER_Draw_Ship();
	ship_xpos++;
}

//----------------------------------------------------------------------------
// Move the ship left
void INVADER_Ship_Left(void)
{
    if (!ship_xpos)
		return;
	INVADER_Draw_Ship();
	ship_xpos--;
}

//----------------------------------------------------------------------------
// Clear the saucer located at the top of the screen
void INVADER_Clear_Saucer(void)
{
	unsigned char f;
	MIOS_GLCD_GCursorSet(saucer_xpos,0);
	for (f=0;f<18;f++)
		MIOS_LCD_Data(0x00);
}

//----------------------------------------------------------------------------
// Draw the saucer located at the top of the screen
void INVADER_Draw_Saucer(void)
{		
	MIOS_GLCD_GCursorSet(saucer_xpos,0);
	MIOS_LCD_Data(0x00);
	MIOS_LCD_Data(0x04);
	MIOS_LCD_Data(0x0e);
	MIOS_LCD_Data(0x3a);
	MIOS_LCD_Data(0x1b);
	MIOS_LCD_Data(0x0f);
	MIOS_LCD_Data(0x1b);
	MIOS_LCD_Data(0x1b);
	MIOS_LCD_Data(0x0f);
	MIOS_LCD_Data(0x1b);
	MIOS_LCD_Data(0x3a);
	MIOS_LCD_Data(0x0e);
	MIOS_LCD_Data(0x04);
	MIOS_LCD_Data(0x00);
}
		
//----------------------------------------------------------------------------
// Move the saucer if it is not already flying
void INVADER_Move_Saucer(void)
{	
	if (game.SAUCER_ENABLED) // Don't enter the routine if the saucer is already flying       	
	{
		//SAUCER_SOUND_ENABLE = 1 // Enable the saucer's sound channel
        if (game.TIME_TO_MOVE_SAUCER) // Is it time to move the saucer ?
		{
			saucer_xpos--; //Move the saucer accross the screen
            //if (saucer_xpos / 2 =- 0) {
            //    SAUCER_FREQ = 10;
            //} else {
            //    SAUCER_FREQ = 12;
			//}
            INVADER_Draw_Saucer(); // Display the saucer
		}
        if (saucer_xpos == 0 || game.SAUCER_HIT) // Has the saucer reached the left of the display or been hit ?      		
		{
            game.SAUCER_ENABLED = 0; // Yes. So signal the sacucer is finished
        	INVADER_Clear_Saucer();	// Clear the saucer		
        	saucer_xpos = 127 - SAUCER_WIDTH; // Reset the saucer to the right of the display
			MIOS_GLCD_FontInit(SPACE_FONT);   
			MIOS_GLCD_GCursorSet(0,0);
			MIOS_LCD_PrintBCD1(level);
			MIOS_LCD_PrintCString(" ");
			MIOS_LCD_PrintBCD5(score);
			MIOS_GLCD_GCursorSet(120,0);
			MIOS_LCD_PrintBCD1(lives);
        } 
	}
	//else 
    //	SAUCER_SOUND_ENABLE =0; // Disable the saucer's sound channel 
}
//----------------------------------------------------------------------------
// Determine whether a missile, ship, saucer or base should move 
// governed by their individual speed controls
void INVADER_Govern_Speeds(void)
{
	//Advance the saucers's tick (governs its speed)
    saucer_speed--;
    game.TIME_TO_MOVE_SAUCER = 0;
    if (saucer_speed == 0)
	{
        game.TIME_TO_MOVE_SAUCER = 1;
        saucer_speed = 3;
    }        
    // Advance the ship's tick (governs its speed)
    ship_speed--;
    game.TIME_TO_MOVE_BASE = 0;
    if (ship_speed == 0)
	{
       	game.TIME_TO_MOVE_BASE = 1;
        ship_speed = 3;
    }        

	// Advance the invader's missile tick (governs its speed)
    invader_missile_tick++;
    invader.TIME_TO_MOVE_INV_MISSILE = 0;
    if (invader_missile_tick > invader_missile_speed-1) // Set the speed for the invader's missile
	{
		invader.TIME_TO_MOVE_INV_MISSILE = 1;
        invader_missile_tick = 0;
    }
    // Advance the invader's second missile tick (governs its speed)
    invader_missile2_tick++;
    invader.TIME_TO_MOVE_INV_MISSILE2 = 0;
    if (invader_missile2_tick > invader_missile2_speed-1) // Set the speed for the invader's second missile
	{
		invader.TIME_TO_MOVE_INV_MISSILE2 = 1;
        invader_missile2_tick = 0;
	}
    // Advance the ship's missile tick (governs its speed)
    ship_missile_tick--;
    game.TIME_TO_MOVE_SHIP_MISSILE = 0;
    if (ship_missile_tick == 0) // Set the speed for the ship's missile
	{
		game.TIME_TO_MOVE_SHIP_MISSILE = 1;
        ship_missile_tick = 2;
	}
}


//---------------------------------------------------------------------------- 
// Fire an invader missile if possible 
// The logic is: -
// Make sure a missile is not already in flight.
// Scan all the invaders...
// If the invader is enabled, then check if it is over the ship.
// If it is then enable a missile to be fired.      
void INVADER_Fire_Invader_Missile()
{
	if (!invader.INVADER_MISSILE_FIRED) //Is it OK to fire an invader missile ?
	{
		invader_loop = 0;
        while (invader_loop <18) // Create a loop for all the invaders
		{
            if (invader_enabled[invader_loop]) //Is this invader enabled ?
			{
				if ((invader_xpos[invader_loop]+5 > ship_xpos-1) && (invader_xpos[invader_loop]+5 < ship_xpos +9))
				{	// Is the invader over the ship ?
                    invader.INVADER_MISSILE_FIRED = 1; //Signal that an invader's  missile is in the air
            		invader_missile_xpos = invader_xpos[invader_loop] + 5;	// Move missile XPOS to the middle of the invader
                	invader_missile_ypos = (invader_ypos[invader_loop] * 8) + 8;	// Move missile YPOS to below the invader
                }                   
            }           
			invader_loop++;
        } //Close the loop when all invaders have been scanned
	}
}


//---------------------------------------------------------------------------- 
// Fire an invader second missile if possible 
// The logic is: -
// Make sure a missile is not already in flight.
// Scan all the invaders...
// If the invader is enabled, then check if it is over the ship.
// If it is then enable a missile to be fired.      
void INVADER_Fire_Invader_Missile2(void)
{
	if (!invader.INVADER_MISSILE2_FIRED) // Is it OK to fire an invader second missile ?
	{
		if ((ship_xpos > invader_missile_xpos+random_value) && (ship_xpos < invader_missile_xpos - random_value))
		{
			invader_loop = 0;
            while (invader_loop < 18) //Create a loop for all the invaders
			{
				if (invader_enabled[invader_loop]) // Is this invader enabled ?
				{
					if ((invader_enabled[invader_loop]+5 > ship_xpos-1) && invader_enabled[invader_loop]+5 < ship_xpos+9)
					{ //Is the invader over the ship ?
						invader.INVADER_MISSILE2_FIRED = 1;// Signal that an invader's  missile is in the air
						invader_missile2_xpos = invader_xpos[invader_loop] + 5;	// Move missile XPOS to the middle of the invader
						invader_missile2_ypos = (invader_ypos[invader_loop] * 8) + 8; // Move missile YPOS to below the invader
                    }                   
            	}           
            	invader_loop++;
			}
		}
	}
}



//----------------------------------------------------------------------------
// Move the INVADERS down a line
// And check whether they have reached the bottom of the screen
// Flag INVADERS_REACHED_BOTTOM will be set if they have
void INVADER_Move_Invaders_Down(void)
{
	unsigned char f;
	invader.INVADERS_REACHED_BOTTOM = 0; //Default to the invaders not at bottom of the screen
	temp_loop = 18;
	while (temp_loop > 0)
	{
		temp_loop--;
        if (invader_enabled[temp_loop])
		{
			MIOS_GLCD_GCursorSet(invader_xpos[temp_loop],invader_ypos[temp_loop]);
			for (f=0;f<12;f++)
				MIOS_LCD_Data(0x00);
            invader_ypos[temp_loop] = invader_ypos[temp_loop] + 1;
			if (invader_ypos[temp_loop] == 6)
			{
				//Have the invaders hit the bases ?
				for (f=0;f<9;f++)
				{
					base_enabled[f]=0; // Yes.. So disable them all
					base_hits[f]=5; // And move their hit counters to past their thresholds
				}
			}
            if (invader_ypos[temp_loop] > 6)
				invader.INVADERS_REACHED_BOTTOM = 1; // Set a flag if the invaders have reached the bottom
			MIOS_GLCD_GCursorSet(invader_xpos[temp_loop],invader_ypos[temp_loop]);
			MIOS_LCD_Data(0x00);
			MIOS_LCD_Data(0x06);
			MIOS_LCD_Data(0x0c);
			MIOS_LCD_Data(0x9c);
			MIOS_LCD_Data(0xea);
			MIOS_LCD_Data(0x36);
			MIOS_LCD_Data(0x36);
			MIOS_LCD_Data(0xea);
			MIOS_LCD_Data(0x9c);
			MIOS_LCD_Data(0x0c);
			MIOS_LCD_Data(0x06);
			MIOS_LCD_Data(0x00);
		}
	}
	
    invader_missile_speed++;// Slow down the invader's missile speed as they approach the bottom of the screen
    if (invader_speed > 2) 
		invader_speed--;	//Increase the speed of the invaders

}


//----------------------------------------------------------------------------
// Move the INVADERS
void INVADER_Move_Invaders(void)
{
  	invader.TIME_TO_MOVE_INVADERS_DOWN = 0;
    if (invader.INVADERS_DIRECTION == FORWARD)	//Are the invaders to move forward (right) ?         
	{
		invader_loop = 0;	//Yes.. So reset the invader loop
		while (invader_loop <18)
		{ // Create a loop for all the invaders
			if (invader_enabled[invader_loop]) // Is this invader enabled ?          	
			{
				invader_xpos[invader_loop] = invader_xpos[invader_loop] + 1; // Yes.. So increment its XPOS
       			if (invader_xpos[invader_loop]+1 > INVADER_RIGHT_LIMIT) // Have we hit the right side of the screen ?
				{
					invader.INVADERS_DIRECTION = BACKWARD;	//Yes.. So indicate that we need to go backwards
                    game.SAUCER_ENABLED = 1;
                    invader.TIME_TO_MOVE_INVADERS_DOWN = 1; // and signal that we need to move the invaders down
				}
			}
			invader_loop++;
		}
        // Close the loop after all the invader elements have been scanned
	} else { // Otherwise we go backwards (left)
		invader_loop = 0;	//Reset the invader loop
		while (invader_loop <18)
		{ // Create a loop for all the invaders
			if (invader_enabled[invader_loop] == 1) // Is this invader enabled ?        		
			{
				invader_xpos[invader_loop] = invader_xpos[invader_loop] - 1; // Yes.. So decrement its XPOS
                   if (invader_xpos[invader_loop] < 2) //Have we hit the left side of the screen ?                   
				{
				invader.INVADERS_DIRECTION = FORWARD;	// Yes.. So indicate that we need to go forwards
                       invader.TIME_TO_MOVE_INVADERS_DOWN = 1;	// and signal that we need to move the invaders down
				}
			}
			invader_loop++;
		}
	}
    if (invader.TIME_TO_MOVE_INVADERS_DOWN)
		INVADER_Move_Invaders_Down(); //do  we need to move the invaders down ?
    INVADER_Draw_Invaders();
}

//----------------------------------------------------------------------------
// Draw the invaders
// Draws one of two invader shapes depending on the contents of BIT variable invader_character
// Each row of invaders is a different character
//INVADER TOP ROW character 1 $00,$18,$4C,$2E,$5B,$2F,$2F,$5B,$2E,$4C,$18,$00
//INVADER TOP ROW character 2 $00,$08,$2C,$5E,$0B,$1F,$1F,$0B,$5E,$2C,$08,$00                  

//INVADER MIDDLE ROW character 1 $00,$38,$0D,$3E,$5A,$1E,$1E,$5A,$3E,$0D,$38,$00
//INVADER MIDDLE ROW character 2 $00,$06,$48,$7F,$1A,$1E,$1E,$1A,$7F,$48,$06,$00
                    
//INVADER BOTTOM ROW character 1 $00,$4C,$6E,$3A,$1B,$2F,$2F,$1B,$3A,$6E,$4C,$00
//INVADER BOTTOM ROW character 2 $00,$0C,$0E,$3A,$4B,$1F,$1F,$4B,$3A,$0E,$0C,$00
void INVADER_Draw_Invaders()
{
	invader_loop = 0;
	while (invader_loop < 18)
	{
        if (invader_enabled[invader_loop])
		{
            if (invader_character == 0)
			{
              	//INVADER_SOUND_ENABLE = 0;
                //INVADER_FREQ = 90 ;                                    
				if (invader_loop < 6)//Decide on the score depending on which invader is hit
				{
 					MIOS_GLCD_GCursorSet(invader_xpos[invader_loop],invader_ypos[invader_loop]);
					MIOS_LCD_Data(0x00);
					MIOS_LCD_Data(0x18);
					MIOS_LCD_Data(0x4c);
					MIOS_LCD_Data(0x2e);
					MIOS_LCD_Data(0x5b);
					MIOS_LCD_Data(0x2f);
					MIOS_LCD_Data(0x2f);
					MIOS_LCD_Data(0x5b);
					MIOS_LCD_Data(0x2e);
					MIOS_LCD_Data(0x4c);
					MIOS_LCD_Data(0x18);
					MIOS_LCD_Data(0x00);
				}
				else if (invader_loop>5 && invader_loop < 12)
				{
					MIOS_GLCD_GCursorSet(invader_xpos[invader_loop],invader_ypos[invader_loop]);
					MIOS_LCD_Data(0x00);
					MIOS_LCD_Data(0x38);
					MIOS_LCD_Data(0x0d);
					MIOS_LCD_Data(0x3e);
					MIOS_LCD_Data(0x5a);
					MIOS_LCD_Data(0x1e);
					MIOS_LCD_Data(0x1e);
					MIOS_LCD_Data(0x5a);
					MIOS_LCD_Data(0x3e);
					MIOS_LCD_Data(0x0d);
					MIOS_LCD_Data(0x38);
					MIOS_LCD_Data(0x00);
                }
				else if (invader_loop > 11 && invader_loop < 18)
				{
					MIOS_GLCD_GCursorSet(invader_xpos[invader_loop],invader_ypos[invader_loop]);
					MIOS_LCD_Data(0x00);
					MIOS_LCD_Data(0x4c);
					MIOS_LCD_Data(0x6e);
					MIOS_LCD_Data(0x3a);
					MIOS_LCD_Data(0x1b);
					MIOS_LCD_Data(0x2f);
					MIOS_LCD_Data(0x2f);
					MIOS_LCD_Data(0x1b);
					MIOS_LCD_Data(0x3a);
					MIOS_LCD_Data(0x6e);
					MIOS_LCD_Data(0x4c);
					MIOS_LCD_Data(0x00);
				}
                //INVADER_SOUND_ENABLE = 1;
            } else {
			    //INVADER_SOUND_ENABLE = 0
                //INVADER_FREQ = 105
				if (invader_loop < 6) //Decide on the score depending on which invader is hit
				{	
					MIOS_GLCD_GCursorSet(invader_xpos[invader_loop],invader_ypos[invader_loop]);
					MIOS_LCD_Data(0x00);
					MIOS_LCD_Data(0x08);
					MIOS_LCD_Data(0x2c);
					MIOS_LCD_Data(0x5e);
					MIOS_LCD_Data(0x0b);
					MIOS_LCD_Data(0x1f);
					MIOS_LCD_Data(0x1f);
					MIOS_LCD_Data(0x0b);
					MIOS_LCD_Data(0x5e);
					MIOS_LCD_Data(0x2c);
					MIOS_LCD_Data(0x08);
					MIOS_LCD_Data(0x00);
				}
				else if (invader_loop>5 && invader_loop < 12)
				{							
 					MIOS_GLCD_GCursorSet(invader_xpos[invader_loop],invader_ypos[invader_loop]);
					MIOS_LCD_Data(0x00);
					MIOS_LCD_Data(0x06);
					MIOS_LCD_Data(0x48);
					MIOS_LCD_Data(0x7f);
					MIOS_LCD_Data(0x1a);
					MIOS_LCD_Data(0x1e);
					MIOS_LCD_Data(0x1e);
					MIOS_LCD_Data(0x1a);
					MIOS_LCD_Data(0x7f);
					MIOS_LCD_Data(0x48);
					MIOS_LCD_Data(0x06);
					MIOS_LCD_Data(0x00);
                }
				else if (invader_loop > 11 && invader_loop < 18)
				{												
 					MIOS_GLCD_GCursorSet(invader_xpos[invader_loop],invader_ypos[invader_loop]);
					MIOS_LCD_Data(0x00);
					MIOS_LCD_Data(0x0c);
					MIOS_LCD_Data(0x0e);
					MIOS_LCD_Data(0x3a);
					MIOS_LCD_Data(0x4b);
					MIOS_LCD_Data(0x1f);
					MIOS_LCD_Data(0x1f);
					MIOS_LCD_Data(0x4b);
					MIOS_LCD_Data(0x3a);
					MIOS_LCD_Data(0x0e);
					MIOS_LCD_Data(0x0c);
					MIOS_LCD_Data(0x00);
				}
                //INVADER_SOUND_ENABLE = 1;
			}
		}
        invader_loop++;
	}
}
 
//----------------------------------------------------------------------------
// Draw the ship
void INVADER_Draw_Ship()
{
	MIOS_GLCD_GCursorSet(ship_xpos,7);
	MIOS_LCD_Data(0x00);
	MIOS_LCD_Data(0xe0);
	MIOS_LCD_Data(0xf0);
	MIOS_LCD_Data(0xf0);
	MIOS_LCD_Data(0xf8);
	MIOS_LCD_Data(0xfc);
	MIOS_LCD_Data(0xf8);
	MIOS_LCD_Data(0xf0);
	MIOS_LCD_Data(0xf0);
	MIOS_LCD_Data(0xe0);
	MIOS_LCD_Data(0x00);
}
//----------------------------------------------------------------------------
// Reset the invaders positions
// Each element of the arrays hold coordinates for the invaders
// Bit-0 of invader_enabled determines whether the invader is active or not (has been hit)
// Bit-1 of invader_enabled sets which graphic to use for the invader
// invader_xpos holds the X position of the invader
// invader_ypos holds the Y position of the invader
void INVADER_Reset_Invaders(void)
{
    invader_loop = 0;
	while (invader_loop < 18)
	{
		invader_enabled[invader_loop] = 1; // Set all invaders to active and image 1
        if (invader_loop < 6)//Decide on the score depending on which invader is hit
		{
           	invader_xpos[invader_loop] = (invader_loop + 1) * 16;
           	invader_ypos[invader_loop] = 1;
        }
		else if (invader_loop>5 && invader_loop < 12)
		{
           	invader_xpos[invader_loop] = (invader_loop - 5) * 16;
           	invader_ypos[invader_loop] = 2;
        }
		else if (invader_loop > 11 && invader_loop < 18)
		{
           	invader_xpos[invader_loop] = (invader_loop - 11) * 16;
           	invader_ypos[invader_loop] = 3;
		}
       	invader_loop++;
	}
}


// Re-Draw the bases
void INVADER_Update_Bases(void)
{
    MIOS_GLCD_FontInit(SPACE_FONT);   
	MIOS_GLCD_GCursorSet(12,6);
	MIOS_LCD_PrintChar(2+base_hits[0]);
	MIOS_LCD_PrintChar(8+base_hits[1]);
	MIOS_LCD_PrintChar(14+base_hits[2]);
	MIOS_GLCD_GCursorSet(54,6);
	MIOS_LCD_PrintChar(2+base_hits[3]);
	MIOS_LCD_PrintChar(8+base_hits[4]);
	MIOS_LCD_PrintChar(14+base_hits[5]);
	MIOS_GLCD_GCursorSet(90,6);
	MIOS_LCD_PrintChar(2+base_hits[6]);
	MIOS_LCD_PrintChar(8+base_hits[7]);
	MIOS_LCD_PrintChar(14+base_hits[8]);
}


//----------------------------------------------------------------------------
// Check if BASE hit by a missile
// Each base is built from three elements (characters)
// Returns with BASE_HIT set if a hit was detected
void INVADER_Check_Base_Hit(void)
{
	game.BASE_HIT = 0;	// Default to no hit detected
    if (base_xpos_test >11 && base_xpos_test < 18)	// Has the missile XPOS hit BASE 1, ELEMENT 0
    {
		if (base_hits[0] < 5 && base_enabled[0]) //  Is the base element enabled, and has had less than 5 hits ?
		{
           	base_hits[0]++; // Increment the amount of hits the base element has sustained
            game.BASE_HIT = 1; // Indicate a missile has hit a target
        } else {
           	base_enabled[0] = 0;	// Disable the base element
        }            
	}
	else if (base_xpos_test >17 && base_xpos_test < 24)	// Has the missile XPOS hit BASE 1, ELEMENT 1
	{
		if (base_hits[1] < 5 && base_enabled[1]) // Is the base element enabled, and has had less than 5 hits ? 
        {
			base_hits[1]++;	// Increment the amount of hits the base element has sustained
            game.BASE_HIT = 1;	// Indicate a missile has hit a target
        } else {
            base_enabled[1] = 0; //Disable the base element
        }
	}
	else if (base_xpos_test >23 && base_xpos_test < 30)	// Has the missile XPOS hit BASE 1, ELEMENT 2
	{	
		if (base_hits[2] < 5 && base_enabled[2]) // Is the base element enabled, and has had less than 5 hits ? 
        {
			base_hits[2]++;	// Increment the amount of hits the base element has sustained
            game.BASE_HIT = 1;	// Indicate a missile has hit a target
        } else {
            base_enabled[2] = 0;	// Disable the base element
        }
	}	
	else if (base_xpos_test >53 && base_xpos_test < 60)	// Has the missile XPOS hit BASE 2 ELEMENT 3
	{
		if (base_hits[3] < 5 && base_enabled[3]) // Is the base element enabled, and has had less than 5 hits ? 
        {
			base_hits[3]++;	// Increment the amount of hits the base element has sustained
            game.BASE_HIT = 1;	// Indicate a missile has hit a target
        } else {
            base_enabled[3] = 0;	// Disable the base element
        }
	}	
	else if (base_xpos_test >59 && base_xpos_test < 66)	// Has the missile XPOS hit BASE 2 ELEMENT 4
	{
		if (base_hits[4] < 5 && base_enabled[4]) // Is the base element enabled, and has had less than 5 hits ? 
        {
			base_hits[4]++;	// Increment the amount of hits the base element has sustained
            game.BASE_HIT = 1;	// Indicate a missile has hit a target
        } else {
            base_enabled[4] = 0;	// Disable the base element
        }
	}	
	else if (base_xpos_test >65 && base_xpos_test < 72)	// Has the missile XPOS hit BASE 2 ELEMENT 5
	{
		if (base_hits[5] < 5 && base_enabled[5]) // Is the base element enabled, and has had less than 5 hits ? 
        {
			base_hits[5]++;	// Increment the amount of hits the base element has sustained
            game.BASE_HIT = 1;	// Indicate a missile has hit a target
        } else {
            base_enabled[5] = 0;	// Disable the base element
        }
	}	
	else if (base_xpos_test >89 && base_xpos_test < 96)	// Has the missile XPOS hit BASE 3 ELEMENT 6
	{
		if (base_hits[6] < 5 && base_enabled[6]) // Is the base element enabled, and has had less than 5 hits ? 
        {
			base_hits[6]++;	// Increment the amount of hits the base element has sustained
            game.BASE_HIT = 1;	// Indicate a missile has hit a target
        } else {
            base_enabled[6] = 0;	// Disable the base element
        }
	}	
	else if (base_xpos_test >95 && base_xpos_test < 102)	// Has the missile XPOS hit BASE 3 ELEMENT 7
	{
		if (base_hits[7] < 5 && base_enabled[7]) // Is the base element enabled, and has had less than 5 hits ? 
        {
			base_hits[7]++;	// Increment the amount of hits the base element has sustained
            game.BASE_HIT = 1;	// Indicate a missile has hit a target
        } else {
            base_enabled[7] = 0;	// Disable the base element
        }
	}	
	else if (base_xpos_test >101 && base_xpos_test < 108)	// Has the missile XPOS hit BASE 3 ELEMENT 8
	{				
		if (base_hits[8] < 5 && base_enabled[8]) // Is the base element enabled, and has had less than 5 hits ? 
        {
			base_hits[8]++;	// Increment the amount of hits the base element has sustained
            game.BASE_HIT = 1;	// Indicate a missile has hit a target
        } else {
            base_enabled[8] = 0;	// Disable the base element
        }
	}	
	
    if (game.BASE_HIT)
		INVADER_Update_Bases();	// Update the base shapes if a hit was detected
}



void INVADER_New_Sheet(void)
{
   level++;
   MIOS_LCD_Clear();
   INVADER_Update_Bases();  //Draw the bases (or what's left of them)
   game.SHIP_HIT = 0; //Default to no hit on the ship by a missile
   game.SAUCER_ENABLED = 0; // Disable the saucer
   saucer_xpos = 127 - SAUCER_WIDTH; // Position saucer at top tight of the screen
   missile_xpos = 0;
   missile_ypos = 63 - 9;
   invader_missile_xpos = 0;
   invader_missile_ypos = 0;
   missile_status = 0;
   invader_missile2_ypos = 0; // Y position of Invader's second MISSILE
   invader_missile2_xpos = 0; // X position of Invader's second MISSILE
   invader.TIME_TO_MOVE_INV_MISSILE2 = 0; // Indicate time to move the invader's second missile
   invader.INVADER_MISSILE2_FIRED = 0; // TRUE if Invader's second MISSILE in the air
   invader.INVADER_MISSILE2_HIT = 0; // Default to not hit anything
   game.TIME_TO_MOVE_BASE = 0;
        
   //INVADER_Clear_Invader_Missile();
   //INVADER_Clear_Missile();
   INVADER_Draw_Ship(); //Draw the initial ship
            
   default_invader_speed--; // Speed up the invaders every new sheet
   invader_speed = default_invader_speed; // Transfer the speed into the actual speed altering variable
   invader_tick=0;
   invader_missile_tick=0;
   invader_missile2_tick=0;
   ship_missile_tick = 2;
   invader.INVADERS_REACHED_BOTTOM = 0; // Default to the invaders not at the bottom of the screen
   invader_character = 0;
   invader_missile_speed = 5; // Default speed of the invader's missile
   invader_missile2_speed = 4;
   invader.INVADERS_DIRECTION = FORWARD; // Default to the invaders moving right
   invaders_enabled = 18; // All 18 invaders are enabled
   INVADER_Reset_Invaders(); // Reset all the invaders positions   
   INVADER_Draw_Invaders(); // Place all the invaders on the screen 
   MIOS_GLCD_FontInit(SPACE_FONT);   
   MIOS_GLCD_GCursorSet(0,0);
   MIOS_LCD_PrintBCD1(level);
   MIOS_LCD_PrintCString(" ");
   MIOS_LCD_PrintBCD5(score);
   MIOS_GLCD_GCursorSet(120,0);
   MIOS_LCD_PrintBCD1(lives);
}


//----------------------------------------------------------------------------
// Check if the invader's missile has hit something
// Returns with INVADER_MISSILE_HIT set if the invader's missile has hit something

void INVADER_Check_For_Invader_Missile_Hit(void)
{
	invader.INVADER_MISSILE_HIT = 0; // Default to not hit 
    if (invader.INVADER_MISSILE_FIRED) // First make sure that a missile is actually launched
	{
		//Check if the invader's missile has hit a BASE
        if (invader_missile_ypos > 44)
		{
        	base_xpos_test = invader_missile_xpos; // Transfer the invader's missile XPOS for testing
            INVADER_Check_Base_Hit(); // Check if a base was hit
            if (game.BASE_HIT)
			{
            	invader.INVADER_MISSILE_HIT = 1; // Transfer the hit detector into the invader's missile detector
				return; // Return from the subroutine prematurely
            }
		}
		// check if the invader's missile has hit the ship
		if (invader_missile_ypos > 55)
		{

            if ((invader_missile_xpos > ship_xpos - 2) && invader_missile_xpos < (ship_xpos + SHIP_WIDTH+1))
			{
				INVADER_Draw_Ship();
                invader.INVADER_MISSILE_HIT = 1; // Indicate the invader's missile has hit something
       			game.SHIP_HIT = 1; //Indicate that it is the ship that has been hit
			}
		}
	}
}

//--------------------------------------------------------------------------
//heck if the invader's second missile has hit something
//eturns with INVADER_MISSILE2_HIT set if the invader's missile has hit something
void INVADER_Check_For_Invader_Missile2_Hit(void)
{ 
	invader.INVADER_MISSILE2_HIT = 0; // Default to not hit
    if (invader.INVADER_MISSILE2_FIRED) // First make sure that a second missile is actually launched
	{
		// Check if the invader's second missile has hit a BASE
        if (invader_missile2_ypos > 44) 
		{
        	base_xpos_test = invader_missile2_xpos;	// Transfer the invader's second missile XPOS for testing
           	INVADER_Check_Base_Hit(); // Check if a base was hit
           	if (game.BASE_HIT)
			{
            	invader.INVADER_MISSILE2_HIT = 1; // Transfer the hit detector into the invader's second missile detector
        		return;	// Return from the subroutine prematurely
            }
		}
             
		// Check if the invader's missile has hit the ship
		if (invader_missile2_ypos > 55) 
		{
		
			if ((invader_missile2_xpos > ship_xpos - 2) && invader_missile2_xpos < (ship_xpos + SHIP_WIDTH+1))
			{
				INVADER_Draw_Ship();
                invader.INVADER_MISSILE2_HIT = 1; // Indicate the invader's missile has hit something
       			game.SHIP_HIT = 1; //Indicate that it is the ship that has been hit
			}
		}
	}
}

//----------------------------------------------------------------------------
// Draw or clear the Ship's missile
void INVADER_Clear_Missile(void)
{
	INVADER_LCD_UnPlot(missile_ypos, missile_xpos);
    INVADER_LCD_UnPlot(missile_ypos, missile_xpos + 1);
    INVADER_LCD_UnPlot(missile_ypos + 1, missile_xpos);
    INVADER_LCD_UnPlot(missile_ypos + 1, missile_xpos + 1);
    INVADER_LCD_UnPlot(missile_ypos + 2, missile_xpos);
    INVADER_LCD_UnPlot(missile_ypos + 2, missile_xpos + 1);
}
void INVADER_Draw_Missile(void)
{
    INVADER_LCD_Plot(missile_ypos, missile_xpos);
    INVADER_LCD_Plot(missile_ypos, missile_xpos + 1);
    INVADER_LCD_Plot(missile_ypos + 1, missile_xpos);
    INVADER_LCD_Plot(missile_ypos + 1, missile_xpos + 1);
    INVADER_LCD_UnPlot(missile_ypos + 2, missile_xpos);
    INVADER_LCD_UnPlot(missile_ypos + 2, missile_xpos + 1);
}


// Draw or clear the Invader's missile
void INVADER_Clear_Invader_Missile(void)
{
	INVADER_LCD_UnPlot (invader_missile_ypos, invader_missile_xpos);
    INVADER_LCD_UnPlot (invader_missile_ypos, invader_missile_xpos + 1);
    INVADER_LCD_UnPlot (invader_missile_ypos + 1, invader_missile_xpos);
    INVADER_LCD_UnPlot (invader_missile_ypos + 1, invader_missile_xpos + 1);
    INVADER_LCD_UnPlot (invader_missile_ypos + 2, invader_missile_xpos);
    INVADER_LCD_UnPlot (invader_missile_ypos + 2, invader_missile_xpos + 1);
}
		
void INVADER_Draw_Invader_Missile(void)
{
	INVADER_LCD_UnPlot (invader_missile_ypos, invader_missile_xpos);
    INVADER_LCD_UnPlot (invader_missile_ypos, invader_missile_xpos + 1);
    INVADER_LCD_Plot (invader_missile_ypos + 1, invader_missile_xpos);
    INVADER_LCD_Plot (invader_missile_ypos + 1, invader_missile_xpos + 1);
    INVADER_LCD_Plot (invader_missile_ypos + 2, invader_missile_xpos);
    INVADER_LCD_Plot (invader_missile_ypos + 2, invader_missile_xpos + 1);
}
        
//----------------------------------------------------------------------------
// Draw or clear the second Invader's missile
void INVADER_Clear_Invader_Missile2(void)
{
	INVADER_LCD_UnPlot (invader_missile2_ypos, invader_missile2_xpos);
    INVADER_LCD_UnPlot (invader_missile2_ypos, invader_missile2_xpos + 1);
    INVADER_LCD_UnPlot (invader_missile2_ypos + 1, invader_missile2_xpos);
    INVADER_LCD_UnPlot (invader_missile2_ypos + 1, invader_missile2_xpos + 1);
    INVADER_LCD_UnPlot (invader_missile2_ypos + 2, invader_missile2_xpos);
    INVADER_LCD_UnPlot (invader_missile2_ypos + 2, invader_missile2_xpos + 1);
}

void INVADER_Draw_Invader_Missile2(void)
{
	INVADER_LCD_UnPlot (invader_missile2_ypos, invader_missile2_xpos);
    INVADER_LCD_UnPlot (invader_missile2_ypos, invader_missile2_xpos + 1);
    INVADER_LCD_Plot (invader_missile2_ypos + 1, invader_missile2_xpos);
    INVADER_LCD_Plot (invader_missile2_ypos + 1, invader_missile2_xpos + 1);
    INVADER_LCD_Plot (invader_missile2_ypos + 2, invader_missile2_xpos);
    INVADER_LCD_Plot (invader_missile2_ypos + 2, invader_missile2_xpos + 1);
}


void INVADER_LCD_Plot(unsigned char y, unsigned char x)
{

	unsigned char line=y/8;
	unsigned char digit=y%8;
	unsigned char addpixel=MIOS_HLP_GetBitORMask(digit);
	MIOS_GLCD_GCursorSet(x,line);
	missile_char = addpixel;
	MIOS_LCD_Data(missile_char);
#ifdef DEBUG
  // only for debugging - remove this (or change "#if 1" to "#if 0") in a common application!
  DEBUG_MSG_SendHeader();
  DEBUG_MSG_SendCString("Missile Plot: X=");
  DEBUG_MSG_SendBCD3(x);
  DEBUG_MSG_SendCString(" Y=");
  DEBUG_MSG_SendBCD3(line);
  DEBUG_MSG_SendCString(" Pixel: ");
  DEBUG_MSG_SendBCD3(missile_char);
  DEBUG_MSG_SendCString(" digit: ");
  DEBUG_MSG_SendBCD3(digit);
  //DEBUG_MSG_SendChar(' ');
  //DEBUG_MSG_SendCString(pin_value ? "depressed" : "pressed");
  DEBUG_MSG_SendFooter();
#endif
}


void INVADER_LCD_UnPlot(unsigned char y, unsigned char x)
{
	unsigned char line=y/8;
	unsigned char digit=y%8;
	unsigned char delpixel=MIOS_HLP_GetBitORMask(digit);
	MIOS_GLCD_GCursorSet(x,line);
	missile_char = !delpixel;
	MIOS_LCD_Data(missile_char);	
#ifdef DEBUG
  // only for debugging - remove this (or change "#if 1" to "#if 0") in a common application!
  DEBUG_MSG_SendHeader();
  DEBUG_MSG_SendCString("Missile UnPlot: X=");
  DEBUG_MSG_SendBCD3(x);
  DEBUG_MSG_SendCString(" Y=");
  DEBUG_MSG_SendBCD3(line);
  DEBUG_MSG_SendCString(" Pixel: ");
  DEBUG_MSG_SendBCD3(missile_char);
  DEBUG_MSG_SendCString(" digit: ");
  DEBUG_MSG_SendBCD3(digit);
  //DEBUG_MSG_SendChar(' ');
  //DEBUG_MSG_SendCString(pin_value ? "depressed" : "pressed");
  DEBUG_MSG_SendFooter();
#endif
}


//--------------------------------------------------------------------
// Explode the ship when the game is over
void INVADER_Game_Over(void)
{
	MIOS_GLCD_FontInit(SPACE_FONT);   
	MIOS_GLCD_GCursorSet(0,0);
	MIOS_LCD_PrintBCD1(level);
	MIOS_LCD_PrintCString(" ");
	MIOS_LCD_PrintBCD5(score);
	MIOS_GLCD_GCursorSet(120,0);
	MIOS_LCD_PrintBCD1(lives);
    //saucer_sound_enable = 1; // Enable SAUCER sound channel
    //missile_sound_enable=1; // Enable MISSILE sound channel
    //SAUCER_FREQ = 70											' Set MISSILE channel frequency to 70 
    //SAUCER_FREQ = 90     										' Set SAUCER channel frequency to 90 
    temp_loop = 1;
	while (temp_loop <25)
	{
      	//MISSILE_FREQ = MISSILE_FREQ - TEMP_LOOP
        //SAUCER_FREQ = SAUCER_FREQ - TEMP_LOOP
		MIOS_Delay(10+temp_loop);
        //    Circle 1,SHIP_XPOS + 5,63,TEMP_LOOP          
        //if (temp_loop > 10) 	
		//	Circle 0,SHIP_XPOS + 5,63,TEMP_LOOP - 10
        temp_loop=temp_loop+2;
    }
	
    //SAUCER_SOUND_ENABLE = FALSE										' Disable SAUCER sound channel
    //MISSILE_SOUND_ENABLE = FALSE       								' Disable MISSILE sound channel
    temp_loop = 1;
	while (temp_loop < 25)
    {
        //Circle 0,SHIP_XPOS + 5,63,TEMP_LOOP    
        temp_loop=temp_loop + 2;
    }
    //MIOS_LCD_Clear();
	MIOS_GLCD_GCursorSet(36,3);
	MIOS_LCD_Cmd(0xa7); // Set INVERSE characters.
	MIOS_LCD_PrintCString("GAME OVER");	
	MIOS_Delay(500);
	MIOS_GLCD_GCursorSet(6,4);
	MIOS_LCD_PrintCString("PRESS FIRE TO START");	 
	INVADER_New_Game(); // Clear the game data.
	game.CURRENT_SCREEN=GAME_OVER; // As we were probably called directly.
}


/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
	if (evnt2==0x0)
		button_pressed=0;
	else
		button_pressed=evnt1;
	
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has been received
// which has been specified in the MIOS_MPROC_EVENT_TABLE
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has not been completly
// received within 2 seconds
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyTimeout(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI byte has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedByte(unsigned char byte) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Prepare(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after the shift register have been loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Finish(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{
	if (pin_value==1)
	{
		button_pressed=0;
		return;
	}
	switch(pin)
	{
	case 0:
		button_pressed=FIRE_BUTTON;
		break;
	case 1:
		button_pressed=L_BUTTON;
		break;
	case 2:
		button_pressed=R_BUTTON;
		break;
	case 3:
		button_pressed=PAUSE_BUTTON;
		break;
	default:
		button_pressed=0;
	}	
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{
}
