/***************************************************************************
                                          
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

 TODO: 
       Test the 16bpp->24bpp update routine
       Test the 16bpp->32bpp update routine
       Improve performance.
       Test mouse buttons (which games use them?)

***************************************************************************/
/* #define PARANOIC */
#define __SDL_C

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

static int Vid_width;
static int Vid_height;
static int Vid_depth = 16;
//static SDL_Surface* Surface;
static SDL_Surface* Offscreen_surface;
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
//SDL_Texture * texture;

#define QUITKEY SDLK_ESCAPE
#define WIDTH 1024
#define HEIGHT 768
Uint16 * pixels = NULL;


/* returns a number between 1 and max */
int Random(int max) {
	return (rand() % max) + 1;
}

int rectCount = 0;
void DrawRandomRectangle() {
	char buff[20];
	SDL_Rect rect;
	SDL_SetRenderDrawColor(sdlRenderer, Random(256) - 1, Random(256) - 1, Random(256) - 1, 255);
	rect.h = 120;// Random(100) + 20;
	rect.w = 120;// Random(100) + 20;
	rect.y = Random(HEIGHT - rect.h - 1);
	rect.x = Random(WIDTH - rect.w - 1);
	SDL_RenderFillRect(sdlRenderer, &rect);

	rectCount++;
	//if (rectCount % 100000 == 0) {
	//	SDL_RenderPresent(renderer);
	//}
}







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
    pixels[y * Vid_width + x] = (red&0xff)<<24 | (green & 0xff)<<16 | (blue & 0xff)<<8|(o & 0xff) ;
}
void plot(int x,int y ,int red, int green, int blue, int o){
   pixels[y * Vid_width + x] = (red&0x1f)<<11 | (green & 0x2f)<<5 | (blue & 0x1f) ;
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

   if ( pixels ){
      free(pixels);
   }
   //pixels = (Uint32 *) malloc (sizeof(Uint32)*Vid_width*Vid_height);
   pixels = (Uint16 *) malloc (sizeof(Uint16)*Vid_width*Vid_height);
   // grey
   memset(pixels, 255, Vid_width * Vid_height * sizeof(Uint16));

     for(int y=0;y<Vid_height;y++) {
        plot(100,y,255,0,0,255);
        plot(101,y,255,255,0,255);
        plot(102,y,255,128,0,255);
        plot(103,y,255,0,255,255);
        plot(104,y,0,255,0,255);
    }
   if (!sdlWindow) {
   sdlWindow = SDL_CreateWindow(title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Vid_width, Vid_height, 0);
//SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_MAXIMIZED, &sdlWindow, &sdlRenderer);
   }
   if (!sdlWindow) {
		fprintf(stderr,"InitSetup failed to create window");
	}

  //  SDL_Window * window = SDL_CreateWindow("SDL2 Pixel Drawing",
   //     SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, v_width, v_height, 0);

   sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
 //  texture = SDL_CreateTexture(sdlRenderer,
 //       SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, Vid_width, Vid_height);
   sdlTexture = SDL_CreateTexture(sdlRenderer,
        SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STATIC, Vid_width, Vid_height);


//   Surface = SDL_GetWindowSurface(sdlWindow);
//   if (!Surface) {
//      fprintf(stderr,"InitSetup failed to create Surface");
//   }

 /*  Offscreen_surface = SDL_GetWindowSurface(sdlWindow);
   if (!Offscreen_surface) {
      fprintf(stderr,"InitSetup failed to create Offscreen_surface");
      SDL_Quit();
      exit (OSD_NOT_OK);
   }
*/
     /* Offscreen_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,Vid_width,Vid_height,Vid_depth,0,0,0,0); 
   if(Offscreen_surface==NULL) {
      SDL_Quit();
      exit (OSD_NOT_OK);
   }
*/

   //SDL_SetWindowTitle(Surface, "title screen");
	//SDL_SetWindowTitle(sdlWindow, title);
   //SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &Surface, &renderer2);


   SDL_SetRenderDrawColor(sdlRenderer, 255, 0, 0, 255);
   SDL_RenderClear(sdlRenderer);

   SDL_UpdateTexture(sdlTexture, NULL, pixels, Vid_width * sizeof(Uint16));
   SDL_RenderPresent(sdlRenderer);

 
   SDL_Log("SDL w,h,d: %d,%d,%d", Vid_width,Vid_height,Vid_depth);
   SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
   SDL_RenderSetLogicalSize(sdlRenderer, Vid_width, Vid_height);

   SDL_Log( "SDL_BITSPERPIXEL %d",SDL_BITSPERPIXEL(16) );
   //sdlTexture = SDL_CreateTexture(sdlRenderer,
   //                            SDL_PIXELFORMAT_ARGB8888,
   //                            SDL_TEXTUREACCESS_STREAMING,
   //                            Vid_width, Vid_height);
//   SDL_UpdateTexture(sdlTexture, NULL, myPixels, 640 * sizeof (Uint32));

SDL_Event event;
while(SDL_PollEvent(&event)) {}

SDL_Rect rect;
    rect.x = 250;
    rect.y = 150;
    rect.w = 200;
    rect.h = 200;

//SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 255);
//SDL_RenderDrawRect(sdlRenderer, &rect);




//SDL_RenderClear(sdlRenderer);
//SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
//SDL_RenderPresent(sdlRenderer);


/*
static int display_in_use = 0; // Only using first display 

int i, display_mode_count;
SDL_DisplayMode mode;
Uint32 f;

SDL_Log("SDL_GetNumVideoDisplays(): %i", SDL_GetNumVideoDisplays());

display_mode_count = SDL_GetNumDisplayModes(display_in_use);
if (display_mode_count < 1) {
    SDL_Log("SDL_GetNumDisplayModes failed: %s", SDL_GetError());
   exit (OSD_NOT_OK);
}
SDL_Log("SDL_GetNumDisplayModes: %i", display_mode_count);

for (i = 0; i < display_mode_count; ++i) {
    if (SDL_GetDisplayMode(display_in_use, i, &mode) != 0) {
        SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
       exit (OSD_NOT_OK);
    }
    f = mode.format;

    SDL_Log("Mode %i\tbpp %i\t%s\t%i x %i",
            i, SDL_BITSPERPIXEL(f),
            SDL_GetPixelFormatName(f),
            mode.w, mode.h);
}

  int display_count = 0, display_index = 0, mode_index = 0;
    if ((display_count = SDL_GetNumVideoDisplays()) < 1) {
        SDL_Log("SDL_GetNumVideoDisplays returned: %i", display_count);
        exit (OSD_NOT_OK);
    }
    if (SDL_GetDisplayMode(display_index, mode_index, &mode) != 0) {
        SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
        exit (OSD_NOT_OK);
    }

SDL_Log("SDL_GetDisplayMode(0, 0, &mode):\t\t%i bpp\t%i x %i",SDL_BITSPERPIXEL(mode.format), mode.w, mode.h);
   Vid_depth = SDL_BITSPERPIXEL(mode.format);


*/

/*
   SDL_Rect** vid_modes;
   //const SDL_VideoInfo* video_info;
   const SDL_RendererInfo* render_info;
   int vid_modes_i;
   int vid_mode_flag; / * Flag to set the video mode * /

   //video_info = SDL_GetVideoInfo();

#ifdef SDL_DEBUG
   fprintf (stderr,"SDL: create_display(%d): \n",depth);
   fprintf (stderr,"SDL: Info: HW blits %d\n"
      "SDL: Info: SW blits %d\n"
      "SDL: Info: Vid mem %d\n"
      "SDL: Info: Best supported depth %d\n",
      render_info->blit_hw,
      render_info->blit_sw,
      render_info->video_mem,
      render_info->vfmt->BitsPerPixel);
#endif
*/
//   Vid_depth = video_info->vfmt->BitsPerPixel;


/*
   vid_modes = SDL_ListModes(NULL,SDL_FULLSCREEN);
   vid_modes_i = 0;

   hardware = video_info->hw_available;

   if ( (! vid_modes) || ((long)vid_modes == -1)) {
#ifdef SDL_DEBUG
      fprintf (stderr, "SDL: Info: Possible all video modes available\n");
#endif
      Vid_height = visual_height*heightscale;
      Vid_width = visual_width*widthscale;
   } else {
      int best_vid_mode; / * Best video mode found * /
      int best_width,best_height;
      int i;

#ifdef SDL_DEBUG
      fprintf (stderr, "SDL: visual w:%d visual h:%d\n", visual_width, visual_height);
#endif
      best_vid_mode = 0;
      best_width = vid_modes[best_vid_mode]->w;
      best_height = vid_modes[best_vid_mode]->h;
      for (i=0;vid_modes[i];++i)
      {
         int cur_width, cur_height;

         cur_width = vid_modes[i]->w;
         cur_height = vid_modes[i]->h;

#ifdef SDL_DEBUG
         fprintf (stderr, "SDL: Info: Found mode %d x %d\n", cur_width, cur_height);
#endif / * SDL_DEBUG * /

         / * If width and height too small, skip to next mode * /
         if ((cur_width < visual_width*widthscale) || (cur_height < visual_height*heightscale)) {
            continue;
         }

         / * If width or height smaller than current best, keep it * /
         if ((cur_width < best_width) || (cur_height < best_height)) {
            best_vid_mode = i;
            best_width = cur_width;
            best_height = cur_height;
         }
      }

#ifdef SDL_DEBUG
      fprintf (stderr, "SDL: Info: Best mode found : %d x %d\n",
         vid_modes[best_vid_mode]->w,
         vid_modes[best_vid_mode]->h);
#endif / * SDL_DEBUG * /

      vid_modes_i = best_vid_mode;

      / * mode_number is a command line option * /
      if( mode_number != -1) {
         if( mode_number >vid_modes_i)
            fprintf(stderr, "SDL: The mode number is invalid... ignoring\n");
         else
            vid_modes_i = mode_number;
      }
      if( vid_modes_i<0 ) {
         fprintf(stderr, "SDL: None of the modes match :-(\n");
         Vid_height = visual_height*heightscale;
         Vid_width = visual_width*widthscale;
      } else {
         if(*(vid_modes+vid_modes_i)==NULL) 
            vid_modes_i--;

         Vid_width = (*(vid_modes + vid_modes_i))->w;
         Vid_height = (*(vid_modes + vid_modes_i))->h;
      }
   }
*/

   if( depth == 16 )
   {
      switch( Vid_depth ) {
      case 32:
         update_function = &sdl_update_16_to_32bpp;
         break;
      case 24:
         update_function = &sdl_update_16_to_24bpp;
         break;
      case 16:
         update_function = &sdl_update_16_to_16bpp;
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

/*
sdlTexture = SDL_CreateTexture(sdlRenderer,
                                            SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            WIDTH, HEIGHT);
*/
/*

   / * Set video mode according to flags * /
   vid_mode_flag = SDL_HWSURFACE;
   if (start_fullscreen) {
      vid_mode_flag |= SDL_FULLSCREEN;
   }
/

// if all this hex scares you, check out SDL_PixelFormatEnumToMasks()!
SDL_Surface *screen = SDL_CreateRGBSurface(0, 640, 480, 32,
                                        0x00FF0000,
                                        0x0000FF00,
                                        0x000000FF,
                                        0xFF000000);
SDL_Texture *sdlTexture = SDL_CreateTexture(sdlRenderer,
                                            SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            640, 480);

   if(! (Surface = SDL_SetVideoMode(Vid_width, Vid_height,Vid_depth, vid_mode_flag))) {
      fprintf (stderr, "SDL: Error: Setting video mode failed\n");
      SDL_Quit();
      exit (OSD_NOT_OK);
   } else {
      fprintf (stderr, "SDL: Info: Video mode set as %d x %d, depth %d\n", Vid_width, Vid_height, Vid_depth);
   }
*/

  /* Offscreen_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,Vid_width,Vid_height,Vid_depth,0,0,0,0); 
   if(Offscreen_surface==NULL) {
      SDL_Quit();
      exit (OSD_NOT_OK);
   }
*/

   // * Creating event mask * /
   SDL_EventState(SDL_KEYUP, SDL_ENABLE);
   SDL_EventState(SDL_KEYDOWN, SDL_ENABLE);
   //SDL_EnableUNICODE(1);
   

    // * fill the display_palette_info struct * /
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

/*
   / * Hide mouse cursor and save its previous status * /
   cursor_state = SDL_ShowCursor(0);
   */
   // Set window title 
   //SDL_WM_SetCaption(title, NULL);
   //SDL_SetWindowTitle(title, NULL);
   effect_init2(depth, Vid_depth, Vid_width);

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
#define SRC_PIXEL  unsigned short
#define DEST_PIXEL unsigned short
#define DEST pixels
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
#define SRC_PIXEL  unsigned short
#define DEST_PIXEL unsigned int
#define PACK_BITS
#define DEST pixels
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
#define INDIRECT current_palette->lookup
#define SRC_PIXEL unsigned short
#define DEST_PIXEL unsigned int
#define DEST pixels
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
#define SRC_PIXEL unsigned int
#define DEST_PIXEL unsigned int
#define DEST pixels
#define DEST_WIDTH Vid_width
#include "blit.h"
#undef DEST_WIDTH
#undef DEST
#undef DEST_PIXEL
#undef SRC_PIXEL
}


void sysdep_update_display(struct mame_bitmap *bitmap)
{
   //SDL_Log("sysdep_update_display");
   (*update_function)(bitmap);
   SDL_UpdateTexture(sdlTexture, NULL, pixels, Vid_width * sizeof(Uint16));
   SDL_RenderClear(sdlRenderer);
   SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
   SDL_RenderPresent(sdlRenderer);
} //sysdep_update_display

/* shut up the display */
void sysdep_display_close(void)
{
SDL_Log("sysdep_display_close");
if ( sdlRenderer) {
  SDL_DestroyRenderer(sdlRenderer);
}
if (sdlWindow ) {
//	SDL_DestroyWindow(sdlWindow);
}
  // SDL_FreeSurface(Offscreen_surface);

   /* Restore cursor state */
   SDL_ShowCursor(cursor_state);
}

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
   //SDL_SetColors (Offscreen_surface,Colors,0,totalcolors-1);
   SDL_SetPaletteColors(Offscreen_surface->format->palette, Colors, 0, totalcolors-1);

   fprintf (stderr, "SDL: Info: Palette with %d colors allocated\n", totalcolors);
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
       if ( (!     SDL_SetPaletteColors(Offscreen_surface->format->palette, Colors, 0, 1)) && (! warned)) {
      //if ( (! SDL_SetColors(Offscreen_surface, Colors + pen, pen,1)) && (! warned)) {
         printf ("Color allocation failed, or > 8 bit display\n");
         warned = 0;
      }
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
/*
while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				keypressed = event.key.keysym.sym;
				if (keypressed == QUITKEY)
				{
					gameRunning = 0;
					break;
				}

				break;
			case SDL_QUIT: / * if mouse click to close window * /
			{
				gameRunning = 0;
				break;
			}
			case SDL_KEYUP: {
				break;
			}
			} / * switch * /

		} / * while SDL_PollEvent * /
      */
void sysdep_update_keyboard() 
{
   
   struct xmame_keyboard_event kevent;
   SDL_Event event;

   if (sdlWindow) {
      while(SDL_PollEvent(&event)) {
         kevent.press = 0;
         switch (event.type) 
         {
//            case SDL_TEXTINPUT:
 //              kevent.unicode = event.text.text[0] & 0xff;
  //             break;
            case SDL_KEYDOWN:
               kevent.press = 1;

               // * ALT-Enter: toggle fullscreen * /
               /*
               if ( event.key.keysym.sym == SDLK_RETURN )
               {
                  if(event.key.keysym.mod & KMOD_ALT)
                     SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
               }
               */
              if ( event.key.keysym.sym == SDLK_LSHIFT )
               {
                  fprintf(stderr,"LEFT SHIFT");
               }
              if ( event.key.keysym.sym == SDLK_RSHIFT )
               {
                  fprintf(stderr,"RIGHT SHIFT");
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
               fprintf (stderr, "Key %s %ssed\n",
                  SDL_GetKeyName(event.key.keysym.sym),
                  kevent.press? "pres":"relea");
#endif
            //fprintf(stderr,"key4");
               //kevent.unicode = event.key.keysym.unicode;
               kevent.unicode = 0;
               xmame_keyboard_register_event(&kevent);
            //fprintf(stderr,"key5");
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
   /*
   const SDL_VideoInfo* video_info;
   video_info = SDL_GetVideoInfo();
   return ( video_info->vfmt->BitsPerPixel >=16);
   */
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
