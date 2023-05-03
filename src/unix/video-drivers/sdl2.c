/*                                       

 This is the SDL XMAME display driver.
 FIrst incarnation by Tadeusz Szczyrba <trevor@pik-nel.pl>,
 based on the Linux SVGALib adaptation by Phillip Ezolt.

 updated and patched by Ricardo Calixto Quesada (riq@core-sdi.com)
 
 patched by Patrice Mandin (pmandin@caramail.com)
  modified support for fullscreen modes using SDL and XFree 4
  added toggle fullscreen/windowed mode (Alt + Return)
  added title for the window
  hide mouse cursor in fullscreen mode
  added command line switch to start fullscreen or windowed
  modified the search for the best screen size (SDL modes are sorted by
    Y size)

 patched by Dan Scholnik (scholnik@ieee.org)
  added support for 32bpp XFree86 modes
  new update routines: 8->32bpp & 16->32bpp

 updated for sdl2 by Arco van Geest <arco@appeltaart.mine.nu>
  removed PARANOIC and HERMES for now.
  only depth=16 works

 TODO: 
       Test the 16bpp->24bpp update routine
       Test the 16bpp->32bpp update routine
       Improve performance.
       Test mouse buttons (which games use them?)

***************************************************************************/
#define __SDL2_C

#undef SDL_DEBUG
#define SDL_DEBUG


#include <sys/ioctl.h>
#include <sys/types.h>
#include <SDL.h>
#include "xmame.h"
#include "devices.h"
#include "keyboard.h"
#include "driver.h"
#include "SDL-keytable.h"
#include "effect.h"
#include "../video.h"

static int Vid_width;
static int Vid_height;
static int Vid_depth = 8;
//1 static SDL_Surface* Offscreen_surface;
static int hardware=1;
static int mode_number=-1;
static int start_fullscreen=0;
SDL_Color *Colors=NULL;
static int cursor_state; /* previous mouse cursor state */


//SDL_Window* screen = NULL;
SDL_Event event;
SDL_Rect source, destination, dst;
SDL_Texture *sdlTexture;
SDL_Window *sdlWindow ;
SDL_Renderer* sdlRenderer;
SDL_Palette* sdlPalette;
SDL_Surface* sdlSurface;
SDL_Surface* sdlSurfaceOptimized;
//SDL_Texture * texture;

//Uint32 *pixels;

#define QUITKEY SDLK_ESCAPE
#define WIDTH 1024
#define HEIGHT 768
Uint16 * sdlPixels = NULL;

typedef void (*update_func_t)(struct mame_bitmap *bitmap);

update_func_t update_function;

static int sdl_mapkey(struct rc_option *option, const char *arg, int priority);

static int list_sdl_modes(struct rc_option *option, const char *arg, int priority);

struct rc_option display_opts[] = {
    /* name, shortname, type, dest, deflt, min, max, func, help */
   { "SDL Related",  NULL,    rc_seperator,  NULL,
       NULL,         0,       0,             NULL,
       NULL },
   { "fullscreen",   NULL,    rc_bool,       &start_fullscreen,
      "0",           0,       0,             NULL,
      "Start fullscreen" },
   { "listmodes",    NULL,    rc_use_function_no_arg,       NULL,
      NULL,           0,       0,             list_sdl_modes,
      "List all posible fullscreen modes" },
   { "modenumber",   NULL,    rc_int,        &mode_number,
      "-1",          0,       0,             NULL,
      "Try to use the fullscreen mode numbered 'n' (see the output of -listmodes)" },
   { "sdlmapkey",	"sdlmk",	rc_use_function,	NULL,
     NULL,		0,			0,		sdl_mapkey,
     "Set a specific key mapping, see xmamerc.dist" },
   { NULL,           NULL,    rc_end,        NULL,
      NULL,          0,       0,             NULL,
      NULL }
};

void sdl_update_16_to_16bpp(struct mame_bitmap *bitmap);
void sdl_update_16_to_24bpp(struct mame_bitmap *bitmap);
void sdl_update_16_to_32bpp(struct mame_bitmap *bitmap);
void sdl_update_rgb_direct_32bpp(struct mame_bitmap *bitmap);


void plot32(int x,int y ,int red, int green, int blue, int o){
    sdlPixels[y * Vid_width + x] = (red&0xff)<<24 | (green & 0xff)<<16 | (blue & 0xff)<<8|(o & 0xff) ;
}
void plot(int x,int y ,int red, int green, int blue, int o){
   sdlPixels[y * Vid_width + x] = (red&0x1f)<<11 | (green & 0x2f)<<5 | (blue & 0x1f) ;
}

int sysdep_init(void)
{
   //    SDL_Init(SDL_INIT_VIDEO);
   if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
      fprintf (stderr, "SDL2: Error: %s\n",SDL_GetError());
      return OSD_NOT_OK;
   } 
   SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);
   SDL_FlushEvent(SDL_KEYDOWN);

   fprintf (stderr, "SDL2: Info: SDL2 initialized\n");
   atexit (SDL_Quit);
   return OSD_OK;
} //sysdep_init

void sysdep_close(void)
{
   SDL_DestroyRenderer(sdlRenderer);
	SDL_DestroyWindow(sdlWindow);
   SDL_Quit();
} //sysdep_close

int sysdep_create_display(int depth)
{
   Vid_width = visual_width;
   Vid_height = visual_height;
   Vid_depth = depth ;

   if ( sdlPixels ){
      free(sdlPixels);
   }

   if (!sdlWindow) {
         sdlWindow = SDL_CreateWindow(title,
         SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Vid_width, Vid_height, 0);
         SDL_MaximizeWindow(sdlWindow);
   }

   if (!sdlWindow) {
		fprintf(stderr,"InitSetup failed to create window");
      exit (OSD_NOT_OK);
	}

   sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
   if (!sdlRenderer) {
		fprintf(stderr,"InitSetup failed to create renderer");
      exit (OSD_NOT_OK);
	}


// CHANGE
//        SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_STATIC, Vid_width, Vid_height);
   
   if ( depth == 16 ) {
      sdlPixels = (Uint16 *) malloc (sizeof(Uint16)*Vid_width*Vid_height);
      sdlSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Vid_width, Vid_height, 16, 0, 0, 0, 0);
      sdlTexture = SDL_CreateTexture(sdlRenderer,
          SDL_PIXELFORMAT_RGB555,SDL_TEXTUREACCESS_STATIC , Vid_width, Vid_height);
   }
   if ( depth == 24 ) {
      // ?
      sdlPixels = (Uint32 *) malloc (sizeof(Uint32)*Vid_width*Vid_height);
      sdlSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Vid_width, Vid_height, 16, 0, 0, 0, 0);
   
      sdlTexture = SDL_CreateTexture(sdlRenderer,
          SDL_PIXELFORMAT_RGB888,SDL_TEXTUREACCESS_STATIC , Vid_width, Vid_height);
   }
   if ( depth == 32 ) {
      sdlSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Vid_width, Vid_height, 16, 0, 0, 0, 0);
   
      sdlPixels = (Uint32 *) malloc (sizeof(Uint32)*Vid_width*Vid_height);
            sdlTexture = SDL_CreateTexture(sdlRenderer,
          SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC , Vid_width, Vid_height);
   }
      if (!sdlSurface) {
		fprintf(stderr,"InitSetup failed to create sdlSurface");
      exit (OSD_NOT_OK);
	}

   if (!sdlTexture) {
		fprintf(stderr,"InitSetup failed to create texture");
      exit (OSD_NOT_OK);
	}

   // clear screen
   SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
   SDL_RenderClear(sdlRenderer);
   SDL_RenderPresent(sdlRenderer);

   // just some info
   SDL_Log("SDL w,h,d: %d,%d,%d", Vid_width,Vid_height,Vid_depth);
   SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
   SDL_RenderSetLogicalSize(sdlRenderer, Vid_width, Vid_height);

   SDL_Log( "requested depth %d", depth );
   //sdlTexture = SDL_CreateTexture(sdlRenderer,
   //                            SDL_PIXELFORMAT_ARGB8888,
   //                            SDL_TEXTUREACCESS_STREAMING,
   //                            Vid_width, Vid_height);
//   SDL_UpdateTexture(sdlTexture, NULL, myPixels, 640 * sizeof (Uint32));


// FIXME how can I clean up events?
SDL_FlushEvents(SDL_KEYDOWN,SDL_KEYUP);
SDL_FlushEvent(SDL_KEYDOWN);
SDL_Event event;
while(SDL_PollEvent(&event)) {
   fprintf(stderr, "clear events");
}

   if( depth == 16 )
   {
               fprintf (stderr, "SDL: supported Vid_depth=%d in depth=%d\n", Vid_depth,depth);
      switch( Vid_depth ) {
      case 32:
         update_function = &sdl_update_16_to_32bpp;
         break;
      case 24:
         update_function = &sdl_update_16_to_24bpp;
         break;
      case 16:
         update_function = &sdl_update_16_to_16bpp;
//         update_function = &sysdep_update_display;
         break;
      default:
         fprintf (stderr, "SDL: Unsupported Vid_depth=%d in depth=%d\n", Vid_depth,depth);
         SDL_Quit();
         exit (OSD_NOT_OK);
         break;
      }
   }
   else if (depth == 32)
   {
      if (Vid_depth == 32)
      {
               fprintf (stderr, "SDL: supported Vid_depth=%d in depth=%d\n", Vid_depth,depth);
         update_function = &sdl_update_rgb_direct_32bpp; 
      }
      else
      {
         fprintf (stderr, "SDL: Unsupported Vid_depth=%d in depth=%d\n",
            Vid_depth, depth);
         SDL_Quit();
         exit (OSD_NOT_OK);
      }
   }
   else
   {
      fprintf (stderr, "SDL: Unsupported depth=%d\n", depth);
      SDL_Quit();
      exit (OSD_NOT_OK);
   }
//update_function = &sysdep_update_display;

   // * Creating event mask * /
   SDL_EventState(SDL_KEYUP, SDL_ENABLE);
   SDL_EventState(SDL_KEYDOWN, SDL_ENABLE);
   //SDL_EnableUNICODE(1);
   
    memset(&display_palette_info, 0, sizeof(struct sysdep_palette_info));
    display_palette_info.depth = Vid_depth;
    if (Vid_depth == 8)
         display_palette_info.writable_colors = 256;
    else if (Vid_depth == 16) {
      display_palette_info.red_mask = 0xF800;
      display_palette_info.green_mask = 0x07E0;
      display_palette_info.blue_mask   = 0x001F;
    }
    else {
      display_palette_info.red_mask   = 0x00FF0000;
      display_palette_info.green_mask = 0x0000FF00;
      display_palette_info.blue_mask  = 0x000000FF;
    };


   // * Hide mouse cursor and save its previous status * /
   cursor_state = SDL_ShowCursor(0);

   effect_init2(depth, Vid_depth, Vid_width);
	if (normal_palette)
	{
		sysdep_palette_destroy(normal_palette);
		normal_palette = NULL;
	}

	if (debug_palette)
	{
		sysdep_palette_destroy(debug_palette);
		debug_palette = NULL;
	}

/*-- Colours --
#define CORE_COLOR(x)  Machine->pens[(x)]
#define COL_DMD        1
#define COL_DMDOFF     (COL_DMD+0)
#define COL_DMD33      (COL_DMD+1)
#define COL_DMD66      (COL_DMD+2)
#define COL_DMDON      (COL_DMD+3)
#define COL_DMDCOUNT   4
#define COL_LAMP       (COL_DMD+COL_DMDCOUNT)
#define COL_LAMPCOUNT  8
#define COL_SHADE(x)   (COL_LAMPCOUNT+(x))
#define COL_DMDAA      (COL_LAMP+COL_LAMPCOUNT*2)
#define COL_DMDAACOUNT 7
#define COL_SEGAAON1   (COL_DMDAA+COL_DMDAACOUNT)
#define COL_SEGAAON2   (COL_SEGAAON1+1)
#define COL_SEGAAOFF1  (COL_SEGAAON1+2)
#define COL_SEGAAOFF2  (COL_SEGAAON1+3)
#define COL_SEGAACOUNT 4
#define COL_COUNT      (COL_SEGAAON1+COL_SEGAACOUNT)
*/
/*  Machine->pens[0]=0x0000;
  Machine->pens[1]=0x2200; //dmd0
  Machine->pens[2]=0x55c0; //dmd33
  Machine->pens[3]=0x88c0; //dmd66
  Machine->pens[4]=0xffc0; //dmd100
  Machine->pens[5]=0xffff; //lamp0
  Machine->pens[6]=0x8800; //lamp1
  Machine->pens[7]=0x4400; //lamp2
  Machine->pens[8]=0x2200; //lamp3
  Machine->pens[9]=0x03c0; //lamp4
  Machine->pens[10]=0x0;   //lamp5
  Machine->pens[11]=0x0; //lamp6
  Machine->pens[12]=0x0;  //lamp7
  Machine->pens[13]=0xf8f0;  //lamp8
  Machine->pens[14]=0xf0f8; //lamp9
  Machine->pens[15]=0xf8f0; //lamp10
  Machine->pens[16]=0xf0f8; //lamp11
  Machine->pens[17]=0xf0f8; //lamp12
  Machine->pens[18]=0xf0f8; //lamp13
  Machine->pens[19]=0xf0f8; //lamp14
  Machine->pens[20]=0xf0f8; //lamp15
  Machine->pens[21]=0x88c0; //segon1
  Machine->pens[22]=0xffc0; //segon2
  Machine->pens[23]=0x2200; //segoff1
  Machine->pens[24]=0x0400; //segoff2
  Machine->pens[25]=0x0400; //
  Machine->pens[26]=0x0400; //
  Machine->pens[27]=0x0400; //
  Machine->pens[28]=0x0400; //
  Machine->pens[29]=0x0400; //
  Machine->pens[30]=0x0ff0; //
  Machine->pens[31]=0x00ff; //

  


  fprintf (stderr, "SDL: machine pens=%d\n", sizeof(Machine->pens));

*/

   return OSD_OK;
}

/*
 *  keyboard remapping routine
 *  invoiced in startup code
 *  returns 0-> success 1-> invalid from or to
 */
static int sdl_mapkey(struct rc_option *option, const char *arg, int priority)
{
   unsigned int from, to;
   /* ultrix sscanf() requires explicit leading of 0x for hex numbers */
   if (sscanf(arg, "0x%x,0x%x", &from, &to) == 2)
   {
      /* perform tests */
      /* fprintf(stderr,"trying to map %x to %x\n", from, to); */
      if (from >= 0 && from < SDL_NUM_SCANCODES  && to >= 0 && to <= 127)
      {
         klookup[from] = to;
	 return OSD_OK;
      }
      /* stderr_file isn't defined yet when we're called. */
      fprintf(stderr,"Invalid keymapping %s. Ignoring...\n", arg);
   }
   return OSD_NOT_OK;
}

/* Update routines */
void sdl_update_16_to_16bpp (struct mame_bitmap *bitmap)
{
 //        SDL_Log("sdl_update_16_to_16bpp\n");
#define SRC_PIXEL  unsigned short
#define DEST_PIXEL unsigned short
#define DEST sdlSurface->pixels
#define DEST_WIDTH Vid_width
   if(current_palette->lookup)
   {
#define INDIRECT current_palette->lookup
#include "blit.h"
#undef INDIRECT
   }
   else
   {
#include "blit.h"
   }
#undef DEST
#undef DEST_WIDTH
#undef SRC_PIXEL
#undef DEST_PIXEL
}

void sdl_update_16_to_24bpp (struct mame_bitmap *bitmap)
{
  //       SDL_Log("sdl_update_16_to_24bpp\n");
#define SRC_PIXEL  unsigned short
#define DEST_PIXEL unsigned int
#define PACK_BITS
#define DEST sdlSurface->pixels
#define DEST_WIDTH Vid_width
   if(current_palette->lookup)
   {
#define INDIRECT current_palette->lookup
#include "blit.h"
#undef INDIRECT
   }
   else
   {
#include "blit.h"
   }
#undef DEST_WIDTH
#undef DEST
#undef PACK_BITS
#undef DEST_PIXEL
#undef SRC_PIXEL
}

void sdl_update_16_to_32bpp (struct mame_bitmap *bitmap)
{
      //   SDL_Log("sdl_update_16_to_32bpp\n");
#define INDIRECT current_palette->lookup
#define SRC_PIXEL unsigned short
#define DEST_PIXEL unsigned int
#define DEST sdlSurface->pixels
#define DEST_WIDTH Vid_width
#include "blit.h"
#undef DEST_WIDTH
#undef DEST
#undef DEST_PIXEL
#undef SRC_PIXEL
#undef INDIRECT
}

void sdl_update_rgb_direct_32bpp(struct mame_bitmap *bitmap)
{
  //    SDL_Log("sdl_update_rgb_direct_32bpp\n");
#define SRC_PIXEL unsigned int
#define DEST_PIXEL unsigned int
#define DEST sdlSurface->pixels
#define DEST_WIDTH Vid_width
#include "blit.h"
#undef DEST_WIDTH
#undef DEST
#undef DEST_PIXEL
#undef SRC_PIXEL
}


void sysdep_update_display(struct mame_bitmap *bitmap)
{
//   SDL_Log("sysdep_update_display");
 // SDL_Log("bitmap: %d %d %d %d", bitmap->width, bitmap->height, bitmap->depth ,bitmap->rowbytes);
   (*update_function)(bitmap);
   SDL_RenderClear(sdlRenderer);
   SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, sdlSurface);
   SDL_RenderCopy(sdlRenderer, texture, NULL, NULL);
   SDL_RenderPresent(sdlRenderer);
} //sysdep_update_display

/* shut up the display */
void sysdep_display_close(void)
{
SDL_Log("sysdep_display_close");
if ( sdlTexture) {
  SDL_DestroyTexture(sdlTexture);
}
SDL_Log("close sdlTexture");
if ( sdlRenderer) {
  SDL_DestroyRenderer(sdlRenderer);
}
SDL_Log("close sdlRenderer");
if (sdlWindow ) {
//	SDL_DestroyWindow(sdlWindow);
}
  // SDL_FreeSurface(Offscreen_surface);
SDL_Log("close sdlWindow");

   /* Restore cursor state */
  // SDL_ShowCursor(cursor_state);
}
/*


/*
 * In 8 bpp we should alloc pallete - some ancient people  
 * are still using 8bpp displays
 */
int sysdep_display_alloc_palette(int totalcolors)
{
   int ncolors;
   int i;
   ncolors = totalcolors;

   fprintf (stderr, "SDL: sysdep_display_alloc_palette(%d);\n",totalcolors);
   if (Vid_depth != 8)
      return 0;

   Colors = (SDL_Color*) malloc (totalcolors * sizeof(SDL_Color));
   if( !Colors )
      return 1;
   for (i=0;i<totalcolors;i++) {
      (Colors + i)->r = 0xFF;
      (Colors + i)->g = 0x00;
      (Colors + i)->b = 0x00;
   }
     for (i=0;i<totalcolors;i++) {
      (Colors + i)->r = (i>>10)&0x1f;
      (Colors + i)->g = (i>>5)&0x1f;
      (Colors + i)->b = i&0x1f;
   }
    SDL_SetPaletteColors(sdlSurface->format->palette, Colors, 0, totalcolors-1);
   //SDL_SetColors (sdlSurface,Colors,0,totalcolors-1);

   fprintf (stderr, "SDL: Info: Palette with %d colors allocated\n", totalcolors);
   return 0;
}



/*
 * In 8 bpp we should alloc pallete - some ancient people  
 * are still using 8bpp displays
 */
int sysdep_display_alloc_palette2(int totalcolors)
{
   int ncolors;
   int i;
   ncolors = totalcolors;

   fprintf (stderr, "SDL: sysdep_display_alloc_palette(%d);\n",totalcolors);
   if (Vid_depth != 8)
      return 0;


   Colors = (SDL_Color*) malloc (totalcolors * sizeof(SDL_Color));
   if( !Colors )
      return 1;
   fprintf (stderr, "SDL: Colors OK(%d);\n",totalcolors);
   for (i=0;i<totalcolors;i++) {
      (Colors + i)->r = (i>>10)&0x1f;
      (Colors + i)->g = (i>>5)&0x1f;
      (Colors + i)->b = i&0x1f;
   }
   //SDL_SetColors (Offscreen_surface,Colors,0,totalcolors-1);
 //1  SDL_SetPaletteColors(Offscreen_surface->format->palette, Colors, 0, totalcolors-1);
      //SDL_Color colors[2] = {{255,0,0,255}, {0,255,0,255}};
   SDL_SetPaletteColors(sdlSurface->format->palette, Colors, 0, 2);
 //SDL_SetPaletteColors(sdlPalette, Colors, 0, totalcolors-1);
 
   //fprintf (stderr, "SDL: Info: Palette with %d colors allocated\n", totalcolors);
   return 0;
}

int sysdep_display_set_pen(int pen,unsigned char red, unsigned char green, unsigned char blue)
{
   static int warned = 0;
#ifdef SDL_DEBUG
   fprintf(stderr,"sysdep_display_set_pen(%d,%d,%d,%d)\n",pen,red,green,blue);
#endif


   if( Colors ) {
      (Colors + pen)->r = red;
      (Colors + pen)->g = green;
      (Colors + pen)->b = blue;
      /* Replace:
SDL_SetColors(screen, color, 0, intColors);

With:
SDL_ SDL_SetPaletteColors(screen->format->palette, color, 0, intColors); 
*/
//1       if ( (!     SDL_SetPaletteColors(Offscreen_surface->format->palette, Colors, 0, 1)) && (! warned)) {
      //if ( (! SDL_SetColors(Offscreen_surface, Colors + pen, pen,1)) && (! warned)) {
 //1        printf ("Color allocation failed, or > 8 bit display\n");
  //1       warned = 0;
   //1   }
     //SDL_SetPaletteColors(sdlPalette, Colors, 0, sizeof(Colors));
   }


#ifdef SDL_DEBUG
   fprintf(stderr, "STD: Debug: Pen %d modification: r %d, g %d, b, %d\n", pen, red,green,blue);
#endif /* SDL_DEBUG */
   return 0;
}

void sysdep_mouse_poll (void)
{
   int i;
   int x,y;
   Uint8 buttons;

   buttons = SDL_GetRelativeMouseState( &x, &y);
   mouse_data[0].deltas[0] = x;
   mouse_data[0].deltas[1] = y;
   for(i=0;i<MOUSE_BUTTONS;i++) {
      mouse_data[0].buttons[i] = buttons & (0x01 << i);
   }
}

/* Keyboard procs */
/* Lighting keyboard leds */
void sysdep_set_leds(int leds) 
{
}

void sysdep_update_keyboard() 
{
   
   struct xmame_keyboard_event kevent;
   SDL_Event event;

   if (sdlWindow) {
      while(SDL_PollEvent(&event)) {
         kevent.press = 0;
         switch (event.type) 
         {
            case SDL_TEXTINPUT:
                 kevent.unicode = event.text.text[0] & 0xff;
                   fprintf(stderr,event.text.text);
               break;
            case SDL_KEYDOWN:
               kevent.press = 1;

               // * ALT-Enter: toggle fullscreen * /
               
               if ( event.key.keysym.sym == SDLK_RETURN )
               {
                  if(event.key.keysym.mod & KMOD_ALT) {
                     //SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
                     SDL_DisplayMode dm;
                     SDL_GetDesktopDisplayMode(0, &dm);
                     SDL_RestoreWindow(sdlWindow); //Incase it's maximized...
                     SDL_SetWindowSize(sdlWindow, dm.w, dm.h + 10);
                     SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                  }
               }
               
              if ( event.key.keysym.sym == SDLK_LSHIFT )
               {
                  fprintf(stderr,"LEFT SHIFT\n");
               }
              if ( event.key.keysym.sym == SDLK_RSHIFT )
               {
                  fprintf(stderr,"RIGHT SHIFT\n");
               }
               
            case SDL_KEYUP:
            //fprintf(stderr,"key1");
               if ( event.key.keysym.sym & (1<<30) ) {
                  kevent.scancode = klookuphigh[event.key.keysym.sym & 0x1ff];
               } else {
                  kevent.scancode = klookup[event.key.keysym.sym & 0x1ff];
               }
               //kevent.scancode = event.key.keysym.sym & 0xff;
          //  fprintf(stderr,"key2");
               #ifdef SDL_DEBUG
            //fprintf(stderr,"key3");
/*
               fprintf (stderr, "Key %s %ssed\n",
                  SDL_GetKeyName(event.key.keysym.sym),
                  kevent.press? "pres":"relea");

                  fprintf(stderr,"scancode: %x\n",kevent.scancode);
               fprintf(stderr,"scancodesh: %x\n",SDLK_LSHIFT);
               fprintf(stderr,"scancodelo: %x\n",klookuphigh[SDLK_LSHIFT & 0x1ff]);
*/
               
#endif
            //fprintf(stderr,"key4");
               //kevent.unicode = event.key.keysym.unicode;
               kevent.unicode = 0;
               xmame_keyboard_register_event(&kevent);
               fprintf(stderr,"scancode: %x\n",kevent.scancode);
               if(!kevent.scancode)
                  fprintf (stderr, "Unknown symbol 0x%x\n",
                     event.key.keysym.sym);

               break; 

            case SDL_QUIT:
               // Should leave this to application 
               sysdep_close();
               exit(OSD_OK);
               break;

    	    case SDL_JOYAXISMOTION:   
               if (event.jaxis.which < JOY_AXIS)
                  joy_data[event.jaxis.which].axis[event.jaxis.axis].val = event.jaxis.value;
#ifdef SDL_DEBUG
               fprintf (stderr,"Axis=%d,value=%d\n",event.jaxis.axis ,event.jaxis.value);
#endif
		break;
	    case SDL_JOYBUTTONDOWN:

	    case SDL_JOYBUTTONUP:
               if (event.jbutton.which < JOY_BUTTONS)
                  joy_data[event.jbutton.which].buttons[event.jbutton.button] = event.jbutton.state;
#ifdef SDL_DEBUG
               fprintf (stderr, "Button=%d,value=%d\n",event.jbutton.button ,event.jbutton.state);
#endif
		break;


          default:
//#ifdef SDL_DEBUG
//               fprintf(stderr, "SDL: Debug: Other event\n");
//#endif // SDL_DEBUG 
               break;
         }
    joy_evaluate_moves ();
      }
   }
   
}

/* added funcions */
int sysdep_display_16bpp_capable(void)
{
   
 //  const SDL_VideoInfo* video_info;
  // video_info = SDL_GetVideoInfo();
   //return ( video_info->vfmt->BitsPerPixel >=16);
   
  return 1;
}

int list_sdl_modes(struct rc_option *option, const char *arg, int priority)
{
   /*
   SDL_Rect** vid_modes;
   int vid_modes_i;
We don't call SDL_ListModes() anymore. There's an equivalent in SDL2 (call SDL_GetDisplayMode() in a loop, SDL_GetNumDisplayModes() times), but instead we're going to use a new feature called "fullscreen desktop," which tells SDL "give me the whole screen and don't change the resolution." For our hypothetical 640x480 game, it might look like this:

SDL_Window *sdlWindow = SDL_CreateWindow(title,
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             0, 0,
                             SDL_WINDOW_FULLSCREEN_DESKTOP);

   vid_modes = SDL_ListModes(NULL,SDL_WINDOW_FULLSCREEN);
   vid_modes_i = 0;

   if ( (! vid_modes) || ((long)vid_modes == -1)) {
      printf("This option only works in a full-screen mode (eg: linux's framebuffer)\n");
      return - 1;
   }

   printf("Modes available:\n");

   while( *(vid_modes+vid_modes_i) ) {
      printf("\t%d) Mode %d x %d\n",
         vid_modes_i,
         (*(vid_modes+vid_modes_i))->w,
         (*(vid_modes+vid_modes_i))->h
         );
   
      vid_modes_i++;
   }
*/
   return -1;
}
