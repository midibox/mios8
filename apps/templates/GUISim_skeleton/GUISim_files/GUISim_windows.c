#define RENDER_MODE 2 //0=solid 1=shaded 2=blended

#define LCD_SDL_WIDTH 570
#define LCD_SDL_HEIGHT 300
#define LCD_SDL_BPP 4
#define LCD_SDL_DEPTH 32
#define LCD_SDL_Y_OFFSET 20

SDL_Surface *LCD_SDL_screen, *LCD_SDL_text_line1[65], *LCD_SDL_text_line2[65], *LCD_SDL_text[128];
SDL_Surface *LCD_SDL_dout_led_image_on, *LCD_SDL_dout_led_image_off, *LCD_frame_image, *LCD_background_image;
SDL_Event   LCD_SDL_event;
TTF_Font    *LCD_SDL_font=0;

int LCD_SDL_stop_program = 0;
int LCD_SDL_h=0;
SDL_Color LCD_SDL_fg={200,200,220,255};
int LCD_SDL_minx, LCD_SDL_maxx, LCD_SDL_miny, LCD_SDL_maxy, LCD_SDL_advance;
Uint32 LCD_SDL_color[6];
int LCD_Cursor_x=0;
int LCD_Cursor_y=0;
SDL_Rect r, r2;
char LCD_Print_Buffer[65];
char button_mapping[32]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
char enc_saved_values[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char LCD_DOUT_Pin_State[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char entry_mode=0;


int start_glyph=0, style=TTF_STYLE_NORMAL, font_size=32;

typedef struct {
	int minx,
		maxx,
		miny,
		maxy,
		advance;
} GlyphMetrics;

GlyphMetrics gm1[65];
GlyphMetrics gm2[65];

#define SPACING 0

//int main_SDL(void);
void LCD_SDL_CursorSet(int pos)
{
    LCD_Cursor_y=0; //Assume Line 0
    if (pos>63) {
        LCD_Cursor_y=30;
        pos = pos - 64;
    }

    LCD_Cursor_x=pos;
//    printf("\nLCD_SDL: CursorSet: x=%d, y=%d",LCD_Cursor_x,LCD_Cursor_y);
}

void LCD_SDL_CursorIncrement(void)
{
    LCD_Cursor_x++; //Assume Line 0
    if (LCD_Cursor_x>63) {
        LCD_Cursor_x=0;
        if (LCD_Cursor_y == 0)
            LCD_Cursor_y = 30;
        else
            LCD_Cursor_y=0;
    }
}
void LCD_SDL_PrintChar(char c)
{
    switch (LCD_Cursor_y) {
        case 0:
            if(LCD_SDL_text_line1[LCD_Cursor_x]) {
                SDL_FreeSurface(LCD_SDL_text_line1[LCD_Cursor_x]);
                LCD_SDL_text_line1[LCD_Cursor_x]=0;
            }
            TTF_GlyphMetrics(LCD_SDL_font, c,&gm1[LCD_Cursor_x].minx, &gm1[LCD_Cursor_x].maxx,&gm1[LCD_Cursor_x].miny, &gm1[LCD_Cursor_x].maxy,&gm1[LCD_Cursor_x].advance);
            LCD_SDL_text_line1[LCD_Cursor_x]=TTF_RenderGlyph_Blended(LCD_SDL_font,c,LCD_SDL_fg);
            break;
        default:
            if(LCD_SDL_text_line2[LCD_Cursor_x]) {
                SDL_FreeSurface(LCD_SDL_text_line2[LCD_Cursor_x]);
                LCD_SDL_text_line2[LCD_Cursor_x]=0;
            }
            TTF_GlyphMetrics(LCD_SDL_font, c,&gm2[LCD_Cursor_x].minx, &gm2[LCD_Cursor_x].maxx,&gm2[LCD_Cursor_x].miny, &gm2[LCD_Cursor_x].maxy,&gm2[LCD_Cursor_x].advance);
			LCD_SDL_text_line2[LCD_Cursor_x]=TTF_RenderGlyph_Blended(LCD_SDL_font,c,LCD_SDL_fg);
            break;
    }
    LCD_SDL_CursorIncrement();
}

void LCD_SDL_PrintCString(code char *str)
{
    while (*str!=0) {
        LCD_SDL_PrintChar((char)*str);
        str++;
    }
}

void LCD_SDL_PrintBCD1(unsigned char v)
{
    int i;
    sprintf(LCD_Print_Buffer,"%1.1i",v);
    for(i=0;LCD_Print_Buffer[i]>0;i++) {
        LCD_SDL_PrintChar(LCD_Print_Buffer[i]);
    }
}

void LCD_SDL_PrintBCD2(unsigned char v)
{
    int i;

    sprintf(LCD_Print_Buffer,"%2.2i",v);
    for(i=0;LCD_Print_Buffer[i]>0;i++) {
        LCD_SDL_PrintChar(LCD_Print_Buffer[i]);
    }
}

void LCD_SDL_PrintBCD3(unsigned char v)
{
    int i;

    sprintf(LCD_Print_Buffer,"%3.3i",v);
    for(i=0;LCD_Print_Buffer[i]>0;i++) {
        LCD_SDL_PrintChar(LCD_Print_Buffer[i]);
    }
}

void LCD_SDL_PrintBCD4(unsigned int v)
{
    int i;

    sprintf(LCD_Print_Buffer,"%4.4i",v);
    for(i=0;LCD_Print_Buffer[i]>0;i++) {
        LCD_SDL_PrintChar(LCD_Print_Buffer[i]);
    }
}

void LCD_SDL_PrintBCD5(unsigned int v)
{
    int i;

    sprintf(LCD_Print_Buffer,"%5.5i",v);
    for(i=0;LCD_Print_Buffer[i]>0;i++) {
        LCD_SDL_PrintChar(LCD_Print_Buffer[i]);
    }
}

void LCD_SDL_Clear(void)
{
    int i;
//    printf("\nLCD_SDL: Clear");
    LCD_SDL_CursorSet(0x00);
    for(i=0;i<64;i++) {
        LCD_SDL_PrintChar(' ');
        if(LCD_SDL_text_line1[i]) {
            SDL_FreeSurface(LCD_SDL_text_line1[i]);
            LCD_SDL_text_line1[i]=0;
        }
    }
    LCD_SDL_CursorSet(0x40);
    for(i=0;i<64;i++) {
        LCD_SDL_PrintChar(' ');
        if(LCD_SDL_text_line2[i]) {
            SDL_FreeSurface(LCD_SDL_text_line2[i]);
            LCD_SDL_text_line2[i]=0;
        }
    }

    LCD_SDL_CursorSet(0);
}

//////////////////////////////////////////////////////////////////
// This function draws/redraws the LCD display and DOUT Pin states
//////////////////////////////////////////////////////////////////
void LCD_SDL_DrawScreen(SDL_Surface* screen, int h)
{
    int i;

    if(SDL_MUSTLOCK(screen))
    {
        if(SDL_LockSurface(screen) < 0) return;
    }
    SDL_FillRect(screen, NULL, LCD_SDL_color[0]);

    //LCD Background
    r2.x=0;r2.y=0;r2.h=LCD_SDL_HEIGHT;r2.w=LCD_SDL_WIDTH;
    SDL_BlitSurface(LCD_background_image,0,screen,&r2);

    //LCD Frame Background
    r2.x=0;r2.y=0;r2.h=90;r2.w=580;
    SDL_BlitSurface(LCD_frame_image,0,screen,&r2);

    //Set Background block width and height
    r2.w=13;
    r2.h=20;

    //LCD Line 1 background
    r2.x=4; r2.y=4+LCD_SDL_Y_OFFSET;
    for (i=4;i<560;i+=14){
        r2.x=i;
        SDL_FillRect(screen,&r2,LCD_SDL_color[1]);
    }

    //LCD Line 2 background
    r2.x=4; r2.y=26+LCD_SDL_Y_OFFSET;
    for (i=4;i<560;i+=14){
        r2.x=i;
        SDL_FillRect(screen,&r2,LCD_SDL_color[1]);
    }


    //Set font width and height
    r2.w=13;
    r2.h=20;


    //Show line1 on LCD
    r2.y=5+LCD_SDL_Y_OFFSET;
    r2.x=6;
    for (i=0;i<40;i++) {
        r2.y=0+LCD_SDL_Y_OFFSET;
		r2.y=r2.y+TTF_FontAscent(LCD_SDL_font)-gm1[i].maxy;
        SDL_BlitSurface(LCD_SDL_text_line1[i],0,screen,&r2);
        r2.x=r2.x+14;
    }

    //Show line2 on LCD
    r2.y=25+LCD_SDL_Y_OFFSET;
    r2.x=6;
    for (i=0;i<40;i++) {
        r2.y=22+LCD_SDL_Y_OFFSET;
		r2.y=r2.y+TTF_FontAscent(LCD_SDL_font)-gm2[i].maxy;
        SDL_BlitSurface(LCD_SDL_text_line2[i],0,screen,&r2);
        r2.x=r2.x+14;
    }

    // Display DOUT Pins State
    r2.w=20;
    r2.h=20;

    //Show LEDs
    r2.x=4; r2.y=160+LCD_SDL_Y_OFFSET;
    for (i=0;i<16;i++){
        r2.x=500 - (i*30+5);
//        SDL_FillRect(screen,&r2,color[LCD_DOUT_Pin_State[i]+2]);

        if (LCD_DOUT_Pin_State[15-i]>0)
            SDL_BlitSurface(LCD_SDL_dout_led_image_on,0,screen,&r2);
        else
            SDL_BlitSurface(LCD_SDL_dout_led_image_off,0,screen,&r2);

    }
    r2.x=4; r2.y=200+LCD_SDL_Y_OFFSET;
    for (i=0;i<16;i++){
        r2.x=500 - (i*30+5);
//        SDL_FillRect(screen,&r2,color[LCD_DOUT_Pin_State[i]+2]);

        if (LCD_DOUT_Pin_State[31-i+16]>0)
            SDL_BlitSurface(LCD_SDL_dout_led_image_on,0,screen,&r2);
        else
            SDL_BlitSurface(LCD_SDL_dout_led_image_off,0,screen,&r2);

    }


    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

    SDL_Flip(screen);
}

void free_font()
{
	if(LCD_SDL_font)
		TTF_CloseFont(LCD_SDL_font);
	LCD_SDL_font=0;
}

void free_glyphs()
{
	int i;

	for(i=0; i<40; i++)
	{
		if(LCD_SDL_text_line1[i])
			SDL_FreeSurface(LCD_SDL_text_line1[i]);
		if(LCD_SDL_text_line2[i])
			SDL_FreeSurface(LCD_SDL_text_line2[i]);
		LCD_SDL_text_line1[i]=0;
		LCD_SDL_text_line2[i]=0;
	}
	for(i=0; i<128; i++)
	{
		if(LCD_SDL_text[i])
			SDL_FreeSurface(LCD_SDL_text[i]);
		LCD_SDL_text[i]=0;
	}
}


int totval=0;
static void printdial(Ihandle *c, double a, char* color)
{
  Ihandle* label = NULL;

  label = IupGetHandle("enc1_lbl");

  if (label)
  {
    IupSetfAttribute(label, IUP_TITLE, "%3d", (int)a);
    IupSetAttribute(label, IUP_BGCOLOR, LCD_SDL_color);
  }
}

static int mousemove(Ihandle *c, double a)
{
    int diff;
  printdial(c, a, "0 255 0");

  diff=(int) a -  enc_saved_values[atoi(IupGetAttribute(c,IUP_TITLE))];

  if (abs(diff)>1) {
    diff=(diff>0)?-1:1;
    printf("\nLCD_SDL: ENC %s - (Rel:%d) : (Abs:%d)",IupGetAttribute(c,IUP_TITLE),diff,(int) a);
    ENC_NotifyChange(atoi(IupGetAttribute(c,IUP_TITLE)), diff);
  }

  enc_saved_values[atoi(IupGetAttribute(c,IUP_TITLE))]= (int) a;
  return IUP_DEFAULT;
}

static int button_press(Ihandle *c,double a)
{
  printdial(c, a, "255 0 0");
  return IUP_DEFAULT;
}

static int button_release(Ihandle *c,double a)
{
  printdial(c, a, NULL);
  enc_saved_values[atoi(IupGetAttribute(c,IUP_TITLE))]=0;
  return IUP_DEFAULT;
}
static int action_end(Ihandle *c, int button, int pressed, int x, int y, char *status)
{
    if (pressed>1) return IUP_DEFAULT;
    printf("\nLCD_SDL: BTN %s ( %d )",IupGetAttribute(c,IUP_TITLE),pressed);
    DIN_NotifyToggle(button_mapping[atoi(IupGetAttribute(c,IUP_TITLE))],pressed);
  return IUP_DEFAULT;
}


static int action_entrymode(Ihandle *c1, int button, int pressed, int x, int y, char *status)
{
    char str[100] = "";
	char choice = 'q';
	unsigned char c = 0;
	unsigned int i;
	unsigned int pin = 0;
	unsigned int value = 0;
	unsigned int value2 = 0;


  int fields_read;
  int integer = 12;
  float real = 1e-3;
  char text2[300]="This is a vector of characters";
  char string[350];
  char *fmt =
  {
   "ACSim text command\n"
   "Command: %100.40%s\n"
  };



  fields_read = IupScanf (fmt, str);

  if(fields_read == -1)
  {
    IupMessage("IupScanf","Operation canceled");
  }
  else
  {
        printf("\nLCD_SDL: ACSim Command:  %s Entered",str);
        choice = str[0];
        printf("\nChoice: %c",str[0]);
        switch (choice) {

            case ' ': // OK
                debug_din_value[DEBUG_BUTTON_OK] = 0;
                DIN_NotifyToggle(DEBUG_BUTTON_OK, debug_din_value[DEBUG_BUTTON_OK]);
                break;

            case '+': // ENC++
                for(i=0;i<32;i++) { if(str[i]=='+') { c++; } }
                    ENC_NotifyChange(DEBUG_ENCODER, c);
                break;
            case '-': // ENC --
                for(i=0;i<32;i++) { if(str[i]=='-') { c--; } }
                ENC_NotifyChange(DEBUG_ENCODER, c);
                break;

            case 'a': // AIN...
                sscanf(str, "a%i,%i", &pin, &value);
                if((pin > DEBUG_AIN_NUM) || (value > 1023)) {
                    printf("! max_pin=9, max_value=1023 !");
                    pin = 0; value = 0;
                } else {
                    printf("scanning pin %i: %i", pin, value);
                    // "set" pin values:
                    debug_ain_value[pin] = value;
                    AIN_NotifyChange(pin, value);
                }
                break;

            case 'b': // System Realtime BYTE
                debug_MIDI_byteNum = 0;	// sending single byte, maybe without MIDI_START/_STOP
                sscanf(str, "b%i", &pin);
                MPROC_NotifyReceivedByte(pin);
                debug_MIDI_byteNum = 0;
                break;

            case 'd': // DIN...
                sscanf(str, "d%i,%i", &pin, &value);
                // invert value for easier rememberance
                value = value ^ 0x1;
                debug_din_value[pin] = value;
                DIN_NotifyToggle(pin, value);
                break;

            case 'e': // ENC # ++/--
                sscanf(str, "e%i", &pin);
                if(pin > DEBUG_ENC_NUM) {
                    printf("! max_enc = %i", (DEBUG_ENC_NUM - 1));
                    return;
                }
                for(i=3;i<32;i++) {
                    if(str[i]=='+') {
                        c++;
                    } else if(str[i]=='-') {
                        c--;
                    }
                }
                ENC_NotifyChange(pin, c);
                break;

            case 'j': // set jumper pin
                sscanf(str, "j%i", &pin);
                switch(pin) {
                    case 10:
                        PORTCbits.RC5 = 1;
                        Timer();
                        PORTCbits.RC5 = 0;
                        Timer();
                        break;
                    case 14:
                        PORTDbits.RD4 = 1;
                        Timer();
                        PORTDbits.RD4 = 0;
                        Timer();
                        break;
                    default:
                        printf("pin %i not (yet?) supported", pin);
                        break;
                }
                break;

            case 'm': // MIDI Receive (m msgType, argA, argB)
                sscanf(str, "m%i,%i,%i", &pin, &value, &value2);
                MPROC_NotifyReceivedEvnt(pin,value,value2);
                break;

            case 'n': // send note_on (n noteValue, volume, {channel})
                sscanf(str, "n%i,%i,%i", &pin, &value, &value2);
                if((value2 < 1) || (value2 > 16)) { value2 = 1; }
                MPROC_NotifyReceivedEvnt((143+value2),pin,value);
                break;

            case 'r': // random
                pin = ACRandomPin();
                value = ACRandomInt();
                printf("scanning pin %i: %i", pin, value);
                // "set" pin values:
                debug_ain_value[pin] = value;
                AIN_NotifyChange(pin, value);
                break;

            case 't': // test function
                sscanf(str, "t%i", &value);
        //			for example:
        //			IIC_SPEAKJET_Transmit14bit(value);
        //			printf("\nDEC: %i \tPARAM3: %X \tPARAM2: %X \tPARAM1: %X", value, MIOS_PARAMETER3, MIOS_PARAMETER2, MIOS_PARAMETER1);
                break;

            default:
                // nothing...
                break;
        }
  }


  return IUP_DEFAULT;
}

static int k_any(Ihandle *self, int c)
{

return IUP_DEFAULT;

  switch (c)
  {
    case K_CR:
    case K_SP:
    case K_ESC:
    case K_INS:
    case K_DEL:
    case K_TAB:
    case K_HOME:
    case K_UP:
    case K_PGUP:
    case K_LEFT:
    case K_MIDDLE:
    case K_RIGHT:
    case K_END:
    case K_DOWN:
    case K_PGDN:
      return IUP_IGNORE;
    default:
      return IUP_DEFAULT;
  }
}


static int key_act(Ihandle *self, int c1, char *new_value)
{

    entry_mode=1;
    switch (c1)
    {
    case K_CR:
        entry_mode=0;
      return IUP_DEFAULT;
    default:
       break;
    }

  return (char) c1;
}

static int action_exit(Ihandle *c1, int button, int pressed, int x, int y, char *status)
{
  LCD_SDL_stop_program=1;
  return IUP_DEFAULT;
}

void LCD_SDL_Run(void) {
  int i=0;
  char wrk_str[5];
  char *error=NULL;
  Ihandle *dlg=NULL;
  Ihandle *cmd0=NULL,*cmd_exit=NULL;
  Ihandle *enc[16]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  Ihandle *btn[32]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  Ihandle *text=NULL;
  Ihandle *mnu;

// Set up IUP for Control surface
  IupOpen(NULL,NULL);
  IupControlsOpen () ;
  IupSetLanguage(IUP_ENGLISH);

  if((error = IupLoad("GUISim_files/InputSurface.led")) != NULL)
  {
    IupMessage("%s\n", error);
    return;
  }

  dlg = IupGetHandle("dlg");

  // Set up 16 Encoders
  for (i=0;i<16;i++) {
      sprintf(wrk_str,"enc%d",i);
      enc[i]= IupGetHandle(wrk_str);
      IupSetAttribute(enc[i], ICTL_BUTTON_PRESS_CB,     "button_press");
      IupSetAttribute(enc[i], ICTL_BUTTON_RELEASE_CB,   "button_release");
      IupSetAttribute(enc[i], ICTL_MOUSEMOVE_CB,        "mousemove");
  }

  // Set up 16 32 Buttons
  for (i=0;i<16;i++) {
      sprintf(wrk_str,"btn%d",i);
      btn[i]= IupGetHandle(wrk_str);
      IupSetAttribute(btn[i], IUP_BUTTON_CB, "action_end");
  }


  cmd_exit = IupGetHandle("cmd_exit");

  IupSetFunction("mousemove", (Icallback) mousemove);
  IupSetFunction("button_press", (Icallback) button_press);
  IupSetFunction("button_release", (Icallback) button_release);
  IupSetFunction("action_end", (Icallback) action_end);
  IupSetFunction("key_act", (Icallback) key_act);
  IupSetFunction("k_any", (Icallback) k_any);
  IupSetFunction("action_entrymode", (Icallback) action_entrymode);
  IupSetFunction("action_exit", (Icallback) action_exit);

  IupSetCallback(text, IUP_ACTION, (Icallback) key_act);
  IupSetCallback(text, IUP_K_ANY, (Icallback) k_any);

  IupSetAttribute(cmd0, ICTL_BUTTON_PRESS_CB, "action_entrymode");
  IupSetAttribute(cmd_exit, ICTL_BUTTON_PRESS_CB, "action_exit");

  IupSetAttributes( dlg, "MENUBOX = NO, MAXBOX = NO, MINBOX = NO" );

  mnu = IupMenu(
  IupSubmenu("View Configuration",IupMenu(
      IupSetAttributes(IupItem("CORE", "myaction"), "VALUE=ON"),
      IupSeparator(),
      IupSetAttributes(IupItem("LCD 2X16", "myaction"), "VALUE=OFF"),
      IupSetAttributes(IupItem("LCD 1X40", "myaction"), "VALUE=OFF"),
      IupSetAttributes(IupItem("LCD 2X40", "myaction"), "VALUE=ON"),
      IupSeparator(),
      IupSetAttributes(IupItem("DINX4 -  0 Encoders, 32 Buttons", "myaction"), "VALUE=OFF"),
      IupSetAttributes(IupItem("DINX4 -  2 Encoders, 28 Buttons", "myaction"), "VALUE=OFF"),
      IupSetAttributes(IupItem("DINX4 -  4 Encoders, 24 Buttons", "myaction"), "VALUE=OFF"),
      IupSetAttributes(IupItem("DINX4 -  8 Encoders, 16 Buttons", "myaction"), "VALUE=ON"),
      IupSetAttributes(IupItem("DINX4 - 12 Encoders,  8 Buttons", "myaction"), "VALUE=OFF"),
      IupSetAttributes(IupItem("DINX4 - 16 Encoders,  0 Buttons", "myaction"), "VALUE=OFF"),
      IupSeparator(),
      IupSetAttributes(IupItem("DOUTX4 - 32 Pin Status LEDs", "myaction"), "VALUE=ON"),
      IupSetAttributes(IupItem("AOUT (not functioning yet)", "myaction"), "VALUE=OFF"),

      NULL)),
    NULL);
  IupSetHandle("mnu",mnu);

  IupSetAttribute(dlg,IUP_MENU,"mnu");


  IupShowXY(dlg,IUP_CENTER,IUP_CENTER);


// Setup SDL for LCD Display

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;


    if (!(LCD_SDL_screen = SDL_SetVideoMode(LCD_SDL_WIDTH, LCD_SDL_HEIGHT, LCD_SDL_DEPTH, SDL_RESIZABLE|SDL_SWSURFACE)))
    {
        SDL_Quit();
        return 1;
    }

	/* start SDL_ttf */
	if(TTF_Init()==-1)
	{
		printf("TTF_Init: %s\n", TTF_GetError());
		SDL_Quit();
		return 2;
	}
	atexit(TTF_Quit); /* remember to quit SDL_ttf */
	atexit(free_font); /* remember to free any loaded font and glyph cache */
	atexit(free_glyphs); /* remember to free any loaded font and glyph cache */
    LCD_SDL_font=TTF_OpenFont("GUISim_files/FreeMonoBold.ttf", 16);
	if(!LCD_SDL_font)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		SDL_Quit();
		exit(3);
	}

	free_glyphs();

    TTF_SetFontStyle(LCD_SDL_font, TTF_STYLE_BOLD);
	if(style!=TTF_GetFontStyle(LCD_SDL_font))
		TTF_SetFontStyle(LCD_SDL_font,style);


    LCD_SDL_dout_led_image_on = SDL_LoadBMP("GUISim_files/LEDON.bmp");
    LCD_SDL_dout_led_image_off = SDL_LoadBMP("GUISim_files/LEDOFF.bmp");
    LCD_frame_image = SDL_LoadBMP("GUISim_files/LCD_frame.bmp");
    LCD_background_image = SDL_LoadBMP("GUISim_files/LCD_background.bmp");

	LCD_SDL_color[0]=SDL_MapRGB(LCD_SDL_screen->format,255,255,255);
	LCD_SDL_color[1]=SDL_MapRGB(LCD_SDL_screen->format,20,60,20);
	LCD_SDL_color[2]=SDL_MapRGB(LCD_SDL_screen->format,150,0,0);
	LCD_SDL_color[3]=SDL_MapRGB(LCD_SDL_screen->format,255,255,10);
	LCD_SDL_color[4]=SDL_MapRGB(LCD_SDL_screen->format,0,255,255);
	LCD_SDL_color[5]=SDL_MapRGB(LCD_SDL_screen->format,0,0,255);
	LCD_SDL_color[6]=SDL_MapRGB(LCD_SDL_screen->format,128,0,128);

		TTF_GlyphMetrics(LCD_SDL_font,'g', &LCD_SDL_minx, &LCD_SDL_maxx, &LCD_SDL_miny, &LCD_SDL_maxy, &LCD_SDL_advance);
//		printf("TTF_GlyphMetrics('g'):\n\tminx=%d\n\tmaxx=%d\n\tminy=%d\n\tmaxy=%d\n\tadvance=%d\n",minx, maxx, miny, maxy, advance);

    SDL_WM_SetCaption("MIDIBOX Simulator Output Control","WINSIM");

	printf("\n++++++ MIOS-DEBUG-CONSOLE - GUI Enabled++++++\n");
	printf("(r)and (a)(pin),(value) (d)(pin),(state) \n");
	printf("(e{opt.})(++)(-) (j)umper(pin) \n");
	printf("(m)idi(msg),(argA),(argB) \n");
	printf("(n)ote_on(value),(velocity),{channel} \n");
	printf("(SPACE)OK e(x)it + [ENTER] \n\n");

//Andre check this	srand(); // set the random seed

	// init MIOS
	MIOS_BOX_STAT.BS_AVAILABLE = 1;
	Init();
	DISPLAY_Init();

	printf("\n");

	// runloop
//	runloop(0);
//andre end


    LCD_SDL_DrawScreen(LCD_SDL_screen,LCD_SDL_h++);

    while(!LCD_SDL_stop_program)
    {
         IupLoopStep();

         Tick();
         DISPLAY_Tick();
         LCD_SDL_DrawScreen(LCD_SDL_screen,LCD_SDL_h++);
         if (entry_mode==0) {
             while(SDL_PollEvent(&LCD_SDL_event))
             {
                  switch (LCD_SDL_event.type)
                  {
                      case SDL_QUIT:
//                      IupMessage("Information Message","To close the program\nPlease use the Stop and Exit button\nin the Input Control Dialog");
                      LCD_SDL_stop_program = 1;
                      break;
                      case SDL_KEYDOWN:
                            entry_mode=1;
                           break;
                  }
             }

         }
    }

    SDL_Quit();

// Close down IUP
  IupDestroy(dlg);

  IupControlsClose() ;
  IupClose();

}




