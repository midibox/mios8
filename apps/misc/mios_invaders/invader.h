/*
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Phil Taylor (phil.taylor@lansyste,s.co.uk)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _INVADER_H
#define _INVADER_H

/////////////////////////////////////////////////////////////////////////////
// Exported variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// global definitions
/////////////////////////////////////////////////////////////////////////////

#define SPLASH_SCREEN	0
#define SCORE_PAGE		1
//#define HALL_OF_FAME	1
#define GAME_RUNNING	2
#define GAME_PAUSED		3
#define GAME_OVER		4

#define L_BUTTON 	0x1a
#define R_BUTTON	0x1c
#define FIRE_BUTTON 0x18
#define PAUSE_BUTTON	0x1d

#define SHIP_WIDTH 	9	//The width of the ship in pixels. This does not include the two blanks
#define INVADER_WIDTH	10	//The width of the invaders. minus the two outside blanks 
#define INVADER_RIGHT_LIMIT 127 - INVADER_WIDTH	//The right most limit for the invaders before they need to move down
#define SAUCER_WIDTH 14
        
#define FORWARD 1
#define BACKWARD 0
 		
/////////////////////////////////////////////////////////////////////////////
// Type definitions
/////////////////////////////////////////////////////////////////////////////
 
typedef union {
  struct {
    unsigned ALL:16;
  };
  struct {
	unsigned CURRENT_SCREEN:3;
    unsigned TIME_TO_MOVE_SHIP_MISSILE:1;
    unsigned SAUCER_ENABLED:1;
    unsigned TIME_TO_MOVE_SAUCER:1;
    unsigned TIME_TO_MOVE_BASE:1;
    unsigned CURSOR_POS_RECEIVED:1;
    unsigned MISSILE_FIRED:1;
    unsigned MISSILE_HIT:1;
    unsigned SAUCER_HIT:1;
    unsigned SHIP_HIT:1;
    unsigned BASE_HIT:1;
    unsigned :3;
  };
} game_status_t;

typedef union {
  struct {
    unsigned ALL:16;
  };
  struct {
    unsigned TIME_TO_MOVE_INVADERS:1;
    unsigned TIME_TO_MOVE_INVADERS_DOWN:1;
    unsigned INVADER_MISSILE_FIRED:1;
    unsigned TIME_TO_MOVE_INV_MISSILE:1;
    unsigned INVADER_MISSILE_HIT:1;
    unsigned INVADER_MISSILE2_FIRED:1;
    unsigned TIME_TO_MOVE_INV_MISSILE2:1;
    unsigned INVADER_MISSILE2_HIT:1;
    unsigned INVADERS_REACHED_BOTTOM:1;
    unsigned INVADERS_DIRECTION:1;
    unsigned :6;
  }; 
} invader_status_t;



/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void INVADER_Screen_Intro(void);
void INVADER_Screen_Hall_Of_Fame(void);
void INVADER_Screen_Scoring(void);
void INVADER_Move_Invaders(void);
void INVADER_Move_Invaders_Down(void);
void INVADER_Draw_Invaders(void);
void INVADER_Reset_Invaders(void);
void INVADER_New_Sheet(void);
void INVADER_Update_Bases(void);
void INVADER_Check_Base_Hit(void);
void INVADER_LCD_UnPlot(unsigned char x, unsigned char y);
void INVADER_LCD_Plot(unsigned char x, unsigned char y);
void INVADER_Draw_Missile(void);
void INVADER_Clear_Missile(void);
void INVADER_Check_For_Invader_Missile2_Hit(void);
void INVADER_Check_For_Invader_Missile_Hit(void);
void INVADER_Govern_Speeds(void);
void INVADER_Move_Saucer(void);
void INVADER_Draw_Saucer(void);
void INVADER_Clear_Saucer(void);
void INVADER_Move_Missile(void); 
void INVADER_Ship_Right(void);
void INVADER_Ship_Left(void);
void INVADER_Check_For_Missile_Hit(void);
void INVADER_Fire_Invader_Missile(void);
void INVADER_Move_Invader_Missile(void);
void INVADER_Draw_Invader_Missile(void);
void INVADER_Clear_Invader_Missile(void);
void INVADER_Draw_Invader_Missile2(void);
void INVADER_Clear_Invader_Missile2(void);
void INVADER_Fire_Invader_Missile2(void);
void INVADER_Move_Invader_Missile2(void);
void INVADER_Draw_Ship(void);
void INVADER_Game_Over(void);
void INVADER_New_Game(void);

#endif /* _INVADER_H */
