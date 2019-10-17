#include "wolf.h"

int         get_map(t_mlx *stuff, int nb)
{
    int     ret = 0;
    if (stuff->map.matrix)
    {
        free(stuff->map.matrix);
        stuff->map.matrix = NULL;
    }
    std::string files[1] = { "wolf_data/maps/good.map" };
    if (nb > -1 && nb < 1)
        ret = create_map(stuff, files[nb]);
    if (ret != 1)
        return (0);
    return (1);
}

t_RGB color_converter (int hexValue)
{
    t_RGB rgbColor;
    rgbColor.r = hexValue >> 16;//((hexValue >> 16) & 0xFF) / 255.0; // Extract the RR byte
    rgbColor.g = (hexValue & 0x00ff00) >> 8;//((hexValue >> 8) & 0xFF) / 255.0; // Extract the GG byte
    rgbColor.b = (hexValue & 0x0000ff);//((hexValue) & 0xFF) / 255.0; // Extract the BB byte
    return (rgbColor); 
}

/*
void draw_line(t_mlx *stuff, SDL_Renderer *renderer, int x1, int y1, int x2, int y2)
{
    t_RGB c = color_converter(0);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
    //SDL_RenderDrawLine(renderer, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
*/

void wolf_init(t_mlx *stuff)//SDL_Surface **surface, SDL_Texture **texture, SDL_Window **window, SDL_Renderer **renderer)
{

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        exit (-1);
    }

    // create an SDL window (OpenGL ES2 always enabled)
    // when SDL_FULLSCREEN flag is not set, viewport is automatically handled by SDL (use SDL_SetWindowSize to "change resolution")
    // available switch SDL2 video modes :
    // 1920 x 1080 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    // 1280 x 720 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    stuff->sdl_data.window = SDL_CreateWindow("sdl2_gles2", 0, 0, 1280, 720, 0);
    if (!stuff->sdl_data.window) {
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        exit (-1);
    }

    // create a renderer (OpenGL ES2)
    stuff->sdl_data.renderer = SDL_CreateRenderer(stuff->sdl_data.window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!stuff->sdl_data.renderer) {
        SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(stuff->sdl_data.window);
        SDL_Quit();
        exit (-1);
    }

    stuff->sdl_data.surface = SDL_CreateRGBSurface(0, 1280, 720, 32, 0, 0, 0, 0);
    if (!stuff->sdl_data.surface)
    {
        SDL_Log("SDL_CreateRGBSurface: %s\n", SDL_GetError());
        SDL_DestroyRenderer(stuff->sdl_data.renderer);
        SDL_DestroyWindow(stuff->sdl_data.window);
        SDL_Quit();
        exit (-1);
    }

    stuff->sdl_data.win_texture = SDL_CreateTextureFromSurface(stuff->sdl_data.renderer, stuff->sdl_data.surface);
    if (!stuff->sdl_data.win_texture)
    {
        SDL_Log("SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        SDL_FreeSurface(stuff->sdl_data.surface);
        SDL_DestroyRenderer(stuff->sdl_data.renderer);
        SDL_DestroyWindow(stuff->sdl_data.window);
        SDL_Quit();
        exit (-1);
    }
    //IMG_Init(IMG_INIT_XPM);
    // open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
    // when railed, both joycons are mapped to joystick #0,
    // else joycons are individually mapped to joystick #0, joystick #1, ...
    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
    for (int i = 0; i < 2; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
            SDL_DestroyTexture(stuff->sdl_data.win_texture);
            SDL_FreeSurface(stuff->sdl_data.surface);
            SDL_DestroyRenderer(stuff->sdl_data.renderer);
            SDL_DestroyWindow(stuff->sdl_data.window);
            SDL_Quit();
            exit (-1);
        }
    }

}

int main(int argc, char *argv[])
{
    /*
    SDL_Event       event;
    SDL_Window      *window;
    SDL_Renderer    *renderer;
    SDL_Surface     *surface;
    SDL_Texture     *texture;
    */
    t_mlx           stuff;

    int done = 0, w = 1280, h = 720;
    // mandatory at least on switch, else gfx is not properly closed
    wolf_init(&stuff);//&surface, &texture, &window, &renderer);
    int chosen_map = 0;
    stuff.h = h;
    stuff.w = w;
    stuff.map.matrix = NULL;
    stuff.player.x = 0;
    stuff.player.y = 0;
    stuff.player.cam.x = 0.378560f;
	stuff.player.cam.y = -0.540640f;
	stuff.player.dir.x = 0.819152f;
	stuff.player.dir.y = 0.573576f;

	stuff.player.movespeed = 0.1f;
    if (!(get_map(&stuff, chosen_map)))
        done = 1;
    SDL_SetRenderDrawColor(stuff.sdl_data.renderer, 0, 0, 0, 255);
    SDL_RenderClear(stuff.sdl_data.renderer);
    SDL_RenderPresent(stuff.sdl_data.renderer);
    while (!done) {
        while (SDL_PollEvent(&stuff.sdl_data.event)) {
            SDL_Event   *event = &stuff.sdl_data.event;
            switch (event->type) {
                case SDL_JOYAXISMOTION:
                    SDL_Log("Joystick %d axis %d value: %d\n",
                            event->jaxis.which,
                            event->jaxis.axis, event->jaxis.value);
                    if( event->jaxis.which == 0 )
                    {
                        const int JOYSTICK_DEAD_ZONE = 14000;
                        //X axis motion
                        if( event->jaxis.axis == 0 )
                        {
                            //Left of dead zone
                            if( event->jaxis.value < -JOYSTICK_DEAD_ZONE )
                            {
                                rotate_player(3.0f / 180.0f * M_PI, &stuff);
                            }
                            //Right of dead zone
                            else if( event->jaxis.value > JOYSTICK_DEAD_ZONE )
                            {
                                rotate_player(-3.0f / 180.0f * M_PI, &stuff);
                            }
                        }
                        else if( event->jaxis.axis == 1 )
                        {
                                //Below of dead zone
                                if( event->jaxis.value < -JOYSTICK_DEAD_ZONE )
                                {
                                    move_player(&stuff, stuff.player.movespeed);
                                }
                                //Above of dead zone
                                else if( event->jaxis.value > JOYSTICK_DEAD_ZONE )
                                {
                                    move_player(&stuff, -stuff.player.movespeed);
                                }
                        }
                    }

                    break;

                case SDL_JOYBUTTONDOWN:
                    SDL_Log("Joystick %d button %d down\n",
                            event->jbutton.which, event->jbutton.button);
                    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
                    // seek for joystick #0
                    if (event->jbutton.which == 0) {
                        if (event->jbutton.button == 0) {
                            /*
                            // (A) button down, switch resolution
                            if(w == 1920) {
                                SDL_SetWindowSize(stuff.sdl_data.window, 1280, 720);
                            }
                            else {
                                SDL_SetWindowSize(stuff.sdl_data.window, 1920, 1080);
                            }
                            */
                            stuff.h = h;
                            stuff.w = w;
                        }
                        else if (event->jbutton.button == 10) {
                            // (+) button down
                            done = 1;
                        }
                        else if (event->jbutton.button == 11) {
                            // (-) button down
                            chosen_map = (chosen_map + 1 >= MAP_NB) ? 0 : chosen_map + 1;
                            if (!(get_map(&stuff, chosen_map)))
                            {
                                if (stuff.map.matrix)
                                    free(stuff.map.matrix);
                                SDL_DestroyTexture(stuff.sdl_data.win_texture);
                                /*
                                for (int i = TEXTURE_NB; i >= 1; i--)
				                {
					                SDL_FreeSurface(stuff.sdl_data.obj_surface[i]);					                i--;
				                }
                                */
                                SDL_FreeSurface(stuff.sdl_data.surface);
                                SDL_DestroyRenderer(stuff.sdl_data.renderer);
                                SDL_DestroyWindow(stuff.sdl_data.window);
                                SDL_Quit();
                                exit(0);
                            }
                        }
                    }
                    break;

                default:
                    break;
            }
        }
        SDL_GetWindowSize(stuff.sdl_data.window, &w, &h);
        stuff.pixels = (uint32_t *)stuff.sdl_data.surface->pixels;
        memset(stuff.pixels, 0, w * h * sizeof(uint32_t));

        wolf_draw(&stuff);

        SDL_UpdateTexture(stuff.sdl_data.win_texture, nullptr, stuff.sdl_data.surface->pixels, stuff.sdl_data.surface->pitch);
        SDL_RenderClear(stuff.sdl_data.renderer);
        SDL_RenderCopy(stuff.sdl_data.renderer, stuff.sdl_data.win_texture, nullptr, nullptr);
        SDL_RenderPresent(stuff.sdl_data.renderer);
    }
    if (stuff.map.matrix)
        free(stuff.map.matrix);
    SDL_DestroyTexture(stuff.sdl_data.win_texture);
    /*for (int i = TEXTURE_NB; i >= 1; i--)
	{
	    SDL_FreeSurface(stuff.sdl_data.obj_surface[i]);					                i--;
	}
    */
    SDL_FreeSurface(stuff.sdl_data.surface);
    SDL_DestroyRenderer(stuff.sdl_data.renderer);
    SDL_DestroyWindow(stuff.sdl_data.window);
    SDL_Quit();

    return 0;
}
